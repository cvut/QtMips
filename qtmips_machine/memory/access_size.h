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

#ifndef QTMIPS_MACHINE_ACCESS_SIZE_H
#define QTMIPS_MACHINE_ACCESS_SIZE_H

enum AccessSize {
    BYTE,
    HWORD,
    WORD,
    DWORD,
};

//union AccessItem {
//    uint8_t byte;
//    uint16_t hword;
//    uint32_t word;
//    uint64_t dword;
//
//    AccessItem(uint8_t i) : byte(i) {}
//    AccessItem(uint16_t i) : hword(i) {}
//    AccessItem(uint32_t i) : word(i) {}
//    AccessItem(uint64_t i) : dword(i) {}
//};

typedef uint32_t AccessItem; // TODO: Temporary

template<AccessSize SIZE>
struct Access {
    AccessItem item;
};

#endif//QTMIPS_MACHINE_ACCESS_SIZE_H
