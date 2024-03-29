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

#ifndef BACKEND_MEMORY_H
#define BACKEND_MEMORY_H

#include "common/endian.h"
#include "machinedefs.h"
#include "memory/memory_utils.h"

#include <QObject>

// Shortcut for enum class values, type is obvious from context.
using ae = machine::AccessEffects;

namespace machine {

/**
 * Relative index within an instance of backend memory.
 */
typedef size_t Offset;

/**
 * Interface for physical memory or periphery.
 * .
 * Device implementing this interface is connected to the memory system via
 * memory data bus (`memory/memory_bus.h`).
 *
 * ## ENDIAN
 * Each device is responsible to return reads and write with the correct endian.
 * This is because there are different optimal ways to perform the swapping in
 * different kind of peripheries. For example, peripheries that have only word
 * (u23) accessible registers are simple to swap.
 *
 * All backend memory devices must set the `simulated_machine_endian` in
 * `BackendMemory` parent. They should also, by convention, have a private
 * (`const` or `static constexpr`) variable `internal_endian`. It can have
 * values `LITTLE | BIG | NATIVE_ENDIAN` (note: `NATIVE_ENDIAN` is a compile
 * time constant with endian of the host machine i.e. `LITTLE | BIG`). Byteswap
 * is needed, when internal and simulated endian are mismatched.
 *
 * ### Examples of internal endian values
 * - LED diode will have `NATIVE_ENDIAN` as the rgb value needs to be valid for
 *   GUI.
 * - Memory mapped source will have runtime set endian, based on the file
 *   endian.
 * - LCD has fixed `BIG` endian, as required by the hardware.
 */
class BackendMemory : public QObject {
    Q_OBJECT

public:
    /**
     * @param simulated_machine_endian      endian of the simulated CPU/memory
     *                                      system
     */
    explicit BackendMemory(Endian simulated_machine_endian);

    /**
     * Write byte sequence to memory.
     *
     * @param source        pointer to array of bytes to be copied
     * @param destination   relative index of destination to write to
     * @param size         	number of bytes to be written
     * @return              true when memory before and after write differs
     */
    virtual WriteResult write(
        Offset destination,
        const void *source,
        size_t size,
        WriteOptions options)
        = 0;

    /**
     * Read sequence of bytes from memory
     *
     * @param source        relative index of data to be read
     * @param destination   pointer to destination buffer
     * @param size          number of bytes to be read
     * @param options       additional option like debug mode, see type
     *                      definition
     */
    virtual ReadResult read(
        void *destination,
        Offset source,
        size_t size,
        ReadOptions options) const = 0;

    /**
     * Determine status of given address.
     *
     * Applicable values:
     *  - LOCSTAT_NONE          normal RW area
     *  - LOCSTAT_READ_ONLY     read only hw register
     *  - LOCSTAT_ILLEGAL       address is not occupied, write will result in
     *                          NOP, read will return constant zero.
     */
    virtual enum LocationStatus location_status(Offset offset) const = 0;

    /**
     * Endian of the simulated CPU/memory system.
     * @see BackendMemory docs
     */
    const Endian simulated_machine_endian;

signals:
    /**
     * Notify upper layer about a change in managed physical memory of periphery
     *
     * @param mem_access    this
     * @param start_addr    affected area start
     * @param last_addr     affected area end
     * @param type          allowed side effects, see type declaration
     */
    void external_backend_change_notify(
        const BackendMemory *mem_access,
        uint32_t start_addr,
        uint32_t last_addr,
        AccessEffects type) const;
};

inline BackendMemory::BackendMemory(Endian simulated_machine_endian)
    : simulated_machine_endian(simulated_machine_endian) {}

} // namespace machine

#endif // BACKEND_MEMORY_H
