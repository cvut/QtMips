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

#include "programtableview.h"

#include "hinttabledelegate.h"
#include "programmodel.h"

#include <QtGlobal>
#include <QApplication>
#include <QClipboard>
#include <QFontMetrics>
#include <QHeaderView>
#include <QKeyEvent>
#include <QScrollBar>

ProgramTableView::ProgramTableView(QWidget *parent, QSettings *settings)
    : Super(parent) {
    setItemDelegate(new HintTableDelegate);
    connect(
        verticalScrollBar(), &QAbstractSlider::valueChanged, this,
        &ProgramTableView::adjust_scroll_pos_check);
    connect(
        this, &ProgramTableView::adjust_scroll_pos_queue, this,
        &ProgramTableView::adjust_scroll_pos_process, Qt::QueuedConnection);
    this->settings = settings;
    initial_address = machine::Address(
        settings->value("ProgramViewAddr0", 0).toULongLong());
    adjust_scroll_pos_in_progress = false;
    need_addr0_save = false;
    setTextElideMode(Qt::ElideNone);
}

void ProgramTableView::addr0_save_change(machine::Address val) {
    need_addr0_save = false;
    settings->setValue("ProgramViewAddr0", qint64(val.get_raw()));
}

void ProgramTableView::adjustColumnCount() {
    QModelIndex idx;
    int cwidth_dh;
    int totwidth;

    ProgramModel *m = dynamic_cast<ProgramModel *>(model());

    if (m == nullptr) {
        return;
    }

    HintTableDelegate *delegate
        = dynamic_cast<HintTableDelegate *>(itemDelegate());
    if (delegate == nullptr) {
        return;
    }

    QStyleOptionViewItem viewOpts;
    #if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        initViewItemOption(&viewOpts);
    #else
        viewOpts = viewOptions();
    #endif

    idx = m->index(0, 0);
    cwidth_dh = delegate->sizeHintForText(viewOpts, idx, "Bp").width() + 2;
    horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
    horizontalHeader()->resizeSection(0, cwidth_dh);
    totwidth = cwidth_dh;

    idx = m->index(0, 1);
    cwidth_dh
        = delegate->sizeHintForText(viewOpts, idx, "0x00000000").width()
          + 2;
    horizontalHeader()->setSectionResizeMode(1, QHeaderView::Fixed);
    horizontalHeader()->resizeSection(1, cwidth_dh);
    totwidth += cwidth_dh;

    idx = m->index(0, 2);
    cwidth_dh
        = delegate->sizeHintForText(viewOpts, idx, "00000000").width() + 2;
    horizontalHeader()->setSectionResizeMode(2, QHeaderView::Fixed);
    horizontalHeader()->resizeSection(2, cwidth_dh);
    totwidth += cwidth_dh;

    horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
    idx = m->index(0, 3);
    totwidth
        += delegate
               ->sizeHintForText(viewOpts, idx, "BEQ $18, $17, 0x80020058")
               .width()
           + 2;
    totwidth += verticalHeader()->width();
    setColumnHidden(2, totwidth > width());

    if (!initial_address.is_null()) {
        go_to_address(initial_address);
        initial_address = machine::Address::null();
    }
}

void ProgramTableView::adjust_scroll_pos_check() {
    if (!adjust_scroll_pos_in_progress) {
        adjust_scroll_pos_in_progress = true;
        emit adjust_scroll_pos_queue();
    }
}

void ProgramTableView::adjust_scroll_pos_process() {
    adjust_scroll_pos_in_progress = false;
    machine::Address address;
    ProgramModel *m = dynamic_cast<ProgramModel *>(model());
    if (m == nullptr) {
        return;
    }

    QModelIndex prev_index = currentIndex();
    machine::Address row_bytes = machine::Address(m->cellSizeBytes());
    machine::Address index0_offset = m->getIndex0Offset();

    do {
        int row = rowAt(0);
        int prev_row = row;
        if (row < m->rowCount() / 8) {
            if ((row == 0) && (index0_offset < row_bytes)
                && (!index0_offset.is_null())) {
                m->adjustRowAndOffset(row, machine::Address::null());
            } else if (index0_offset >= row_bytes) {
                m->get_row_address(address, row);
                m->adjustRowAndOffset(row, address);
            } else {
                break;
            }
        } else if (row > m->rowCount() - m->rowCount() / 8) {
            m->get_row_address(address, row);
            m->adjustRowAndOffset(row, address);
        } else {
            break;
        }
        scrollTo(m->index(row, 0), QAbstractItemView::PositionAtTop);
        setCurrentIndex(
            m->index(prev_index.row() + row - prev_row, prev_index.column()));
        emit m->update_all();
    } while (false);
    m->get_row_address(address, rowAt(0));
    if (need_addr0_save) {
        addr0_save_change(address);
    }
    emit address_changed(address.get_raw());
}

void ProgramTableView::resizeEvent(QResizeEvent *event) {
    ProgramModel *m = dynamic_cast<ProgramModel *>(model());
    machine::Address address;
    bool keep_row0 = false;

    if (m != nullptr) {
        if (initial_address.is_null()) {
            keep_row0 = m->get_row_address(address, rowAt(0));
        } else {
            address = initial_address;
        }
    }
    Super::resizeEvent(event);
    adjustColumnCount();
    if (keep_row0) {
        initial_address = machine::Address::null();
        go_to_address(address);
    }
}

void ProgramTableView::go_to_address_priv(machine::Address address) {
    ProgramModel *m = dynamic_cast<ProgramModel *>(model());
    int row;
    if (m == nullptr) {
        return;
    }
    m->adjustRowAndOffset(row, address);
    scrollTo(m->index(row, 0), QAbstractItemView::PositionAtTop);
    setCurrentIndex(m->index(row, 1));
    if (need_addr0_save) {
        addr0_save_change(address);
    }
    emit m->update_all();
}

void ProgramTableView::go_to_address(machine::Address address) {
    need_addr0_save = true;
    go_to_address_priv(address);
}

void ProgramTableView::focus_address(machine::Address address) {
    int row;
    ProgramModel *m = dynamic_cast<ProgramModel *>(model());
    if (m == nullptr) {
        return;
    }
    if (!m->get_row_for_address(row, address)) {
        go_to_address_priv(address);
    }
    if (!m->get_row_for_address(row, address)) {
        return;
    }
    setCurrentIndex(m->index(row, 1));
}

void ProgramTableView::focus_address_with_save(machine::Address address) {
    need_addr0_save = true;
    focus_address(address);
}

void ProgramTableView::keyPressEvent(QKeyEvent *event) {
    if (event->matches(QKeySequence::Copy)) {
        QString text;
        QItemSelectionRange range = selectionModel()->selection().first();
        for (auto i = range.top(); i <= range.bottom(); ++i) {
            QStringList rowContents;
            for (auto j = range.left(); j <= range.right(); ++j) {
                rowContents << model()->index(i, j).data().toString();
            }
            text += rowContents.join("\t");
            text += "\n";
        }
        QApplication::clipboard()->setText(text);
    } else {
        Super::keyPressEvent(event);
    }
}
