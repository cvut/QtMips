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
 * Copyright (c) 2020      Jakub Dupak <dupak.jakub@gmail.com>
 * Copyright (c) 2020      Max Hollmann <hollmmax@fel.cvut.cz>
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

#ifndef QTMIPS_MACHINE_MEMORY_H
#define QTMIPS_MACHINE_MEMORY_H

#include "backend_memory.h"
#include "../address.h"

namespace machine {


class MemorySection : public BackendMemory {
public:
    explicit MemorySection(std::uint32_t length);
    MemorySection(const MemorySection&);
    ~MemorySection() override;

    bool write(Offset offset, AccessSize size, AccessItem value) override;

    AccessItem read(Offset offset, AccessSize size, bool debug_read) const override;
//    void merge(MemorySection&);

    std::uint32_t length() const;
    const std::uint32_t* data() const;

    bool operator==(const MemorySection&) const;
    bool operator!=(const MemorySection&) const;

private:
    std::uint32_t len;
    std::uint32_t *dt;
};

union MemoryTree {
    union MemoryTree *mt;
    MemorySection *sec;
};

class Memory : public BackendMemory {
    Q_OBJECT
public:
    Memory();
    Memory(const Memory&);
    ~Memory() override;
    void reset(); // Reset whole content of memory (removes old tree and creates new one)
    void reset(const Memory&);

    MemorySection *get_section(std::uint32_t address, bool create) const; // returns section containing given address

    bool write(Offset offset, AccessSize size, AccessItem value) override;
    AccessItem read(Offset offset, AccessSize size, bool debug_read = false) const override;

    bool operator==(const Memory&) const;
    bool operator!=(const Memory&) const;

    const union MemoryTree *get_memorytree_root() const;

private:
    union MemoryTree *mt_root;
    std::uint32_t change_counter;
    std::uint32_t write_counter;
    static union MemoryTree *allocate_section_tree();
    static void free_section_tree(union MemoryTree*, size_t depth);
    static bool compare_section_tree(const union MemoryTree*, const union MemoryTree*, size_t depth);
//    static bool is_zero_section_tree(const union MemoryTree*, size_t depth);
    static union MemoryTree *copy_section_tree(const union MemoryTree*, size_t depth);
};
};

#endif //QTMIPS_MACHINE_MEMORY_H
