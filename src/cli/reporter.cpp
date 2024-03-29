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

#include "reporter.h"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <utility>

using namespace machine;
using namespace std;

Reporter::Reporter(QCoreApplication *app, Machine *machine) : QObject() {
    this->app = app;
    this->machine = machine;

    connect(machine, &Machine::program_exit, this, &Reporter::machine_exit);
    connect(machine, &Machine::program_trap, this, &Reporter::machine_trap);
    connect(
        machine->core(), &Core::stop_on_exception_reached, this,
        &Reporter::machine_exception_reached);

    e_regs = false;
    e_cache_stats = false;
    e_cycles = false;
    e_fail = (enum FailReason)0;
}

void Reporter::regs() {
    e_regs = true;
}

void Reporter::cache_stats() {
    e_cache_stats = true;
}

void Reporter::cycles() {
    e_cycles = true;
}

void Reporter::expect_fail(enum FailReason reason) {
    e_fail = (enum FailReason)(e_fail | reason);
}

void Reporter::add_dump_range(
    Address start,
    size_t len,
    const QString &path_to_write) {
    dump_ranges.append({ start, len, path_to_write });
}

void Reporter::machine_exit() {
    report();
    if (e_fail != 0) {
        cout << "Machine was expected to fail but it didn't." << endl;
        QCoreApplication::exit(1);
    } else {
        QCoreApplication::exit();
    }
}

void Reporter::machine_exception_reached() {
    ExceptionCause excause;
    excause = machine->get_exception_cause();
    switch (excause) {
    case EXCAUSE_NONE:
        cout << "Machine stopped on NONE exception." << endl;
        break;
    case EXCAUSE_INT:
        cout << "Machine stopped on INT exception." << endl;
        break;
    case EXCAUSE_ADDRL:
        cout << "Machine stopped on ADDRL exception." << endl;
        break;
    case EXCAUSE_ADDRS:
        cout << "Machine stopped on ADDRS exception." << endl;
        break;
    case EXCAUSE_IBUS:
        cout << "Machine stopped on IBUS exception." << endl;
        break;
    case EXCAUSE_DBUS:
        cout << "Machine stopped on DBUS exception." << endl;
        break;
    case EXCAUSE_SYSCALL:
        cout << "Machine stopped on SYSCALL exception." << endl;
        break;
    case EXCAUSE_OVERFLOW:
        cout << "Machine stopped on OVERFLOW exception." << endl;
        break;
    case EXCAUSE_TRAP:
        cout << "Machine stopped on TRAP exception." << endl;
        break;
    case EXCAUSE_HWBREAK:
        cout << "Machine stopped on HWBREAK exception." << endl;
        break;
    default: break;
    }
    report();
    QCoreApplication::exit();
}

void Reporter::machine_trap(SimulatorException &e) {
    report();

    bool expected = false;
    auto &etype = typeid(e);
    if (etype == typeid(SimulatorExceptionUnsupportedInstruction)) {
        expected = e_fail & FR_I;
    } else if (etype == typeid(SimulatorExceptionUnsupportedAluOperation)) {
        expected = e_fail & FR_A;
    } else if (etype == typeid(SimulatorExceptionOverflow)) {
        expected = e_fail & FR_O;
    } else if (etype == typeid(SimulatorExceptionUnalignedJump)) {
        expected = e_fail & FR_J;
    }

    cout << "Machine trapped: " << e.msg(false).toStdString() << endl;
    QCoreApplication::exit(expected ? 0 : 1);
}

static void out_hex(ostream &out, uint64_t val, int digits) {
    std::ios_base::fmtflags saveflg(out.flags());
    char prevfill = out.fill('0');
    out.setf(ios::hex, ios::basefield);
    out << setfill('0') << setw(digits) << val;
    out.fill(prevfill);
    out.flags(saveflg);
}

void Reporter::report() {
    cout << dec;
    if (e_regs) {
        cout << "Machine state report:" << endl;
        cout << "PC:0x";
        out_hex(cout, machine->registers()->read_pc().get_raw(), 8);
        cout << endl;
        for (int i = 0; i < 32; i++) {
            cout << "R" << i << ":0x";
            out_hex(cout, machine->registers()->read_gp(i).as_u64(), 8);
            if (i != 31) {
                cout << " ";
            } else {
                cout << endl;
            }
        }
        cout << "HI:0x";
        out_hex(cout, machine->registers()->read_hi_lo(true).as_u64(), 8);
        cout << " LO:0x";
        out_hex(cout, machine->registers()->read_hi_lo(false).as_u64(), 8);
        cout << endl;
        for (int i = 1; i < Cop0State::COP0REGS_CNT; i++) {
            cout << Cop0State::cop0reg_name((Cop0State::Cop0Registers)i)
                        .toLocal8Bit()
                        .data()
                 << ":0x";
            out_hex(
                cout,
                machine->cop0state()->read_cop0reg((Cop0State::Cop0Registers)i),
                8);
            if (i != Cop0State::COP0REGS_CNT - 1) {
                cout << " ";
            } else {
                cout << endl;
            }
        }
    }
    if (e_cache_stats) {
        cout << "Cache statistics report:" << endl;
        cout << "i-cache:reads:" << machine->cache_program()->get_read_count()
             << endl;
        cout << "i-cache:hit:" << machine->cache_program()->get_hit_count()
             << endl;
        cout << "i-cache:miss:" << machine->cache_program()->get_miss_count()
             << endl;
        cout << "i-cache:hit-rate:" << machine->cache_program()->get_hit_rate()
             << endl;
        cout << "i-cache:stalled-cycles:"
             << machine->cache_program()->get_stall_count() << endl;
        cout << "i-cache:improved-speed:"
             << machine->cache_program()->get_speed_improvement() << endl;
        cout << "d-cache:reads:" << machine->cache_data()->get_read_count()
             << endl;
        cout << "d-cache:writes:" << machine->cache_data()->get_write_count()
             << endl;
        cout << "d-cache:hit:" << machine->cache_data()->get_hit_count()
             << endl;
        cout << "d-cache:miss:" << machine->cache_data()->get_miss_count()
             << endl;
        cout << "d-cache:hit-rate:" << machine->cache_data()->get_hit_rate()
             << endl;
        cout << "d-cache:stalled-cycles:"
             << machine->cache_data()->get_stall_count() << endl;
        cout << "d-cache:improved-speed:"
             << machine->cache_data()->get_speed_improvement() << endl;
    }
    if (e_cycles) {
        cout << "d-cache:stalled-cycles:"
             << machine->cache_data()->get_stall_count() << endl;
        cout << "d-cache:improved-speed:"
             << machine->cache_data()->get_speed_improvement() << endl;
    }
    if (e_cycles) {
        cout << "cycles:" << machine->core()->get_cycle_count() << endl;
        cout << "stalls:" << machine->core()->get_stall_count() << endl;
    }
    foreach (DumpRange range, dump_ranges) {
        ofstream out;
        out.open(
            range.path_to_write.toLocal8Bit().data(), ios::out | ios::trunc);
        Address start = range.start & ~3;
        Address end = range.start + range.len;
        if (end < start) {
            end = 0xffffffff_addr;
        }
        const MemoryDataBus *mem = machine->memory_data_bus();
        for (Address addr = start; addr < end; addr += 4) {
            out << "0x";
            out_hex(
                out, mem->read_u32(addr, ae::INTERNAL), 8);
            out << endl;
        }
        out.close();
    }
}
