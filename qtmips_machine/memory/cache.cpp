// SPDX-License-Identifier: GPL-2.0+
/*******************************************************************************
 * QtMips - MIPS 32-bit Architecture Subset Simulator
 *
 * Implemented to support following courses:
 *
 *   B35APO - Computer Architectures
 *   https://cw.fel.cvut.cz/wiki/courses/b35apo
 *
 *   B4M35PAP - Advanced Computer Architectures
 *   https://cw.fel.cvut.cz/wiki/courses/b4m35pap/start
 *
 * Copyright (c) 2017-2019 Karel Koci<cynerd@email.cz>
 * Copyright (c) 2019      Pavel Pisa <pisa@cmp.felk.cvut.cz>
 *
 * Faculty of Electrical Engineering (http://www.fel.cvut.cz)
 * Czech Technical University        (http://www.cvut.cz/)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301, USA.
 *
 ******************************************************************************/

#include "cache.h"

using namespace machine;

Cache::Cache(FrontendMemory  *m, const CacheConfig *cc, unsigned memory_access_penalty_r,
             unsigned memory_access_penalty_w, unsigned memory_access_penalty_b)
             : cache_config(cc) {
    mem = m;
    access_pen_r = memory_access_penalty_r;
    access_pen_w = memory_access_penalty_w;
    access_pen_b = memory_access_penalty_b;
    uncached_start = 0xf0000000_addr;
    uncached_last = 0xffffffff_addr;
    // Zero hit and miss rate
    hit_read = 0;
    hit_write = 0;
    miss_read = 0;
    miss_write = 0;
    mem_reads = 0;
    mem_writes = 0;
    burst_reads = 0;
    burst_writes = 0;
    dt = nullptr;
    replc.lfu = nullptr;
    replc.lru = nullptr;
    change_counter = 0;

    // Skip any other initialization if cache is disabled
    if (!cc->enabled())
        return;

    // Allocate cache data structure
    dt = new struct cache_data*[cc->associativity()];
    for (unsigned i = 0; i < cc->associativity(); i++) {
        dt[i] = new cache_data[cc->sets()];
        for (unsigned y = 0; y < cc->sets(); y++) {
            dt[i][y].valid = false;
            dt[i][y].dirty = false;
            dt[i][y].data = new std::uint32_t[cc->blocks()];
        }
    }
    // Allocate replacement policy data
    switch (cache_config.replacement_policy()) {
    case CacheConfig::RP_LFU:
        replc.lfu = new unsigned *[cache_config.sets()];
        for (unsigned int row = 0; row < cache_config.sets(); row++) {
            replc.lfu[row] = new unsigned[cache_config.associativity()];
            for (unsigned int  i = 0; i < cache_config.associativity(); i++)
                 replc.lfu[row][i] = 0;
        }
        break;
    case CacheConfig::RP_LRU:
        replc.lru = new unsigned int*[cache_config.sets()];
        for (unsigned int row = 0; row < cache_config.sets(); row++) {
            replc.lru[row] = new unsigned int[cache_config.associativity()];
            for (unsigned int i = 0; i < cache_config.associativity(); i++)
                replc.lru[row][i] = i;
        }
        break;
    case CacheConfig::RP_RAND:
    default:
        break;
    }
}

Cache::~Cache(){
    if (dt != nullptr) {
        for (unsigned i = 0; i < cache_config.associativity(); i++) {
	    if (dt[i]) {
                for (unsigned y = 0; y < cache_config.sets(); y++)
                    delete[] dt[i][y].data;
                delete[] dt[i];
            }
	}
        delete[] dt;
    }
    switch (cache_config.replacement_policy()) {
    case CacheConfig::RP_LFU:
        if (replc.lfu == nullptr)
            break;
        for (unsigned row = 0; row < cache_config.sets(); row++)
            delete[] replc.lfu[row];
        delete [] replc.lfu;
        break;
    case CacheConfig::RP_LRU:
        if (replc.lru == nullptr)
            break;
        for (unsigned row = 0; row < cache_config.sets(); row++)
            delete[] replc.lru[row];
        delete[] replc.lru;
    default:
        break;
    }
}


bool Cache::write(Address address, std::uint32_t value) {
    bool changed;

    if (!cache_config.enabled() ||
        (address >= uncached_start && address <= uncached_last)) {
        mem_writes++;
        emit memory_writes_update(mem_writes);
        update_statistics();
        return mem->write_word(address, value);
    }

    std::uint32_t data;
    changed = access(address, &data, true, value);

    if (cache_config.write_policy() != CacheConfig::WP_BACK) {
        mem_writes++;
        emit memory_writes_update(mem_writes);
        update_statistics();
        return mem->write_word(address, value);
    }
    return changed;
}

std::uint32_t Cache::read(Address address, bool debug_access) const {
    if (!cache_config.enabled() ||
        (address >= uncached_start && address <= uncached_last)) {
        mem_reads++;
        emit memory_reads_update(mem_reads);
        update_statistics();
        return mem->read_word(address, debug_access);
    }

    if (debug_access) {
        if (!(location_status(address) & LOCSTAT_CACHED))
            return mem->read_word(address, debug_access);
        return debug_rword(address);
    }
    std::uint32_t data;
    access(address, &data, false);
    return data;
}

std::uint32_t Cache::get_change_counter() const {
    return change_counter;
}

void Cache::flush() {
    if (!cache_config.enabled())
        return;

    for (unsigned as = cache_config.associativity(); as-- > 0 ; )
        for (unsigned st = 0; st < cache_config.sets(); st++)
            if (dt[as][st].valid) {
                kick(as, st);
                emit cache_update(as, st, 0, false, false, 0, 0, false);
            }
    change_counter++;
    update_statistics();
}

void Cache::sync() {
    flush();
}

unsigned Cache::get_hit_count() const {
    return hit_read + hit_write;
}

unsigned Cache::get_miss_count() const {
    return miss_read + miss_write;
}

unsigned Cache::get_read_count() const {
    return mem_reads;
}

unsigned Cache::get_write_count() const {
    return mem_writes;
}

unsigned Cache::get_stall_count() const {
    unsigned st_cycles = mem_reads * (access_pen_r - 1) + mem_writes * (access_pen_w - 1);
    if (access_pen_b != 0)
        st_cycles -= burst_reads * (access_pen_r - access_pen_b) +
                     burst_writes * (access_pen_w - access_pen_b);
    return st_cycles;
}

double Cache::get_speed_improvement() const {
    unsigned lookup_time;
    unsigned mem_access_time;
    unsigned comp = hit_read + hit_write + miss_read + miss_write;
    if (comp == 0)
        return 100.0;
    lookup_time = hit_read + miss_read;
    if (cache_config.write_policy() == CacheConfig::WP_BACK)
        lookup_time += hit_write + miss_write;
    mem_access_time = mem_reads * access_pen_r + mem_writes * access_pen_w;
    if (access_pen_b != 0)
        mem_access_time -= burst_reads * (access_pen_r - access_pen_b) +
                           burst_writes * (access_pen_w - access_pen_b);
    return (double)((miss_read + hit_read) * access_pen_r + (miss_write + hit_write) * access_pen_w) \
            / (double)(lookup_time + mem_access_time) \
            * 100;
}

double Cache::get_hit_rate() const {
    unsigned comp = hit_read + hit_write + miss_read + miss_write;
    if (comp == 0)
        return 0.0;
    return (double)(hit_read + hit_write) / (double)comp * 100.0;
}

void Cache::reset() {
    // Set all cells to invalid
    if (cache_config.enabled()) {
        for (unsigned as = 0; as < cache_config.associativity(); as++)
            for (unsigned st = 0; st < cache_config.sets(); st++)
                dt[as][st].valid = false;
    }
    // Note: we don't have to zero replacement policy data as those are zeroed when first used on invalid cell
    // Zero hit and miss rate
    hit_read = 0;
    hit_write = 0;
    miss_read = 0;
    miss_write = 0;
    mem_reads = 0;
    mem_writes = 0;
    burst_reads = 0;
    burst_writes = 0;
    // Trigger signals
    emit hit_update(get_hit_count());
    emit miss_update(get_miss_count());
    emit memory_reads_update(get_read_count());
    emit memory_writes_update(get_write_count());
    update_statistics();
    if (cache_config.enabled()) {
        for (unsigned as = 0; as < cache_config.associativity(); as++)
            for (unsigned st = 0; st < cache_config.sets(); st++)
                emit cache_update(as, st, 0, false, false, 0, 0, false);
    }
}

const CacheConfig &Cache::get_config() const {
    return cache_config;
}

enum LocationStatus Cache::location_status(Address address) const {
    std::uint32_t row, col, tag;
    compute_row_col_tag(row, col, tag, address);

    if (cache_config.enabled()) {
        for (unsigned indx = 0; indx < cache_config.associativity(); indx++) {
            if (dt[indx][row].valid && dt[indx][row].tag == tag) {
                if (dt[indx][row].dirty &&
                    cache_config.write_policy() == CacheConfig::WP_BACK)
                    return (enum LocationStatus)(LOCSTAT_CACHED | LOCSTAT_DIRTY);
                else
                    return (enum LocationStatus)LOCSTAT_CACHED;
            }
        }
    }
    return mem->location_status(address);
}

std::uint32_t Cache::debug_rword(Address address) const {
    std::uint32_t row, col, tag;
    compute_row_col_tag(row, col, tag, address);
    for (unsigned indx = 0; indx < cache_config.associativity(); indx++)
        if (dt[indx][row].valid && dt[indx][row].tag == tag)
            return dt[indx][row].data[col];
    return 0;
}

bool Cache::access(Address address, std::uint32_t *data, bool write, std::uint32_t value) const {
    bool changed = false;
    CacheLocation loc = compute_location(address);

    unsigned index = 0;
    // Try to locate exact block
    while (index < cache_config.associativity() && (!dt[index][loc.row].valid || dt[index][loc.row].tag != loc.tag))
        index++;
    // Need to find new block
    if (index >= cache_config.associativity()) {
        // if write through we do not need to alloecate cache line does not allocate
        if (write && cache_config.write_policy() == CacheConfig::WP_THROUGH_NOALLOC) {
            miss_write++;
            emit miss_update(get_miss_count());
            update_statistics();
            return false;
        }
        // We have to kick something
        switch (cache_config.replacement_policy()) {
        case CacheConfig::RP_RAND:
            index = rand() % cache_config.associativity();
            break;
        case CacheConfig::RP_LRU:
            {
                index = replc.lru[loc.row][0];
                break;
            }
        case CacheConfig::RP_LFU:
            {
                unsigned lowest = replc.lfu[loc.row][0];
                index = 0;
                for (unsigned i = 1; i < cache_config.associativity(); i++) {
                    if (!dt[i][loc.row].valid) {
                        index = i;
                        break;
                    }
                    if (lowest > replc.lfu[loc.row][i]) {
                        lowest = replc.lfu[loc.row][i];
                        index  = i;
                    }
                }
                break;
            }
        }
    }
    SANITY_ASSERT(index < cache_config.associativity(), "Probably unimplemented replacement policy");

    struct cache_data &cd = dt[index][loc.row];

    // Verify if we are not replacing
    if (cd.valid && cd.tag != loc.tag) {
        kick(index, loc.row);
        change_counter++;
    }

    // Update statistics and otherwise read from memory
    if (cd.valid) {
        if (write)
            hit_write++;
        else
            hit_read++;
        emit hit_update(get_hit_count());
        update_statistics();
    } else {
        if (write)
            miss_write++;
        else
            miss_read++;
        emit miss_update(get_miss_count());
        for (unsigned i = 0; i < cache_config.blocks(); i++) {
            cd.data[i] = mem->read_word(base_address(loc.tag, loc.row) + (4*i));
            change_counter++;
        }
        mem_reads += cache_config.blocks();
        burst_reads += cache_config.blocks() - 1;
        emit memory_reads_update(mem_reads);
        update_statistics();
    }

    // Update replcement data
    switch (cache_config.replacement_policy()) {
    case CacheConfig::RP_LRU:
    {
        unsigned next_asi = index;
        int i = cache_config.associativity() - 1;
        unsigned tmp_asi = replc.lru[loc.row][i];
        while (tmp_asi != index) {
            SANITY_ASSERT(i >= 0, "LRU lost the way from priority queue - access");
            tmp_asi = replc.lru[loc.row][i];
            replc.lru[loc.row][i] = next_asi;
            next_asi = tmp_asi;
            i--;
        }
        break;
    }
    case CacheConfig::RP_LFU:
        if (cd.valid)
            replc.lfu[loc.row][index]++;
        else
            replc.lfu[loc.row][index] = 0;
        break;
    default:
        break;
    }

    cd.valid = true; // We either write to it or we read from memory. Either way it's valid when we leave Cache class
    cd.dirty = cd.dirty || write;
    cd.tag = loc.tag;
    *data = cd.data[loc.col];

    if (write) {
        changed = cd.data[loc.col] != value;
        cd.data[loc.col] = value;
    }

    emit cache_update(index, loc.row, loc.col, cd.valid, cd.dirty, cd.tag, cd.data, write);
    if (changed)
        change_counter++;
    return changed;
}

void Cache::kick(unsigned associativity_index, unsigned row) const {
    struct cache_data &cd = dt[associativity_index][row];
    if (cd.dirty && cache_config.write_policy() == CacheConfig::WP_BACK) {
        for (unsigned i = 0; i < cache_config.blocks(); i++)
            mem->write_word(base_address(cd.tag, row) + (4 * i), cd.data[i]);
        mem_writes += cache_config.blocks();
        burst_writes += cache_config.blocks() - 1;
        emit memory_writes_update(mem_writes);
    }
    cd.valid = false;
    cd.dirty = false;

    switch (cache_config.replacement_policy()) {
    case CacheConfig::RP_LRU:
    {
        unsigned next_asi = associativity_index;
        unsigned tmp_asi = replc.lru[row][0];
        int i = 1;
        while (tmp_asi != associativity_index) {
            SANITY_ASSERT(i < (int)cache_config.associativity(), "LRU lost the way from priority queue - kick");
            tmp_asi = replc.lru[row][i];
            replc.lru[row][i] = next_asi;
            next_asi = tmp_asi;
            i++;
        }
        break;
    }
    case CacheConfig::RP_LFU:
        replc.lfu[row][associativity_index] = 0;
        break;
    default:
        break;
    }
}

Address Cache::base_address(std::uint32_t tag, unsigned row) const {
    return Address(((tag * cache_config.blocks() * cache_config.sets()) + (row * cache_config.blocks())) << 2);
}

void Cache::update_statistics() const {
    emit statistics_update(get_stall_count(), get_speed_improvement(), get_hit_rate());
}

void Cache::compute_row_col_tag(
    uint32_t &row,
    uint32_t &col,
    uint32_t &tag,
    Address address
) const {
    uint32_t offset = address.get_byte_index();
    std::uint32_t ssize = cache_config.blocks() * cache_config.sets();
    tag = offset / ssize;
    std::uint32_t index = offset % ssize;
    row = index / cache_config.blocks();
    col = index % cache_config.blocks();
}
