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

#ifndef OCSRESOURCE_H
#define OCSRESOURCE_H

#include <resources/AbstractResource.h>
#include <attica/content.h>

class OCSResource : public AbstractResource
{
    Q_OBJECT
    public:
        explicit OCSResource(const Attica::Content& content, QObject* parent = 0);
        
        virtual QString name();
        virtual QString comment();
        virtual QString icon() const;
        virtual QString packageName() const;
        virtual State state();
        virtual QString categories();
        virtual QUrl homepage() const;
        virtual QUrl thumbnailUrl();

        virtual QString license() { return QString(); }
        virtual QString longDescription() const { return QString(); }
        virtual QList<PackageState> addonsInformation() { return QList<PackageState>(); }
    private:
        Attica::Content m_content;
};

#endif // OCSRESOURCE_H