/*
 * Copyright 2013  Lukas Appelhans <l.appelhans@gmx.de>
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

// Qt includes
#include <QButtonGroup>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

// KF5 includes
#include <KMessageBox>
#include <KLocalizedString>

// Akabei includes
#include <akabeidatabase.h>
#include <akabeiquery.h>
#include <akabeiclientqueue.h>
#include <akabeiclientbackend.h>
#include <akabeiclienttransactionhandler.h>

// DiscoverCommon includes
#include <Transaction/TransactionModel.h>

// Own includes
#include "AkabeiTransaction.h"
#include "AkabeiResource.h"
#include "AkabeiBackend.h"
#include "AkabeiQuestion.h"

//FIXME: Transaction messages? How to show them properly?

AkabeiTransaction::AkabeiTransaction(AkabeiBackend* parent, AbstractResource* resource, Transaction::Role role)
  : Transaction(parent, resource, role),
    m_backend(parent),
    m_transaction(0)
{
    setCancellable(false);
    setStatus(Transaction::QueuedStatus);
}

AkabeiTransaction::AkabeiTransaction(AkabeiBackend* parent, AbstractResource* resource, Transaction::Role role, const AddonList& addons)
  : Transaction(parent, resource, role, addons),
    m_backend(parent),
    m_transaction(0)
{
    setCancellable(false);
    setStatus(Transaction::QueuedStatus);
}

AkabeiTransaction::~AkabeiTransaction()
{

}

void AkabeiTransaction::start()
{
    AkabeiClient::Backend::instance()->queue()->clear();
    switch (role()) {
        case Transaction::InstallRole: {
            AkabeiResource * res = qobject_cast<AkabeiResource*>(resource());
            if (res->isInstalled()) {
                finished(true);
                return;
            }
            AkabeiClient::Backend::instance()->queue()->addPackage(res->package(), AkabeiClient::Install);
            break;
        }
        case Transaction::RemoveRole: {
            AkabeiResource * res = qobject_cast<AkabeiResource*>(resource());
            if (!res->isInstalled()) {
                finished(true);
                return;
            }
            AkabeiClient::Backend::instance()->queue()->addPackage(res->installedPackage(), AkabeiClient::Remove);
            break;
        }
        case Transaction::ChangeAddonsRole:
            break;
    }
    foreach (const QString &toRemove, addons().addonsToRemove()) {
        AbstractResource * res = m_backend->resourceByPackageName(toRemove);
        if (res) {
            AkabeiClient::Backend::instance()->queue()->addPackage(qobject_cast<AkabeiResource*>(res)->installedPackage(), AkabeiClient::Remove);
        } else {
            Akabei::Package::List pkgs = Akabei::Backend::instance()->localDatabase()->queryPackages(QLatin1String("SELECT * FROM packages JOIN provides WHERE provides.provides LIKE \"") + toRemove + QLatin1Char('\"'));
            foreach (Akabei::Package * p, pkgs)
                AkabeiClient::Backend::instance()->queue()->addPackage(p, AkabeiClient::Remove);
        }
    }
    foreach (const QString &toInstall, addons().addonsToInstall()) {
        AbstractResource * res = m_backend->resourceByPackageName(toInstall);
        if (res) {
            AkabeiClient::Backend::instance()->queue()->addPackage(qobject_cast<AkabeiResource*>(res)->package(), AkabeiClient::Install);
        } else {
            Akabei::Package::List packages;
            foreach (Akabei::Database * db, Akabei::Backend::instance()->databases()) {
                packages << db->queryPackages(QLatin1String("SELECT * FROM packages JOIN provides WHERE provides.provides LIKE \"") + toInstall + QLatin1Char('\"'));
            } //NOTE: Probably ask the user here, or rather create a method in akabei to resolve the provider for me
            if (!packages.isEmpty())
                AkabeiClient::Backend::instance()->queue()->addPackage(packages.first(), AkabeiClient::Install);
        }
    }
    connect(AkabeiClient::Backend::instance()->transactionHandler(), SIGNAL(transactionCreated(AkabeiClient::Transaction*)), SLOT(transactionCreated(AkabeiClient::Transaction*)));
    connect(AkabeiClient::Backend::instance()->transactionHandler(), SIGNAL(validationFinished(bool)), SLOT(validationFinished(bool)));
    connect(AkabeiClient::Backend::instance()->transactionHandler(), SIGNAL(finished(bool)), SLOT(finished(bool)));
    connect(AkabeiClient::Backend::instance()->transactionHandler()->transactionProgress(), SIGNAL(phaseChanged(AkabeiClient::TransactionProgress::Phase)), SLOT(phaseChanged(AkabeiClient::TransactionProgress::Phase)));
    connect(AkabeiClient::Backend::instance()->transactionHandler(), SIGNAL(newTransactionMessage(QString)), SLOT(transactionMessage(QString)));
    
    foreach (AkabeiClient::QueueItem * item, AkabeiClient::Backend::instance()->queue()->items())
        qDebug() << "QUEUE ITEM" << item->package()->name() << (item->action() == AkabeiClient::Install);
    
    m_transactionMessages.clear();
    AkabeiClient::Backend::instance()->transactionHandler()->start(Akabei::ProcessingOption::NoProcessingOption);
}

void AkabeiTransaction::transactionMessage(const QString& message)
{
    m_transactionMessages.append(message);
}

void AkabeiTransaction::phaseChanged(AkabeiClient::TransactionProgress::Phase phase)
{
    switch (phase) {
        case AkabeiClient::TransactionProgress::Downloading:
            setStatus(Transaction::DownloadingStatus);
            break;
        case AkabeiClient::TransactionProgress::Processing:
            setStatus(Transaction::CommittingStatus);
            break;
        default:
            break;
    };
}

void AkabeiTransaction::transactionCreated(AkabeiClient::Transaction* transaction)
{
    qDebug() << "Transaction created";
    m_transaction = transaction;
    foreach (AkabeiClient::TransactionQuestion * q, transaction->questions()) {
        AkabeiQuestion question(q);
        q->setAnswer(question.ask());
    }
    if (!transaction->isValid()) {
        finished(false);
        return;
    } else {
        qDebug() << "Continue with transaction";
        AkabeiClient::Backend::instance()->transactionHandler()->validate();
    }
}

void AkabeiTransaction::validationFinished(bool successful)
{
    if (!successful) {
        finished(false);
        return;
    }
    
    AkabeiClient::Backend::instance()->transactionHandler()->process();
}

void AkabeiTransaction::finished(bool successful)
{
    qDebug() << "Finished" << successful;
    if (!successful) {
        QString err;
        foreach (const Akabei::Error &error, m_transaction->errors()) {
            err.append(QLatin1Char(' ') + error.description());
        }
        if (err.isEmpty())
            err = i18n("Something went wrong!");
        KMessageBox::error(0, err, i18n("Error"));
    }
    if (!m_transactionMessages.isEmpty()) {
        KMessageBox::informationList(0, i18n("There are a couple of transaction messages:"), m_transactionMessages, i18n("Transaction messages"));
    }
    setStatus(Transaction::DoneStatus);
    disconnect(AkabeiClient::Backend::instance()->transactionHandler(), 0, this, 0);
    disconnect(AkabeiClient::Backend::instance()->transactionHandler()->transactionProgress(), 0, this, 0);
    m_backend->removeFromQueue(this);
    deleteLater();
}
