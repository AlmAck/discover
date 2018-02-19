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

#include "DummyReviewsBackend.h"
#include "DummyBackend.h"
#include "DummyResource.h"
#include <ReviewsBackend/Review.h>
#include <ReviewsBackend/Rating.h>
#include <resources/AbstractResource.h>
#include <QTimer>
#include <QDebug>

DummyReviewsBackend::DummyReviewsBackend(DummyBackend* parent)
    : AbstractReviewsBackend(parent)
{}

void DummyReviewsBackend::fetchReviews(AbstractResource* app, int page)
{
    if (page>=5)
        return;

    QVector<ReviewPtr> review;
    for(int i=0; i<33; i++) {
        review += ReviewPtr(new Review(app->name(), app->packageName(), QStringLiteral("en_US"), QStringLiteral("good morning"), QStringLiteral("the morning is very good"), QStringLiteral("dummy"),
                             QDateTime(), true, page+i, i%5, 1, 1, app->packageName()));
    }
    emit reviewsReady(app, review, false);
}

Rating* DummyReviewsBackend::ratingForApplication(AbstractResource* app) const
{
    return m_ratings[app];
}

void DummyReviewsBackend::initialize()
{
    int i = 11;
    DummyBackend* b = qobject_cast<DummyBackend*>(parent());
    foreach(DummyResource* app, b->resources()) {
        if (m_ratings.contains(app))
            continue;
        auto randomRating = qrand()%10;
        Rating* rating = new Rating(app->packageName(), ++i, randomRating, QStringLiteral("\"0, 0, 0, 4, %1\"").arg(randomRating));
        rating->setParent(this);
        m_ratings.insert(app, rating);
        app->ratingFetched();
    }
    emit ratingsReady();
}

void DummyReviewsBackend::submitUsefulness(Review* r, bool useful)
{
    qDebug() << "usefulness..." << r->applicationName() << r->reviewer() << useful;
    r->setUsefulChoice(useful ? ReviewsModel::Yes : ReviewsModel::No);
}

void DummyReviewsBackend::submitReview(AbstractResource* res, const QString& a, const QString& b, const QString& c)
{
    qDebug() << "dummy submit review" << res->name() << a << b << c;
}

bool DummyReviewsBackend::isResourceSupported(AbstractResource* /*res*/) const
{
    return true;
}
