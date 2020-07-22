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

#ifndef CACHE_H
#define CACHE_H

#include "../machineconfig.h"
#include "frontend_memory.h"

#include <cstdint>
#include <ctime>


namespace machine {
struct cache_data {
    bool valid, dirty;
    std::uint32_t tag;
    std::uint32_t *data;
};

struct CacheLocation {
    uint32_t row;
    uint32_t col;
    uint32_t tag;
};


class Cache : public FrontendMemory {

    Q_OBJECT

public:
    Cache(
        FrontendMemory *m,
        const CacheConfig *c,
        unsigned memory_access_penalty_r = 1,
        unsigned memory_access_penalty_w = 1,
        unsigned memory_access_penalty_b = 0
    );

    ~Cache() override;

    bool write(Address address, std::uint32_t value) override;

    std::uint32_t read( // TODO
        Address address,
        bool debug_access = false
    ) const override;

    std::uint32_t get_change_counter() const override;

    void flush();         // flush cache
    void sync() override; // Same as flush

    unsigned get_hit_count() const;   // Number of recorded hits
    unsigned get_miss_count() const;  // Number of recorded misses
    unsigned get_read_count() const;  // Number backing/main memory reads
    unsigned get_write_count() const; // Number backing/main memory writes
    unsigned get_stall_count()
        const; // Number of wasted get_cycle_count in memory waiting statistic
    double get_speed_improvement()
        const;                   // Speed improvement in percents in comare with no used cache
    double get_hit_rate() const; // Usage efficiency in percents

    void reset(); // Reset whole state of cache

    const CacheConfig &get_config() const;

    enum LocationStatus location_status(Address address) const override;

signals:

    void hit_update(unsigned) const;

    void miss_update(unsigned) const;

    void statistics_update(
        unsigned stalled_cycles,
        double speed_improv,
        double hit_rate) const;

    void cache_update(
        unsigned associat,
        unsigned set,
        unsigned col,
        bool valid,
        bool dirty,
        std::uint32_t tag,
        const std::uint32_t *data,
        bool write) const;

    void memory_writes_update(unsigned) const;

    void memory_reads_update(unsigned) const;

private:
    CacheConfig cache_config;
    FrontendMemory *mem;
    unsigned access_pen_r, access_pen_w, access_pen_b;
    Address uncached_start = Address(0); // Todo this is wierd
    Address uncached_last = Address(0);

    mutable struct cache_data **dt;

    union {
        unsigned int **lru; // Access time
        unsigned **lfu;     // Access count
    } replc;                // Data used for replacement policy

    mutable unsigned hit_read, miss_read, hit_write,
        miss_write; // Hit and miss counters
    mutable unsigned mem_reads, mem_writes, burst_reads,
        burst_writes; // Dirrect access to memory
    mutable std::uint32_t change_counter;

    std::uint32_t debug_rword(Address address) const;

    bool access(
        Address address,
        std::uint32_t *data,
        bool write,
        std::uint32_t value = 0) const;

    void kick(unsigned associativity_index, unsigned row) const;

    Address base_address(std::uint32_t tag, unsigned row) const;

    void update_statistics() const;

    inline void compute_row_col_tag(
        std::uint32_t &row,
        std::uint32_t &col,
        std::uint32_t &tag,
        Address address) const;

    constexpr inline CacheLocation
    compute_location(const Address address) const {
        uint32_t byte_index = address.get_byte_index();
        std::uint32_t ssize =
            cache_config.blocks() * cache_config.sets(); // Todo: size of what
        uint32_t index = byte_index % ssize;             // TODO: index of what
        return {
            .row = index / cache_config.blocks(),
            .col = index % cache_config.blocks(),
            .tag = byte_index / ssize,
        };
    }
};
} // namespace machine

#endif // CACHE_H
