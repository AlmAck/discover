/***************************************************************************
 *   Copyright © 2011 Jonathan Thomas <echidnaman@kubuntu.org>             *
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

#ifndef UPDATEITEM_H
#define UPDATEITEM_H

// Qt includes
#include <QtCore/QSet>
#include <QtCore/QString>
#include "discovercommon_export.h"

#include <QIcon>

class AbstractResource;
class DISCOVERCOMMON_EXPORT UpdateItem
{
public:
    explicit UpdateItem(AbstractResource *app);

    ~UpdateItem();


    void setSection(const QString &section) { m_section = section; }
    QString section() const { return m_section; }
    void setProgress(qreal progress);
    qreal progress() const;

    QString changelog() const;
    void setChangelog(const QString &changelog);

    AbstractResource *app() const;
    QString name() const;
    QString version() const;
    QVariant icon() const;
    qint64 size() const;
    Qt::CheckState checked() const;

    AbstractResource* resource() const { return m_app; }

private:
    AbstractResource * const m_app;

    const QString m_categoryName;
    const QIcon m_categoryIcon;
    qreal m_progress;
    QString m_changelog;
    QString m_section;
};

#endif // UPDATEITEM_H
