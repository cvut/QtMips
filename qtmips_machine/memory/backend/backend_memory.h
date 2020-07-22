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

#ifndef QTMIPS_MACHINE_BACKEND_MEMORY_H
#define QTMIPS_MACHINE_BACKEND_MEMORY_H

#include <QObject>
#include "../access_size.h"


namespace machine {

typedef uint64_t Offset;


/**
 * Interface for physical memory or periphery
 *
 * Properties:
 *  - Values returned have simulated endianness
 *  - Stored endianness is undefined
 */
class BackendMemory : public QObject {
Q_OBJECT

public:
    virtual bool write(
        Offset offset,
        AccessSize size,
        AccessItem value
    ) = 0;

    virtual AccessItem read(
        Offset offset,
        AccessSize size,
        bool debug_read = false
    ) const = 0;


signals:
    /**
     * Notify MMU about a change in managed physical memory of periphery
     *
     * @param mem_access    this
     * @param start_addr    affected area start
     * @param last_addr     affected area end
     * @param external
     */
    void external_backend_change_notify(
        const BackendMemory *mem_access,
        std::uint32_t start_addr,
        std::uint32_t last_addr,
        bool external
    ) const;
};
};

#endif //QTMIPS_MACHINE_BACKEND_MEMORY_H
