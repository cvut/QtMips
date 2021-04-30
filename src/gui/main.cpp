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

#include "common/logging.h"
#include "common/logging_format_colors.h"
#include "mainwindow.h"

#include <QApplication>

LOG_CATEGORY("gui.main");

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    set_default_log_pattern();

    // There constants are set in CMake.
    QApplication::setApplicationName(APP_NAME);
    QApplication::setOrganizationName(APP_ORGANIZATION);
    QApplication::setOrganizationDomain(APP_ORGANIZATION_DOMAIN);
    QApplication::setApplicationVersion(APP_VERSION);

    LOG("Started %s version %s.", APP_NAME, APP_VERSION);
    LOG("Developed at %s (%s).\n\n", APP_ORGANIZATION, APP_ORGANIZATION_DOMAIN);

    /*
     * If environment variable specified in define ENV_CONFIG_FILE_NAME is
     * present, the app should behave in portable manner and use ini file on the
     * specified location. Otherwise Qt defaults are used.
     */
    auto env = QProcessEnvironment::systemEnvironment();
    QSettings *settings;
    if (env.contains(ENV_CONFIG_FILE_NAME)) {
        // Behave as a portable app.
        settings = new QSettings(env.value(ENV_CONFIG_FILE_NAME), QSettings::IniFormat);
    } else {
        // Qt defaults
        settings = new QSettings(APP_ORGANIZATION, APP_NAME);
    }

    MainWindow w(settings); // Moving settings ownership.
    w.start();

    return QApplication::exec();
}
