/***************************************************************************
 *   Copyright © 2013 Lukas Appelhans <boom1992@chakra-project.org>        *
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

#ifndef MUONAKABEIBACKEND_H
#define MUONAKABEIBACKEND_H

// Qt includes
#include <QVariantList>
#include <QUuid>
#include <QQueue>

// DiscoverCommon includes
#include <resources/AbstractResourcesBackend.h>
#include "Transaction/AddonList.h"
#include "discovercommon_export.h"

// Akabei includes
#include <akabeibackend.h>

// Own includes
#include "AkabeiUpdater.h"

class QAction;
class AkabeiTransaction;

struct ApplicationData
{
    QString pkgname;
    QString id;
    QHash<QString, QString> name;
    QHash<QString, QString> summary;
    QString icon;
    QString url;
    QHash<QString, QStringList> keywords;
    QStringList appcategories;
    QStringList mimetypes;
};

class DISCOVERCOMMON_EXPORT AkabeiBackend : public AbstractResourcesBackend
{
Q_OBJECT
public:
    explicit AkabeiBackend(QObject *parent = nullptr);
    virtual ~AkabeiBackend();

    virtual void setMetaData(const QString& path) override;
    virtual bool isValid() const override;
    virtual AbstractReviewsBackend *reviewsBackend() const override;
    virtual AbstractResource* resourceByPackageName(const QString& name) const override;

    virtual int updatesCount() const override;
    
    virtual QVector< AbstractResource* > allResources() const override;
    virtual QList<AbstractResource*> searchPackageName(const QString& searchText) override;
    
    void installApplication(AbstractResource *app, const AddonList& addons);
    virtual void installApplication(AbstractResource *app) override;
    virtual void removeApplication(AbstractResource *app) override;
    virtual void cancelTransaction(AbstractResource *app) override;
    
    virtual AbstractBackendUpdater* backendUpdater() const override;
    virtual QList<AbstractResource*> upgradeablePackages() const override;
    
    void removeFromQueue(AkabeiTransaction * trans);
    
    bool isTransactionRunning() const;

    virtual bool isFetching() const override { return m_isFetching; }
    virtual QList<QAction*> messageActions() const override { return m_messageActions; }

public Q_SLOTS:
    void statusChanged(Akabei::Backend::Status);
    void queryComplete(QUuid,QList<Akabei::Package*>);
    void reload();
    
private:
    QHash<QString, AbstractResource*> m_packages;
    QQueue<AkabeiTransaction*> m_transactionQueue;
    AkabeiUpdater* m_updater;
    QHash<QString, ApplicationData> m_appdata;
    bool m_isFetching;
    QList<QAction*> m_messageActions;
};

#endif
