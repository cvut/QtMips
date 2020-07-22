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

#ifndef MEMORY_H
#define MEMORY_H

#include <QObject>
#include <cstdint>
#include "../qtmipsexception.h"
#include "../machinedefs.h"
#include "address.h"

namespace machine {

// Virtual class for common memory access
class FrontendMemory : public QObject {
    Q_OBJECT
public:
    // Note: hword and word methods are throwing away lowest bits so unaligned access is ignored without error.
    bool write_byte(Address address, std::uint8_t value);
    bool write_hword(Address address, std::uint16_t value);
    bool write_word(Address address, std::uint32_t value);
    bool write_dword(Address address, std::uint64_t value);

    std::uint8_t read_byte(Address address, bool debug_access = false) const;
    std::uint16_t read_hword(Address address, bool debug_access = false) const;
    std::uint32_t read_word(Address address, bool debug_access = false) const;
    std::uint64_t read_dword(Address address, bool debug_access = false) const;

    void write_ctl(enum AccessControl ctl, Address offset, std::uint32_t value);
    std::uint32_t read_ctl(enum AccessControl ctl, Address offset) const;

    virtual void sync();
    virtual enum LocationStatus location_status(Address address) const;
    virtual std::uint32_t get_change_counter() const = 0;

signals:
    void external_change_notify(
        const FrontendMemory *mem_access,
        Address start_addr,
        Address last_addr,
        bool external
    ) const;

protected:
    virtual bool write(Address offset, std::uint32_t value) = 0;
    virtual std::uint32_t read(Address address, bool debug_access = false) const = 0;

private:
    static int sh_nth(std::uint32_t offset);
};



}

Q_DECLARE_METATYPE(machine::AccessControl)
Q_DECLARE_METATYPE(machine::LocationStatus)

#endif // MEMORY_H
