/*
 *   Copyright (C) 2014 Aleix Pol Gonzalez <aleixpol@blue-systems.com>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library/Lesser General Public License
 *   version 2, or (at your option) any later version, as published by the
 *   Free Software Foundation
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details
 *
 *   You should have received a copy of the GNU Library/Lesser General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef PAGINATEMODEL_H
#define PAGINATEMODEL_H

#include <QAbstractListModel>

/**
 * @class PaginateModel
 *
 * This class can be used to create representations of only a chunk of a model.
 *
 * With this component it will be possible to create views that only show a page
 * of a model, instead of drawing all the elements in the model.
 */
class PaginateModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int pageSize READ pageSize WRITE setPageSize NOTIFY pageSizeChanged)
    Q_PROPERTY(int firstItem READ firstItem WRITE setFirstItem NOTIFY firstItemChanged)
    Q_PROPERTY(QAbstractItemModel* sourceModel READ sourceModel WRITE setSourceModel NOTIFY sourceModelChanged)
    Q_PROPERTY(int currentPage READ currentPage NOTIFY firstItemChanged)
    Q_PROPERTY(int pageCount READ pageCount NOTIFY pageCountChanged)

    /** If enabled, ensures that pageCount and pageSize are the same. */
    Q_PROPERTY(bool staticRowCount READ hasStaticRowCount WRITE setStaticRowCount)

    public:
        explicit PaginateModel(QObject* object = nullptr);
        ~PaginateModel() override;

        int pageSize() const;
        void setPageSize(int count);

        int firstItem() const;
        void setFirstItem(int row);

        /**
         * @returns Last visible item.
         *
         * Convenience function
         */
        int lastItem() const;

        QAbstractItemModel* sourceModel() const;
        void setSourceModel(QAbstractItemModel* model);

        QModelIndex mapToSource(const QModelIndex& idx) const;
        QModelIndex mapFromSource(const QModelIndex& idx) const;
        int rowCount(const QModelIndex& parent = QModelIndex()) const override;
        QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

        int currentPage() const;
        int pageCount() const;
        QHash<int, QByteArray> roleNames() const override;

        void setStaticRowCount(bool src);
        bool hasStaticRowCount() const;

        Q_SCRIPTABLE void firstPage();
        Q_SCRIPTABLE void nextPage();
        Q_SCRIPTABLE void previousPage();
        Q_SCRIPTABLE void lastPage();

    private Q_SLOTS:
        void _k_sourceRowsAboutToBeInserted(const QModelIndex &parent, int start, int end);
        void _k_sourceRowsInserted(const QModelIndex &parent, int start, int end);
        void _k_sourceRowsAboutToBeRemoved(const QModelIndex &parent, int start, int end);
        void _k_sourceRowsRemoved(const QModelIndex &parent, int start, int end);
        void _k_sourceRowsAboutToBeMoved(const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destParent, int dest);
        void _k_sourceRowsMoved(const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destParent, int dest);

        void _k_sourceColumnsAboutToBeInserted(const QModelIndex &parent, int start, int end);
        void _k_sourceColumnsInserted(const QModelIndex &parent, int start, int end);
        void _k_sourceColumnsAboutToBeRemoved(const QModelIndex &parent, int start, int end);
        void _k_sourceColumnsRemoved(const QModelIndex &parent, int start, int end);
        void _k_sourceColumnsAboutToBeMoved(const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destParent, int dest);
        void _k_sourceColumnsMoved(const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destParent, int dest);

        void _k_sourceDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles);
        void _k_sourceHeaderDataChanged(Qt::Orientation orientation, int first, int last);

        void _k_sourceModelAboutToBeReset();
        void _k_sourceModelReset();

    Q_SIGNALS:
        void pageSizeChanged();
        void firstItemChanged();
        void sourceModelChanged();
        void pageCountChanged();

    private:
        bool canSizeChange() const;
        bool isIntervalValid(const QModelIndex& parent, int start, int end) const;

        int rowsByPageSize(int size) const;
        int m_firstItem;
        int m_pageSize;
        QAbstractItemModel* m_sourceModel;
        bool m_hasStaticRowCount;
};

#endif
