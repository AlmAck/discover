/***************************************************************************
 *   Copyright © 2014 Aleix Pol Gonzalez <aleixpol@blue-systems.com>       *
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

#ifndef BACKENDNOTIFIERMODULE_H
#define BACKENDNOTIFIERMODULE_H

#include <QObject>
#include "discovernotifiers_export.h"

class DISCOVERNOTIFIERS_EXPORT UpgradeAction : public QObject
{
Q_OBJECT
Q_PROPERTY(QString name READ name CONSTANT)
Q_PROPERTY(QString description READ description CONSTANT)
public:
    UpgradeAction(const QString& name, const QString& description, QObject* parent)
        : QObject(parent)
        , m_name(name)
        , m_description(description)
    {}

    QString name() const { return m_name; }
    QString description() const { return m_description; }

    void trigger() { triggered(m_name); }

Q_SIGNALS:
    void triggered(const QString & name);

private:
    const QString m_name;
    const QString m_description;
};

class DISCOVERNOTIFIERS_EXPORT BackendNotifierModule : public QObject
{
Q_OBJECT
public:
    explicit BackendNotifierModule(QObject* parent = nullptr);
    ~BackendNotifierModule() override;

    /*** Check for new updates. Emits @see foundUpdates when it finds something. **/
    virtual void recheckSystemUpdateNeeded() = 0;

    /*** @returns count of !security updates only. **/
    virtual uint updatesCount() = 0;

    /*** @returns count of security updates only. **/
    virtual uint securityUpdatesCount() = 0;

    /** @returns whether the system changed in a way that needs to be rebooted. */
    virtual bool needsReboot() const = 0;
Q_SIGNALS:
    /**
     * This signal is emitted when any new updates are available.
     * @see recheckSystemUpdateNeeded
     */
    void foundUpdates();

    /** Notifies that the system needs a reboot. @see needsReboot */
    void needsRebootChanged();

    /** notifies about an available upgrade */
    void foundUpgradeAction(UpgradeAction* action);
};

Q_DECLARE_INTERFACE(BackendNotifierModule, "org.kde.discover.BackendNotifierModule")

#endif
