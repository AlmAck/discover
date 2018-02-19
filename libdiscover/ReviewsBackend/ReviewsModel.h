/***************************************************************************
 *   Copyright © 2012 Aleix Pol Gonzalez <aleixpol@blue-systems.com>       *
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

#ifndef REVIEWSMODEL_H
#define REVIEWSMODEL_H

#include <QModelIndex>
#include <QSharedPointer>
#include "discovercommon_export.h"

class Review;
typedef QSharedPointer<Review> ReviewPtr;

class AbstractResource;
class AbstractReviewsBackend;
class DISCOVERCOMMON_EXPORT ReviewsModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(AbstractReviewsBackend* backend READ backend NOTIFY resourceChanged)
    Q_PROPERTY(AbstractResource* resource READ resource WRITE setResource NOTIFY resourceChanged)
    Q_PROPERTY(int count READ rowCount NOTIFY rowsChanged)
    public:
        enum Roles {
            ShouldShow=Qt::UserRole+1,
            Reviewer,
            CreationDate,
            UsefulnessTotal,
            UsefulnessFavorable,
            UsefulChoice,
            Rating,
            Summary
        };
        enum UserChoice {
            None,
            Yes,
            No
        };
        Q_ENUM(UserChoice)

        explicit ReviewsModel(QObject* parent = nullptr);
        ~ReviewsModel() override;
        QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
        int rowCount(const QModelIndex& parent = QModelIndex()) const override;

        AbstractReviewsBackend* backend() const;
        void setResource(AbstractResource* app);
        AbstractResource* resource() const;
        void fetchMore(const QModelIndex& parent=QModelIndex()) override;
        bool canFetchMore(const QModelIndex& /*parent*/) const override;
        QHash<int, QByteArray> roleNames() const override;

    public Q_SLOTS:
        void deleteReview(int row);
        void flagReview(int row, const QString& reason, const QString& text);
        void markUseful(int row, bool useful);

    private Q_SLOTS:
        void addReviews(AbstractResource* app, const QVector<ReviewPtr>& reviews, bool canFetchMore);
        void restartFetching();

    Q_SIGNALS:
        void rowsChanged();
        void resourceChanged();

    private:
        AbstractResource* m_app;
        AbstractReviewsBackend* m_backend;
        QVector<ReviewPtr> m_reviews;
        int m_lastPage;
        bool m_canFetchMore;
};

#endif // REVIEWSMODEL_H

