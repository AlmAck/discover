/***************************************************************************
 *   Copyright © 2016 Aleix Pol Gonzalez <aleixpol@blue-systems.com>       *
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

#ifndef LOCALFILEPKRESOURCE_H
#define LOCALFILEPKRESOURCE_H

#include "PackageKitResource.h"

class LocalFilePKResource : public PackageKitResource
{
    Q_OBJECT
    public:
        LocalFilePKResource(QUrl path, PackageKitBackend* parent);

        QString name() const override;
        QString comment() override;

        AbstractResource::State state() override { return m_state; }
        QString license() override;
        int size() override;
        void markInstalled();
        QString origin() const override;
        void fetchDetails() override;
        bool canExecute() const override { return !m_exec.isEmpty(); }
        void invokeApplication() const override;

    private:
        AbstractResource::State m_state = AbstractResource::None;
        QUrl m_path;
        QString m_exec;
};

#endif // LOCALFILEPKRESOURCE_H
