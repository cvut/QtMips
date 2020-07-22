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

#include "frontend_memory.h"


using namespace machine;

#if __BYTE_ORDER == __LITTLE_ENDIAN
    #define SH_NTH_8(OFFSET) ((3 - ((OFFSET) & 0b11)) * 8)
    #define SH_NTH_16(OFFSET) ((2 - ((OFFSET) & 0b10)) * 8)
#else
    #define SH_NTH_8(OFFSET) (((OFFSET) & 0b11) * 8)
    #define SH_NTH_16(OFFSET) (((OFFSET) & 0b10) * 8)
#endif

bool FrontendMemory::write_byte(
    Address address,
    uint8_t value
) {
    int           nth  = SH_NTH_8(address.get_raw());
    uint32_t mask = 0xff << nth; // Mask for n-th byte
    return write(address, (read(address) & ~mask) | (((std::uint32_t) value << nth) & mask));
}

bool FrontendMemory::write_hword(
    Address address,
    std::uint16_t value
) {
    int           nth  = SH_NTH_16(address.get_raw());
    std::uint32_t mask = 0xffff << nth; // Mask for n-th half-word
    return write(address, (read(address) & ~mask) | (((std::uint32_t) value << nth) & mask));
}

bool FrontendMemory::write_word(
    Address address,
    std::uint32_t value
) {
    return write(address, value);
}

bool FrontendMemory::write_dword(
    Address address,
    uint64_t value
) {
    return write(address, (uint32_t) (value >> 32U)) && write(address + 32, (uint32_t) value); // TODO: Optimize,
    // endiannes?
}

std::uint8_t FrontendMemory::read_byte(
    Address address,
    bool debug_access
) const {
    int nth = SH_NTH_8(address.get_raw());
    return (std::uint8_t) (read(address, debug_access) >> nth);
}

std::uint16_t FrontendMemory::read_hword(
    Address address,
    bool debug_access
) const {
    int nth = SH_NTH_16(address.get_raw());
    return (std::uint16_t) (read(address, debug_access) >> nth);
}

std::uint32_t FrontendMemory::read_word(
    Address address,
    bool debug_access
) const {
    return read(address, debug_access);
}

void FrontendMemory::write_ctl(
    enum AccessControl ctl,
    Address offset,
    std::uint32_t value
) {
    switch (ctl) {
        case AC_NONE:
            break;
        case AC_BYTE:
        case AC_BYTE_UNSIGNED:
            this->write_byte(offset, (std::uint8_t) value);
            break;
        case AC_HALFWORD:
        case AC_HALFWORD_UNSIGNED:
            this->write_hword(offset, (std::uint16_t) value);
            break;
        case AC_WORD:
            this->write_word(offset, value);
            break;
        default:
            throw QTMIPS_EXCEPTION(UnknownMemoryControl, "Trying to write to memory with unknown ctl", QString::number(
                ctl
            ));
    }
}

std::uint32_t FrontendMemory::read_ctl(enum AccessControl ctl, Address offset) const {
    switch (ctl) {
    case AC_NONE:
        return 0;
    case AC_BYTE:
        {
        std::uint8_t b = this->read_byte(offset);
        return  ((std::uint32_t)b & 0xFF) - (((std::uint32_t)b & 0x80) << 1); // Sign extend
        }
    case AC_HALFWORD:
        {
        std::uint16_t h = this->read_hword(offset);
        return ((std::uint32_t)h & 0xFFFF) - (((std::uint32_t)h & 0x8000) << 1); // Sign extend
        }
    case AC_WORD:
        return this->read_word(offset);
    case AC_BYTE_UNSIGNED:
        return this->read_byte(offset);
    case AC_HALFWORD_UNSIGNED:
        return this->read_hword(offset);
    default:
        throw QTMIPS_EXCEPTION(UnknownMemoryControl, "Trying to read from memory with unknown ctl", QString::number(ctl));
    }
}

void FrontendMemory::sync() {}

enum LocationStatus FrontendMemory::location_status(Address address) const {
    (void) address;
    return LOCSTAT_NONE;
}
