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
#ifndef AKABEIRESOURCE_H
#define AKABEIRESOURCE_H

// DiscoverCommon includes
#include <resources/AbstractResource.h>
// Akabei includes
#include <akabeipackage.h>

class KJob;
class AkabeiBackend;

class DISCOVERCOMMON_EXPORT AkabeiResource : public AbstractResource
{
    Q_OBJECT
    public:
        AkabeiResource(Akabei::Package * pkg, AkabeiBackend * parent);
        
        ///used as internal identification of a resource
        virtual QString packageName() const override;
        
        ///resource name to be displayed
        virtual QString name();
        
        ///short description of the resource
        virtual QString comment();
        
        ///xdg-compatible icon name to represent the resource
        virtual QString icon() const override;
        
        ///@returns whether invokeApplication makes something
        /// false if not overridden
        virtual bool canExecute() const override;
        
        ///executes the resource, if applies.
        virtual void invokeApplication() const override;
        
        virtual State state();
        
        virtual QStringList categories();
        
        ///@returns a URL that points to the content
        virtual QUrl homepage();
        
        virtual bool isTechnical() const override;

        virtual QUrl thumbnailUrl();
        virtual QUrl screenshotUrl();
        
        virtual int size();
        virtual QString license();
        
        virtual QString installedVersion() const override;
        virtual QString availableVersion() const override;
        virtual QString longDescription();
        
        virtual QString origin() const override;
        virtual QString section();
        
        ///@returns what kind of mime types the resource can consume
        virtual QStringList mimetypes() const override;
        
        virtual QList<PackageState> addonsInformation();
        bool isFromSecureOrigin() const;
        
        virtual QStringList executables() const override;
        
        Akabei::Package * package() const;
        Akabei::Package * installedPackage() const;

    public Q_SLOTS:
        virtual void fetchScreenshots();
        virtual void fetchChangelog();
        void addPackage(Akabei::Package * pkg);
        void clearPackages();
        
    private Q_SLOTS:
        void slotScreenshotsFetched(KJob *);
        
    private:
        Akabei::Package * m_pkg;
        Akabei::Package * m_installedPkg;
};

#endif // ABSTRACTRESOURCE_H
