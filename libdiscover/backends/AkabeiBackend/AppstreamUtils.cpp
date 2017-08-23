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

// Qt includes
#include <QtCore>
#include <QFile>
#include <QDebug>

// Own includes
#include "AppstreamUtils.h"

namespace {

QStringList joinLists(const QList<QStringList>& list)
{
    QStringList ret;
    foreach(const QStringList& l, list) {
        ret += l;
    }
    return ret;
}

QHash<QString, QStringList> readElementList(QXmlStreamReader* reader, const QString& listedTagName)
{
    Q_UNUSED(listedTagName)
    Q_ASSERT(reader->isStartElement());
    QHash<QString, QStringList> ret;
    QStringRef startTag = reader->name();
    while(!(reader->isEndElement() && reader->name()==startTag)) {
        reader->readNext();

        if(reader->isStartElement()) {
            Q_ASSERT(reader->name()==listedTagName);
            ret[reader->attributes().value(QStringLiteral("lang")).toString()] += reader->readElementText();
        }
    }
    return ret;
}

ApplicationData readApplication(QXmlStreamReader* reader)
{
    Q_ASSERT(reader->isStartElement() && reader->name()==QLatin1String("application"));
    ApplicationData ret;
    while(!(reader->isEndElement() && reader->name()==QLatin1String("application"))) {
        reader->readNext();
        if(reader->isStartElement()) {
            QStringRef name = reader->name();
            QString lang = reader->attributes().value(QLatin1String("lang")).toString();
            
            if(name==QLatin1String("pkgname")) ret.pkgname = reader->readElementText();
            else if(name==QLatin1String("id")) ret.id = reader->readElementText();
            else if(name==QLatin1String("name")) ret.name[lang] = reader->readElementText();
            else if(name==QLatin1String("summary")) ret.summary[lang] = reader->readElementText();
            else if(name==QLatin1String("icon")) ret.icon = reader->readElementText();
            else if(name==QLatin1String("url")) ret.url = reader->readElementText();
            else if(name==QLatin1String("keywords")) ret.keywords = readElementList(reader, QLatin1String("keyword"));
            else if(name==QLatin1String("appcategories")) ret.appcategories = joinLists(readElementList(reader, QLatin1String("appcategory")).values());
            else if(name==QLatin1String("mimetypes")) ret.mimetypes = joinLists(readElementList(reader, QLatin1String("mimetype")).values());
            else {
                qWarning() << "unrecognized element:" << reader->name();
            }
            Q_ASSERT(reader->isEndElement());
        } else {;
            Q_ASSERT(reader->isWhitespace() || (reader->isEndElement() && reader->name()==QLatin1String("application")));
        }
    }
    return ret;
}

}

QHash<QString, ApplicationData> AppstreamUtils::fetchAppData(const QString& path)
{
    QHash<QString, ApplicationData> ret;
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly)) {
        qWarning() << "couldn't open" << path;
        return ret;
    }

    QXmlStreamReader reader(&f);
    while (!reader.atEnd()) {
        reader.readNext();
        if (reader.isStartElement() && reader.name() == QLatin1String("application")) {
            ApplicationData app = readApplication(&reader);
            ret.insert(app.pkgname, app);
        }
    }
    qDebug() << "got a number of appstream datasets:" << ret.size();

    if (reader.hasError()) {
        qWarning() << "error found while parsing" << path << ':' << reader.errorString();
    }
    return ret;
}
