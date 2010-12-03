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

#include "ApplicationProxyModel.h"

#include <QtCore/QRegExp>

#include <LibQApt/Backend>

// Own includes
#include "ApplicationModel.h"

ApplicationProxyModel::ApplicationProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
    , m_backend(0)
    , m_stateFilter((QApt::Package::State)0)
    , m_sortByRelevancy(false)
{
}

ApplicationProxyModel::~ApplicationProxyModel()
{
}

void ApplicationProxyModel::setBackend(QApt::Backend *backend)
{
    m_backend = backend;
    m_apps = static_cast<ApplicationModel *>(sourceModel())->applications();
    search("cd burning");
}

void ApplicationProxyModel::search(const QString &searchText)
{
    // 1-character searches are painfully slow. >= 2 chars are fine, though
    m_packages.clear();
    if (searchText.size() > 1) {
        m_packages = m_backend->search(searchText);
        m_sortByRelevancy = true;
    } else {
        m_sortByRelevancy = false;
    }
    invalidate();
}

void ApplicationProxyModel::setStateFilter(QApt::Package::State state)
{
    m_stateFilter = state;
    invalidate();
}

void ApplicationProxyModel::setOriginFilter(const QString &origin)
{
    m_originFilter = origin;
    invalidate();
}

void ApplicationProxyModel::setFiltersFromCategory(Category *category)
{
    m_andFilters = category->andFilters();
    m_orFilters = category->orFilters();
    m_notFilters = category->notFilters();
    invalidate();
}

bool ApplicationProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    Application *application = static_cast<ApplicationModel *>(sourceModel())->applicationAt(sourceModel()->index(sourceRow, 0, sourceParent));
    //We have a package as internal pointer
    if (!application || !application->package()) {
        return false;
    }

    if (m_stateFilter) {
        if ((bool)(application->package()->state() & m_stateFilter) == false) {
            return false;
        }
    }

    if (!m_originFilter.isEmpty()) {
        if (application->package()->origin() != m_originFilter) {
            return false;
        }
    }

    if (!m_orFilters.isEmpty()) {
        // Set a boolean value to true when any of the conditions are found.
        // It is set to false by default so that if none are found, we return false
        QList<QPair<FilterType, QString> >::const_iterator filter = m_orFilters.constBegin();
        bool foundOrCondition = false;
        while (filter != m_orFilters.constEnd()) {
            switch ((*filter).first) {
            case CategoryFilter:
                if (application->categories().contains((*filter).second)) {
                    foundOrCondition = true;
                }
                break;
            case PkgSectionFilter:
                if (application->package()->section() == (*filter).second) {
                    foundOrCondition = true;
                }
                break;
            case PkgWildcardFilter: {
                QRegExp rx((*filter).second);
                rx.setPatternSyntax(QRegExp::Wildcard);

                if (rx.exactMatch(application->package()->name())) {
                    foundOrCondition = true;
                }
                break;
            }
            case PkgNameFilter: // Only useful in the not filters
            case InvalidFilter:
            default:
                break;
            }

            ++filter;
        }

        if (!foundOrCondition) {
            return false;
        }
    }

    if (!m_andFilters.isEmpty()) {
        // Set a boolean value to false when any conditions fail to meet
        QList<QPair<FilterType, QString> >::const_iterator filter = m_andFilters.constBegin();
        bool andConditionsMet = true;
        while (filter != m_andFilters.constEnd()) {
            switch ((*filter).first) {
            case CategoryFilter:
                if (!application->categories().contains((*filter).second)) {
                    andConditionsMet = false;
                }
                break;
            case PkgSectionFilter:
                if (!(application->package()->section() == (*filter).second)) {
                    andConditionsMet = false;
                }
                break;
            case PkgWildcardFilter: {
                QRegExp rx((*filter).second);
                rx.setPatternSyntax(QRegExp::Wildcard);

                if (!rx.exactMatch(application->package()->name())) {
                    andConditionsMet = false;
                }
            }
                break;
            case PkgNameFilter: // Only useful in the not filters
            case InvalidFilter:
            default:
                break;
            }

            ++filter;
        }

        if (!andConditionsMet) {
            return false;
        }
    }

    if (!m_notFilters.isEmpty()) {
        QList<QPair<FilterType, QString> >::const_iterator filter = m_notFilters.constBegin();
        while (filter != m_notFilters.constEnd()) {
            switch ((*filter).first) {
            case CategoryFilter:
                if (application->categories().contains((*filter).second)) {
                    return false;
                }
                break;
            case PkgSectionFilter:
                if (application->package()->section() == (*filter).second) {
                    return false;
                }
                break;
            case PkgWildcardFilter: {
                QRegExp rx((*filter).second);
                rx.setPatternSyntax(QRegExp::Wildcard);

                if (rx.exactMatch(application->package()->name())) {
                    return false;
                }
            }
                break;
            case PkgNameFilter:
                if (application->package()->name() == (*filter).second) {
                    return false;
                }
                break;
            case InvalidFilter:
            default:
                break;
            }

            ++filter;
        }
    }

    if(m_sortByRelevancy) {
        return m_packages.contains(application->package());
    }

    return true;
}

Application *ApplicationProxyModel::applicationAt(const QModelIndex &index) const
{
    // Since our representation is almost bound to change, we need to grab the parent model's index
    QModelIndex sourceIndex = mapToSource(index);
    Application *application = static_cast<ApplicationModel *>(sourceModel())->applicationAt(sourceIndex);
    return application;
}

void ApplicationProxyModel::reset()
{
    beginRemoveRows(QModelIndex(), 0, m_apps.size());
    m_apps = static_cast<ApplicationModel *>(sourceModel())->applications();
    endRemoveRows();
    invalidate();
}

void ApplicationProxyModel::parentDataChanged()
{
    m_apps = static_cast<ApplicationModel *>(sourceModel())->applications();
    invalidate();
}

bool ApplicationProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    ApplicationModel *model = static_cast<ApplicationModel *>(sourceModel());
    QApt::Package *leftPackage = model->applicationAt(left)->package();
    QApt::Package *rightPackage = model->applicationAt(right)->package();

    if (m_sortByRelevancy) {
        // This is expensive for very large datasets. It takes about 3 seconds with 30,000 packages
        // The order in m_packages is based on relevancy when returned by m_backend->search()
        // Use this order to determine less than
        if (m_packages.indexOf(leftPackage) > m_packages.indexOf(rightPackage)) {
            return false;
        } else {
            return true;
        }
    }
    QString leftString = left.data(ApplicationModel::NameRole).toString();
    QString rightString = right.data(ApplicationModel::NameRole).toString();

    return (QString::localeAwareCompare(leftString, rightString) < 0);
}
