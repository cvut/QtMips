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
#include <cstdint>

using std::uint64_t;

namespace machine {

/**
 * Emulated memory address
 *
 * OPTIMIZATION NOTE: All methods are implemented in header file to support inlining (as we want to
 *  use it as a primitive type) but too keep declaration tidy out-of-line definitions
 *  are proffered.
 */
class Address {
private:
    uint64_t data;//> Real wrapped address

public:
    constexpr explicit Address(uint64_t);

    constexpr Address();

    constexpr Address(const Address &address) = default;//> Copy constructor

    constexpr uint64_t get_raw() const;

    /**
     * Convert to address indexing by bytes (as minimal access unit)
     *  instead of bit indexing.
     *
     * @return index of addressed byte
     */
    constexpr uint64_t get_byte_index() const;

    /**
     * More expressive way to assign null
     */
    constexpr static Address null();

    /**
     * Test for null address
     *
     * Testing for null in very often and without this method a creation of
     *  a new address object set to null would be required. That is considered
     *  too long and badly readable. For other comparison we expect a comparison
     *  with existing value to be the common case.
     */
    constexpr bool is_null() const;

    /* Eq */
    constexpr inline bool operator==(const Address &other) const;
    constexpr inline bool operator!=(const Address &other) const;

    /* Ord */
    constexpr inline bool operator<(const Address &other) const;
    constexpr inline bool operator>(const Address &other) const;
    constexpr inline bool operator<=(const Address &other) const;
    constexpr inline bool operator>=(const Address &other) const;

    /* Offset arithmetic */
    constexpr inline Address operator+(const uint64_t &offset) const;
    constexpr inline Address operator-(const uint64_t &offset) const;
    inline void operator+=(const uint64_t &offset);
    inline void operator-=(const uint64_t &offset);

    /* Bitwise arithmetic */
    constexpr inline Address operator&(const uint64_t &mask) const;
    constexpr inline Address operator|(const uint64_t &mask) const;
    constexpr inline Address operator^(const uint64_t &mask) const;
    constexpr inline Address operator>>(const uint64_t &size) const;
    constexpr inline Address operator<<(const uint64_t &size) const;

    /* Distance arithmetic */
    constexpr inline uint64_t operator-(const Address &other) const;
};

constexpr Address operator"" _addr(unsigned long long literal) {
    return Address(literal);
}

constexpr Address::Address(uint64_t address)
    : data(address) {}

constexpr Address::Address()
    : data(0) {}

constexpr uint64_t Address::get_raw() const {
    return data;
}

constexpr uint64_t Address::get_byte_index() const {
    return this->get_raw() >> 2U;
}


constexpr Address Address::null() {
    return Address(0x0);
}

constexpr bool Address::is_null() const {
    return this->get_raw() == 0;
}

/*
 * Equality operators
 */

constexpr bool Address::operator==(const Address &other) const {
    return this->get_raw() == other.get_raw();
}

constexpr bool Address::operator!=(const Address &other) const {
    return this->get_raw() != other.get_raw();
}

/*
 * Ordering operators
 */

constexpr bool Address::operator<(const Address &other) const {
    return this->get_raw() < other.get_raw();
}

constexpr bool Address::operator>(const Address &other) const {
    return this->get_raw() > other.get_raw();
}

constexpr bool Address::operator<=(const Address &other) const {
    return this->get_raw() <= other.get_raw();
}

constexpr bool Address::operator>=(const Address &other) const {
    return this->get_raw() >= other.get_raw();
}

/*
 * Offset arithmetic operators
 */

constexpr Address Address::operator+(const uint64_t &offset) const {
    return Address(this->get_raw() + offset);
}

constexpr Address Address::operator-(const uint64_t &offset) const {
    return Address(this->get_raw() - offset);
}

void Address::operator+=(const uint64_t &offset) {
    data += offset;
}

void Address::operator-=(const uint64_t &offset) {
    data -= offset;
}


/*
 * Bitwise operators
 */

constexpr Address Address::operator&(const uint64_t &mask) const {
    return Address(this->get_raw() & mask);
}

constexpr Address Address::operator|(const uint64_t &mask) const {
    return Address(this->get_raw() | mask);
}

constexpr Address Address::operator^(const uint64_t &mask) const {
    return Address(get_raw() ^ mask);
}

constexpr Address Address::operator>>(const uint64_t &size) const {
    return Address(this->get_raw() >> size);
}

constexpr Address Address::operator<<(const uint64_t &size) const {
    return Address(this->get_raw() << size);
}

/*
 * Distance arithmetic operators
 */

constexpr uint64_t Address::operator-(const Address &other) const {
    return this->get_raw() - other.get_raw();
}

}// namespace machine


#endif//QTMIPS_ADDRESS_H
