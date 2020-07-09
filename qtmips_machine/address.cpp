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
 * Copyright (c) 2020      Jakub Dupak <dupakjak@gmail.com>
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

#include "address.h"


using namespace machine;

Address::Address(uint_xlen_t address)
    : data(address) {}

uint_xlen_t Address::get_data() const {
    return data;
}

/*
 * Equality operators
 */

bool Address::operator==(const Address &other) const {
    return this->get_data() == other.get_data();
}

bool Address::operator!=(const Address &other) const {
    return this->get_data() != other.get_data();
}

/*
 * Ordering operators
 */

bool Address::operator<(const Address &other) const {
    return this->get_data() < other.get_data();
}

bool Address::operator>(const Address &other) const {
    return this->get_data() > other.get_data();
}

bool Address::operator<=(const Address &other) const {
    return this->get_data() <= other.get_data();
}

bool Address::operator>=(const Address &other) const {
    return this->get_data() >= other.get_data();
}

/*
 * Offset arithmetic operators
 */

Address Address::operator+(const uint_xlen_t &offset) const {
    return Address(this->get_data() + offset);
}

Address Address::operator-(const uint_xlen_t &offset) const {
    return Address(this->get_data() - offset);
}

Address Address::operator*(const uint_xlen_t &offset) const {
    return Address(this->get_data() * offset);
}

Address Address::operator/(const uint_xlen_t &offset) const {
    return Address(this->get_data() / offset);
}

/*
 * Bitwise operators
 */

Address Address::operator&(const uint_xlen_t &mask) const {
    return Address(this->get_data() & mask);
}

Address Address::operator|(const uint_xlen_t &mask) const {
    return Address(this->get_data() | mask);
}

Address Address::operator^(const uint_xlen_t &mask) const {
    return Address(get_data() ^ mask);
}

Address Address::operator>>(const uint_xlen_t &offset) const {
    return Address(this->get_data() >> offset);
}

Address Address::operator<<(const uint_xlen_t &mask) const {
    return Address(this->get_data() << mask);
}

/*
 * Distance arithmetic operators
 */

uint_xlen_t Address::operator-(const Address &other) const {
    return this->get_data() - other.get_data();
}


