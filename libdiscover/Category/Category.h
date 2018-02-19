/***************************************************************************
 *   Copyright © 2010 Jonathan Thomas <echidnaman@kubuntu.org>             *
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

#ifndef CATEGORY_H
#define CATEGORY_H

#include <QtCore/QVector>
#include <QtCore/QPair>
#include <QtCore/QObject>
#include <QtCore/QSet>
#include <QtCore/QUrl>

#include "discovercommon_export.h"

class QDomNode;

enum FilterType {
    InvalidFilter,
    CategoryFilter,
    PkgSectionFilter,
    PkgWildcardFilter,
    PkgNameFilter,
    AppstreamIdWildcardFilter
};

class DISCOVERCOMMON_EXPORT Category : public QObject
{
Q_OBJECT
public:
    Q_PROPERTY(QString name READ name CONSTANT)
    Q_PROPERTY(QString icon READ icon CONSTANT)
    Q_PROPERTY(QObject* parent READ parent CONSTANT)
    Q_PROPERTY(QUrl decoration READ decoration CONSTANT)
    Q_PROPERTY(QVariantList subcategories READ subCategoriesVariant NOTIFY subCategoriesChanged)
    explicit Category(QSet<QString>  pluginNames, QObject* parent = nullptr);

    Category(const QString& name, const QString& iconName, const QVector< QPair< FilterType, QString > >& orFilters, const QSet<QString> &pluginName, const QVector<Category *>& subCategories, const QUrl& decoration, bool isAddons);
    ~Category() override;

    QString name() const;
    QString icon() const;
    QVector<QPair<FilterType, QString> > andFilters() const;
    QVector<QPair<FilterType, QString> > orFilters() const;
    QVector<QPair<FilterType, QString> > notFilters() const;
    QVector<Category *> subCategories() const;
    QVariantList subCategoriesVariant() const;

    static void sortCategories(QVector<Category*>& cats);
    static void addSubcategory(QVector<Category*>& cats, Category* cat);
    void parseData(const QString& path, const QDomNode& data);
    bool blacklistPlugins(const QSet<QString>& pluginName);
    bool isAddons() const { return m_isAddons; }
    QUrl decoration() const;
    bool matchesCategoryName(const QString &name) const;

    Q_SCRIPTABLE bool contains(Category* cat) const;
    Q_SCRIPTABLE bool contains(const QVariantList &cats) const;

    static bool categoryLessThan(Category *c1, const Category *c2);
    static bool blacklistPluginsInVector(const QSet<QString>& pluginNames, QVector<Category *>& subCategories);

Q_SIGNALS:
    void subCategoriesChanged();

private:
    QString m_name;
    QString m_iconString;
    QUrl m_decoration;
    QVector<QPair<FilterType, QString> > m_andFilters;
    QVector<QPair<FilterType, QString> > m_orFilters;
    QVector<QPair<FilterType, QString> > m_notFilters;
    QVector<Category *> m_subCategories;

    QVector<QPair<FilterType, QString> > parseIncludes(const QDomNode &data);
    QSet<QString> m_plugins;
    bool m_isAddons = false;
};

#endif
