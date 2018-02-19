/***************************************************************************
 *   Copyright © 2017 Jan Grulich <jgrulich@redhat.com>                    *
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

#ifndef FLATPAKTRANSACTIONJOB_H
#define FLATPAKTRANSACTIONJOB_H

extern "C" {
#include <flatpak.h>
#include <gio/gio.h>
#include <glib.h>
}

#include <Transaction/Transaction.h>
#include <QThread>

class FlatpakResource;
class FlatpakTransactionJob : public QThread
{
Q_OBJECT
public:
    FlatpakTransactionJob(FlatpakResource *app, const QPair<QString, uint> &relatedRef, Transaction::Role role, QObject *parent = nullptr);
    ~FlatpakTransactionJob();

    void cancel();
    void run() override;

    FlatpakResource * app() const;

    bool isRelated() const;

    int progress() const;
    void setProgress(int progress);

    QString errorMessage() const;
    bool result() const;

Q_SIGNALS:
    void progressChanged(int progress);

private:
    bool m_result;
    int m_progress;
    QString m_errorMessage;
    QString m_relatedRef;
    uint m_relatedRefKind;
    GCancellable *m_cancellable;
    FlatpakResource *m_app;
    Transaction::Role m_role;
};

#endif // FLATPAKTRANSACTIONJOB_H

