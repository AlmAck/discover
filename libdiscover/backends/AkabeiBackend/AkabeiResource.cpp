/***************************************************************************
 *   Copyright © 2013 Lukas Appelhans <boom1992@chakra-project.org>        *
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
#include <QtCore/QStringList>
#include <QFile>

// QJson includes
//#include <qjson/parser.h>//FIXME: Search in CMakeLists.txt for it?

// Akabei includes
#include <akabeidatabase.h>
#include <akabeiquery.h>
#include <akabeigroup.h>

// KF5 includes
//#include <KIO/TransferJob>
//#include <KIO/Job>
#include <kio/job.h>

#include <MuonDataSources.h>

// Own includes
#include "AkabeiResource.h"
#include "AkabeiBackend.h"

AkabeiResource::AkabeiResource(Akabei::Package * pkg, AkabeiBackend * parent)
  : AbstractResource(parent),
    m_pkg(0),
    m_installedPkg(0)
{
    addPackage(pkg);
}
        
QString AkabeiResource::packageName() const
{
    return m_pkg->name();
}
        
QString AkabeiResource::name()
{
    return m_pkg->name();
}
        
QString AkabeiResource::comment()
{
    return m_pkg->description();
}

QString AkabeiResource::longDescription()
{
    return m_pkg->description();
}
        
QString AkabeiResource::icon() const
{
    return QStringLiteral("akabei");
}
        
bool AkabeiResource::canExecute() const
{
    return false;
}
        
void AkabeiResource::invokeApplication() const
{
}

void AkabeiResource::addPackage(Akabei::Package* pkg)
{
    if (pkg->database() == Akabei::Backend::instance()->localDatabase()) {
        if (!m_installedPkg || m_installedPkg->version() <= pkg->version())
            m_installedPkg = pkg;
        if (!m_pkg)
            m_pkg = pkg;
    } else if (!m_pkg || m_pkg->version() <= pkg->version()) {
        m_pkg = pkg;
    }
    //if (s != state()) //FIXME: Save the old state before callin clearPackages
        emit stateChanged();
}

void AkabeiResource::clearPackages()
{
    m_pkg = 0;
    m_installedPkg = 0;
}
        
AbstractResource::State AkabeiResource::state()
{
    if (m_installedPkg && m_installedPkg->version() >= m_pkg->version())
        return AbstractResource::Installed;
    else if (m_installedPkg)
        return AbstractResource::Upgradeable;
    return AbstractResource::None;
}
        
QStringList AkabeiResource::categories()
{
    return QStringList(QStringLiteral("Unknown"));//We just rely on appstream for this until it's implemented in akabei
}
        
QUrl AkabeiResource::homepage()
{
    return m_pkg->url();
}
        
bool AkabeiResource::isTechnical() const
{
    return true;
}

QUrl AkabeiResource::thumbnailUrl()
{
    return QUrl(MuonDataSources::screenshotsSource().toString() + QStringLiteral("/thumbnail/") + packageName());
}

QUrl AkabeiResource::screenshotUrl()
{
    if (m_pkg && !m_pkg->screenshot().isEmpty()) {
        return m_pkg->screenshot();
    }
    return QUrl(MuonDataSources::screenshotsSource().toString() + QStringLiteral("/screenshot/") + packageName());
}
        
int AkabeiResource::size()
{
    return m_pkg->size();
}

QString AkabeiResource::license()
{
    return m_pkg->licenses().join(QStringLiteral(", "));
}
        
QString AkabeiResource::installedVersion() const
{
    if (!m_installedPkg)
        return QString();
    return QString::fromLatin1(m_installedPkg->version().toByteArray().data());
}

QString AkabeiResource::availableVersion() const
{
    return QString::fromLatin1(m_pkg->version().toByteArray().data());
}
        
QString AkabeiResource::origin() const
{
    return m_pkg->database()->name();
}

QString AkabeiResource::section()
{
    if (m_pkg->groups().isEmpty())
        return QStringLiteral("unknown");
    return m_pkg->groups().first()->name();//FIXME: Probably add support for multiple sections?
}
        
QStringList AkabeiResource::mimetypes() const
{
    return QStringList(QStringLiteral(""));
}
        
QList<PackageState> AkabeiResource::addonsInformation()
{
    QList<PackageState> states;
    foreach (const QString &optdep, m_pkg->optionalDependencies()) {
        QStringList split = optdep.split(QLatin1Char(':'));
        if (split.count() >= 2) {
            bool installed = !Akabei::Backend::instance()->localDatabase()->queryPackages(Akabei::Queries::selectPackages(QStringLiteral("name"), QStringLiteral("LIKE"), split.first())).isEmpty();
            if (!installed) {
                installed = !Akabei::Backend::instance()->localDatabase()->queryPackages(QLatin1String("SELECT * FROM packages JOIN provides WHERE provides.provides LIKE \"") + split.first() + QLatin1Char('\"')).isEmpty();
            }
            states.append(PackageState(split.first(), split.at(1), installed));
        }
    }
    return states;
}

bool AkabeiResource::isFromSecureOrigin() const
{
    return true;
}
        
QStringList AkabeiResource::executables() const
{
    return QStringList(QStringLiteral(""));
}

Akabei::Package * AkabeiResource::package() const
{
    return m_pkg;
}

Akabei::Package * AkabeiResource::installedPackage() const
{
    return m_installedPkg;
}

void AkabeiResource::fetchScreenshots()
{
    // https://community.kde.org/Frameworks/Porting_Notes/KStandardDirs
    QString dest = QLatin1String("/tmp/screenshot.") + packageName(); //KStandardDirs::locate("tmp", "screenshots." + packageName());
    
    QFile f(dest);
    if (f.exists())
        f.remove();
    QUrl packageUrl(MuonDataSources::screenshotsSource().toString() + QStringLiteral("/json/package/") + packageName());
    
    /*
    KIO::Job* getJob = KIO::file_copy(packageUrl, QUrl(dest), -1, KIO::Overwrite | KIO::HideProgressInfo);
    connect(getJob, SIGNAL( result( KJob * ) , this, &AkabeiResource::slotScreenshotsFetched);
    getJob->start();
    */
}

void AkabeiResource::slotScreenshotsFetched(KJob * job)
{
    if (job->error() != KJob::NoError) {
        qWarning() << job->errorString();
    }
    bool done = false;
    QString dest = QLatin1String("/tmp/screenshot.") + packageName(); //KStandardDirs::locate("tmp", "screenshots." + packageName());

    QFile f(dest);
    if (f.exists()) {
        bool b = f.open(QIODevice::ReadOnly);
        Q_ASSERT(b);

        /*
        QJson::Parser p;
        bool ok;
        QVariantMap values = p.parse(&f, &ok).toMap();
        if(ok) {
            QVariantList screenshots = values[QStringLiteral("screenshots")].toList();
            
            QList<QUrl> thumbnailUrls, screenshotUrls;
            foreach(const QVariant& screenshot, screenshots) {
                qDebug() << screenshot;
                QVariantMap s = screenshot.toMap();
                thumbnailUrls += s[QStringLiteral("small_image_url")].toUrl();
                screenshotUrls += s[QStringLiteral("large_image_url")].toUrl();
            }
            emit screenshotsFetched(thumbnailUrls, screenshotUrls);
            done = true;
        }*/
    }
    if(!done) {
        QList<QUrl> thumbnails, screenshots;
        if(!thumbnailUrl().isEmpty()) {
            thumbnails += thumbnailUrl();
            screenshots += screenshotUrl();
        }
        emit screenshotsFetched(thumbnails, screenshots);
    }
}

void AkabeiResource::fetchChangelog() 
{
    emit changelogFetched(m_pkg->retrieveChangelog());
}

