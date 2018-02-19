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

#ifndef PACKAGESTATE_H
#define PACKAGESTATE_H

#include <QString>
#include "discovercommon_export.h"

/**
 * The @class PackageState will be used to expose resources related to an @class AbstractResource.
 * 
 * @see ApplicationAddonsModel
 */
class DISCOVERCOMMON_EXPORT PackageState
{
    public:
        PackageState(QString packageName, QString name, QString description, bool installed);
        PackageState(QString name, QString description, bool installed);
        PackageState& operator=(const PackageState& other);
        
        QString packageName() const;
        QString name() const;
        QString description() const;
        bool isInstalled() const;
        void setInstalled(bool installed);
        
    private:
        const QString m_packageName;
        const QString m_name;
        const QString m_description;
        bool m_installed;
};

DISCOVERCOMMON_EXPORT QDebug operator<<(QDebug dbg, const PackageState &state);

#endif // PACKAGESTATE_H
