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

#include "aboutdialog.h"

#include "project_info.h"

#include <QApplication>
#include <QDebug>
#include <QLabel>
#include <QObject>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QTextBrowser>
#include <QVBoxLayout>

AboutDialog::AboutDialog(QWidget *parent) : QDialog(parent) {
    QLabel *lbl;

    setAttribute(Qt::WA_DeleteOnClose);
    setAttribute(Qt::WA_ShowModal);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    setWindowTitle(tr("About " APP_NAME));

    all = new QVBoxLayout(this);

    QWidget *hbox = new QWidget();
    QHBoxLayout *hl = new QHBoxLayout(hbox);
    hl->setContentsMargins(0, 0, 0, 0);

    all->addWidget(hbox);

    QWidget *vbox = new QWidget();
    QVBoxLayout *vl = new QVBoxLayout(vbox);
    // vl->setContentsMargins(0,0,0,0);
    hl->addWidget(vbox);

    QString versionText;
    versionText = "Version " APP_VERSION "\n";

    vl->addWidget(new QLabel("<span style='font-size:x-large; font-weight:bold;"
                             "'>" APP_NAME " "
                             "- MIPS Architecture Simulator</span>"));
    lbl = new QLabel(versionText);
    lbl->setAlignment(Qt::AlignHCenter);
    lbl->setOpenExternalLinks(true);
    vl->addWidget(lbl);
    vl->addWidget(new QLabel(COPYRIGHT_HTML));

    QString supportText;
    supportText = "Home Page : <a "
                  "href=\"" APP_GIT "\">" APP_GIT "</a><br/>"
                  "Implemented for <a "
                  "href=\"https://cw.fel.cvut.cz/wiki/courses/b35apo/"
                  "start\">Computer Architectures</a> and <a "
                  "href=\"https://cw.fel.cvut.cz/wiki/courses/b4m35pap/"
                  "start\">Advanced Computer Achitectures</a> courses "
                  "at <a href=\"https://www.cvut.cz/\">Czech Technical "
                  "University in Prague</a>"
                  " <a href=\"https://www.fel.cvut.cz/\">Faculty of Electrical "
                  "Engineering</a><br/>";

    QTextBrowser *supportBrowser = new QTextBrowser;
    supportBrowser->setOpenExternalLinks(true);
    supportBrowser->setHtml(supportText);
    vl->addWidget(supportBrowser);

    QTextBrowser *licenseBrowser = new QTextBrowser;
    licenseBrowser->setOpenExternalLinks(true);
    licenseBrowser->setHtml(LICENCE_HTML);
    vl->addWidget(licenseBrowser);

    QWidget *hbBtn = new QWidget();
    QHBoxLayout *hlBtn = new QHBoxLayout(hbBtn);
    hlBtn->setContentsMargins(0, 0, 0, 0);
    vl->addWidget(hbBtn);

    QPushButton *okButton = new QPushButton(tr("&OK"), parent);
    okButton->setFocus();
    connect(okButton, &QAbstractButton::clicked, this, &QWidget::close);
    hlBtn->addStretch();
    hlBtn->addWidget(okButton);

    setMinimumSize(450, 500);

    prevTab = 0; // first Tab is selected by default
}
