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

#ifndef PROGRAMDOCK_H
#define PROGRAMDOCK_H

#include "machine/machine.h"
#include "peripheralsview.h"

#include <QComboBox>
#include <QDockWidget>
#include <QLabel>

class ProgramDock : public QDockWidget {
    Q_OBJECT

    using Super = QDockWidget;

public:
    ProgramDock(QWidget *parent, QSettings *settings);

    void setup(machine::Machine *machine);

signals:
    void machine_setup(machine::Machine *machine);
    void jump_to_pc(machine::Address);
    void focus_addr(machine::Address);
    void focus_addr_with_save(machine::Address);
    void stage_addr_changed(uint stage, machine::Address addr);
    void request_update_all();
public slots:
    void set_follow_inst(int);
    void fetch_inst_addr(machine::Address addr);
    void decode_inst_addr(machine::Address addr);
    void execute_inst_addr(machine::Address addr);
    void memory_inst_addr(machine::Address addr);
    void writeback_inst_addr(machine::Address addr);
    void report_error(const QString &error);

private:
    enum FollowSource {
        FOLLOWSRC_NONE,
        FOLLOWSRC_FETCH,
        FOLLOWSRC_DECODE,
        FOLLOWSRC_EXECUTE,
        FOLLOWSRC_MEMORY,
        FOLLOWSRC_WRITEBACK,
        FOLLOWSRC_COUNT,
    };

    void update_follow_position();
    enum FollowSource follow_source;
    machine::Address follow_addr[FOLLOWSRC_COUNT] {};
    QSettings *settings;
};

#endif // PROGRAMDOCK_H
