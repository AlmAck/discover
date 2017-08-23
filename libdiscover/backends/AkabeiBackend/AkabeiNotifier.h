/*
 * Copyright 2013  Lukas Appelhans <l.appelhans@gmx.de>
 * Copyright 2016  Luca Giambonini <almack@chakraos.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef AKABEINOTIFIER_H
#define AKABEINOTIFIER_H

// Akabei includes
#include <akabeibackend.h>

// Discover includes
#include <BackendNotifierModule.h>

class QTimer;

class AkabeiNotifier : public BackendNotifierModule
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kde.discover.BackendNotifierModule")
    Q_INTERFACES(BackendNotifierModule)
public:
    AkabeiNotifier(QObject* parent = 0);
    virtual ~AkabeiNotifier();

    virtual bool isSystemUpToDate() const Q_DECL_OVERRIDE Q_DECL_FINAL;
    virtual void recheckSystemUpdateNeeded() Q_DECL_OVERRIDE Q_DECL_FINAL;
    virtual uint securityUpdatesCount() Q_DECL_OVERRIDE Q_DECL_FINAL;
    virtual uint updatesCount() Q_DECL_OVERRIDE Q_DECL_FINAL;
    
public Q_SLOTS:
    virtual void configurationChanged();    
    
private Q_SLOTS:
    void backendStateChanged(Akabei::Backend::Status status);
    void init();
    
private:
    QTimer * m_timer;
    int m_normalUpdates;
};

#endif // AKABEINOTIFIER_H
