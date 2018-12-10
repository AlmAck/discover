/***************************************************************************
 *   Copyright © 2013 Aleix Pol Gonzalez <aleixpol@blue-systems.com>       *
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

#ifndef SNAPTRANSACTION_H
#define SNAPTRANSACTION_H

#include <resources/AbstractResource.h>
#include <Transaction/Transaction.h>
#include <QPointer>

class SnapResource;
class QSnapdRequest;
class QSnapdClient;

class SnapTransaction : public Transaction
{
    Q_OBJECT
    public:
        SnapTransaction(QSnapdClient* client, SnapResource* app, Role role, AbstractResource::State newState);

        void cancel() override;
        void proceed() override;

    private Q_SLOTS:
        void finishTransaction();

    private:
        void setRequest(QSnapdRequest* req);
        void progressed();

        QSnapdClient * const m_client;
        SnapResource * const m_app;
        QScopedPointer<QSnapdRequest> m_request;
        const AbstractResource::State m_newState;
};

#endif // SNAPTRANSACTION_H
