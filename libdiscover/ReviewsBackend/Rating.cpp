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

#include "Rating.h"
#include <QStringList>
#include <QDebug>
#include <qmath.h>

inline double fastPow(double a, double b) {
    union {
        double d;
        int x[2];
    } u = { a };

#if Q_BYTE_ORDER == Q_LITTLE_ENDIAN
    u.x[1] = (int)(b * (u.x[1] - 1072632447) + 1072632447);
    u.x[0] = 0;
#else
    u.x[1] = 0;
    u.x[0] = (int)(b * (u.x[1] - 1072632447) + 1072632447);
#endif

    return u.d;
}

// Converted from a Ruby example, returns an inverse normal distribution
double pnormaldist(double qn)
{
    double b[] = {1.570796288, 0.03706987906, -0.8364353589e-3, -0.2250947176e-3, 0.6841218299e-5, 0.5824238515e-5, -0.104527497e-5, 0.8360937017e-7, -0.3231081277e-8, 0.3657763036e-10, 0.6936233982e-12};

    if(qn < 0.0 || 1.0 < qn)
        return 0.0;

    if(qn == 0.5)
        return 0.0;

    double w1 = qn;
    if(qn > 0.5)
        w1 = 1.0 - w1;
    double w3 = -qLn(4.0 * w1 * (1.0 - w1));
    w1 = b[0];

    for(int i = 1; i < 11; i++)
        w1 += b[i] * fastPow(w3,i);

    if(qn > 0.5)
        return qSqrt(w1*w3);
    return -qSqrt(w1*w3);
}

double wilson_score(int pos, int n, double power = 0.2)
{
    if (n == 0)
        return 0;

    double z = pnormaldist(1 - power / 2);
    double phat = 1.0 * pos / n;
    return (phat + z * z / (2 * n) - z * qSqrt(
            (phat * (1 - phat) + z * z / (4 * n)) / n)) / (1 + z * z / n);
}

double dampenedRating(const QVector<int> &ratings, double power = 0.1)
{
    if (ratings.count() != 5)
        return 0;

    int tot_ratings = 0;
    Q_FOREACH (const int rating, ratings)
            tot_ratings = rating + tot_ratings;

    double sum_scores = 0.0;

    for (int i = 0; i < ratings.count(); i++) {
        const int rating = ratings.at(i);
        double ws = wilson_score(rating, tot_ratings, power);
        sum_scores = sum_scores + float((i + 1) - 3) * ws;
    }

    return sum_scores + 3;
}

Rating::Rating(const QString &packageName, quint64 ratingCount, const QVariantMap &data)
    : QObject()
    , m_packageName(packageName)
    , m_ratingCount(ratingCount)
    // TODO consider storing this and present in UI
    , m_rating(((data.value(QStringLiteral("star1")).toInt() + (data.value(QStringLiteral("star2")).toInt() * 2) +
                (data.value(QStringLiteral("star3")).toInt() * 3) + (data.value(QStringLiteral("star4")).toInt() * 4) +
                (data.value(QStringLiteral("star5")).toInt() * 5)) * 2) / (float) ratingCount)
    , m_ratingPoints(0)
    , m_sortableRating(0)
{
    const QVector<int> histo = { data.value(QStringLiteral("star1")).toInt(), data.value(QStringLiteral("star2")).toInt(),
                                 data.value(QStringLiteral("star3")).toInt(), data.value(QStringLiteral("star4")).toInt(),
                                 data.value(QStringLiteral("star5")).toInt() };
    QVector<int> spread;
    spread.reserve(histo.size());

    for(int i=0; i<histo.size(); ++i) {
        int points = histo[i];
        m_ratingPoints += (i+1)*points;
        spread.append(points);
    }

    m_sortableRating = dampenedRating(spread) * 2;
}

Rating::Rating(QString packageName, int inst)
    : QObject()
    , m_packageName(std::move(packageName))
    , m_ratingCount(inst)
    , m_rating(0)
    , m_ratingPoints(0)
    , m_sortableRating(m_rating)
{
}

Rating::Rating(const QString &packageName, quint64 ratingCount, double rating, const QString &histogram)
    : QObject()
    , m_packageName(packageName)
    , m_ratingCount(ratingCount)
    , m_rating(rating)
    , m_ratingPoints(0)
    , m_sortableRating(0)
{
    Q_ASSERT(rating <= 10 && rating>=-1);

    const auto histo = histogram.midRef(1,histogram.size()-2).split(QStringLiteral(", "));
    QVector<int> spread;
    spread.reserve(histo.size());

    for(int i=0; i<histo.size(); ++i) {
        int points = histo[i].toInt();
        m_ratingPoints += (i+1)*points;
        spread.append(points);
    }

    m_sortableRating = dampenedRating(spread) * 2;
}

Rating::~Rating() = default;

QString Rating::packageName() const
{
    return m_packageName;
}

quint64 Rating::ratingCount() const
{
    return m_ratingCount;
}

float Rating::rating() const
{
    return m_rating;
}

int Rating::ratingPoints() const
{
    return m_ratingPoints;
}

double Rating::sortableRating() const
{
    return m_sortableRating;
}
