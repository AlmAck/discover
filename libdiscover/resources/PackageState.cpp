/***************************************************************************
 *   Copyright © 2012 Aleix Pol Gonzalez <aleixpol@blue-systems.com>       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU General Public License as        *
 *   published by the Free Software Foundation; either version 2 of        *
 *   the License or (at your option) version 3 or any later version        *
 *   accepted by the membership of KDE e.V. (or its successor approved     *
 *   by the membership of KDE e.V.), which shall act as a proxy            *
 *   defined in Section 14 of version 3 of the license.                    *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 ***************************************************************************/

#include "PackageState.h"
#include "libdiscover_debug.h"

PackageState::PackageState(QString name, QString description, bool installed)
    : PackageState(name, name, description, installed)
{}

PackageState::PackageState(QString packageName, QString name, QString description, bool installed)
    : m_packageName(std::move(packageName))
    , m_name(std::move(name))
    , m_description(std::move(description))
    , m_installed(installed)
{}

QString PackageState::name() const
{
    return m_name;
}

QString PackageState::description() const
{
    return m_description;
}

QString PackageState::packageName() const
{
    return m_packageName;
}

bool PackageState::isInstalled() const
{
    return m_installed;
}

void PackageState::setInstalled(bool installed)
{
    m_installed = installed;
}

QDebug operator<<(QDebug debug, const PackageState& state)
{
    QDebugStateSaver saver(debug);
    debug.nospace() << "PackageState(";
    debug.nospace() << state.name() << ':';
    debug.nospace() << "installed: " << state.isInstalled() << ',';
    debug.nospace() << ')';
    return debug;
}
