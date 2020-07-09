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
 * Copyright (c) 2020      Jakub Dupak <dupakjak@fel.cvut.cz>
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

#ifndef QTMIPS_ADDRESS_H
#define QTMIPS_ADDRESS_H

#include "numeric.h"


namespace machine {

/**
 * Emulated memory address
 *
 * In-place operators are deliberately not implemented as this type is considered
 * primitive and immutable.
 */
class Address {
private:
    const uint_xlen_t data; //> Real wrapped address

public:
    explicit Address(uint_xlen_t);

    Address(const Address &address) = default; //> Copy constructor

    uint_xlen_t get_data() const;

    /* Eq */
    bool operator==(const Address &other) const;
    bool operator!=(const Address &other) const;

    /* Ord */
    bool operator<(const Address &other) const;
    bool operator>(const Address &other) const;
    bool operator<=(const Address &other) const;
    bool operator>=(const Address &other) const;

    /* Offset arithmetic */
    Address operator+(const uint_xlen_t &offset) const;
    Address operator-(const uint_xlen_t &offset) const;
    Address operator*(const uint_xlen_t &offset) const;
    Address operator/(const uint_xlen_t &offset) const;

    /* Bitwise arithmetic */
    Address operator&(const uint_xlen_t &mask) const;
    Address operator|(const uint_xlen_t &mask) const;
    Address operator^(const uint_xlen_t &mask) const;
    Address operator>>(const uint_xlen_t &offset) const;
    Address operator<<(const uint_xlen_t &offset) const;

    /* Distance arithmetic */
    uint_xlen_t operator-(const Address &other) const;
};
}


#endif //QTMIPS_ADDRESS_H
