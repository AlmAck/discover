/*
 *   Copyright (C) 2012 Aleix Pol Gonzalez <aleixpol@blue-systems.com>
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

#include "MuonDiscoverMainWindow.h"
#include <KShortcutsDialog>
#include "PaginateModel.h"
#include "SystemFonts.h"
#include "IconColors.h"

// Qt includes
#include <QDebug>
#include <QDesktopServices>
#include <QtQml/QQmlEngine>
#include <QtQml/QQmlContext>
#include <QtQml/QQmlApplicationEngine>
#include <QtQuick/QQuickItem>
#include <QTimer>
#include <QGraphicsObject>
#include <QToolButton>
#include <QLayout>
#include <qqml.h>
#include <QNetworkAccessManager>
#include <QNetworkDiskCache>
#include <QQmlNetworkAccessManagerFactory>
#include <QLineEdit>
#include <QMenu>
#include <QMenuBar>
#include <QQuickWidget>
#include <QScreen>
#include <QPointer>

// KDE includes
#include <KAuthorized>
#include <KActionCollection>
#include <kdeclarative/kdeclarative.h>
#include <KLocalizedString>
#include <KMessageBox>
#include <KToolBar>
#include <KXMLGUIFactory>
#include <KIO/MetaData>
#include <KHelpMenu>
#include <KAboutData>
#include <KHelpMenu>
#include <KAboutApplicationDialog>
#include <KBugReport>
// #include <KSwitchLanguageDialog>

// DiscoverCommon includes
#include <MuonDataSources.h>
#include <resources/ResourcesModel.h>
#include <resources/UIHelper.h>
#include <Category/Category.h>

#include <cmath>

MuonDiscoverMainWindow::MuonDiscoverMainWindow()
    : QQuickView()
    , m_collection(this)
{
    initialize();

    setObjectName(QStringLiteral("DiscoverMain"));
    setResizeMode(QQuickView::SizeRootObjectToView);
    QQmlEngine* engine = this->engine();
    KDeclarative::KDeclarative kdeclarative;
    kdeclarative.setDeclarativeEngine(engine);
    //binds things like kconfig and icons
    kdeclarative.setupBindings();
    
    qmlRegisterType<PaginateModel>("org.kde.discover.app", 1, 0, "PaginateModel");
    qmlRegisterType<IconColors>("org.kde.discover.app", 1, 0, "IconColors");
    qmlRegisterSingletonType<SystemFonts>("org.kde.discover.app", 1, 0, "SystemFonts", ([](QQmlEngine*, QJSEngine*) -> QObject* { return new SystemFonts; }));
    qmlRegisterType<QQuickView>();
    qmlRegisterType<QActionGroup>();
    qmlRegisterType<QAction>();
    
    //Here we set up a cache for the screenshots
    engine->rootContext()->setContextProperty(QStringLiteral("app"), this);
//
    KConfigGroup window(KSharedConfig::openConfig(), "Window");
    setGeometry(window.readEntry("geometry", QRect()));
    
    setSource(QUrl(QStringLiteral("qrc:/qml/Main.qml")));

    if(!errors().isEmpty()) {
        QString errorsText;

        Q_FOREACH (const QQmlError &error, errors()) {
            errorsText.append(error.toString() + QLatin1String("\n"));
        }
        KMessageBox::detailedSorry(nullptr,
                                   i18n("Found some errors while setting up the GUI, the application can't proceed."),
                                   errorsText, i18n("Initialization error"));
        qDebug() << "errors: " << errors();
        exit(-1);
    }
    Q_ASSERT(errors().isEmpty());

    setupActions();
}

void MuonDiscoverMainWindow::initialize()
{
    ResourcesModel *m = ResourcesModel::global();
    m->integrateActions(actionCollection());
}

MuonDiscoverMainWindow::~MuonDiscoverMainWindow()
{
    KConfigGroup window(KSharedConfig::openConfig(), "Window");
    window.writeEntry("geometry", geometry());
    window.sync();
}

QStringList MuonDiscoverMainWindow::modes() const
{
    QStringList ret;
    QObject* obj = rootObject();
    for(int i = obj->metaObject()->propertyOffset(); i<obj->metaObject()->propertyCount(); i++) {
        QMetaProperty p = obj->metaObject()->property(i);
        QByteArray name = p.name();
        if(name.startsWith("top") && name.endsWith("Comp")) {
            name = name.mid(3);
            name = name.left(name.length()-4);
            name[0] = name[0] - 'A' + 'a';
            ret += QString::fromLatin1(name);
        }
    }
    return ret;
}

void MuonDiscoverMainWindow::openMode(const QByteArray& _mode)
{
    if(!modes().contains(QString::fromLatin1(_mode)))
        qWarning() << "unknown mode" << _mode;
    
    QByteArray mode = _mode;
    if(mode[0]>'Z')
        mode[0] = mode[0]-'a'+'A';
    QObject* obj = rootObject();
    QByteArray propertyName = "top"+mode+"Comp";
    QVariant modeComp = obj->property(propertyName.constData());
    obj->setProperty("currentTopLevel", modeComp);
}

void MuonDiscoverMainWindow::openMimeType(const QString& mime)
{
    emit listMimeInternal(mime);
}

void MuonDiscoverMainWindow::openCategory(const QString& category)
{
    emit listCategoryInternal(category);
}

void MuonDiscoverMainWindow::openApplication(const QString& app)
{
    rootObject()->setProperty("defaultStartup", false);
    m_appToBeOpened = app;
    triggerOpenApplication();
    if(!m_appToBeOpened.isEmpty())
        connect(ResourcesModel::global(), &ResourcesModel::rowsInserted, this, &MuonDiscoverMainWindow::triggerOpenApplication);
}

void MuonDiscoverMainWindow::triggerOpenApplication()
{
    AbstractResource* app = ResourcesModel::global()->resourceByPackageName(m_appToBeOpened);
    if(app) {
        emit openApplicationInternal(app);
        m_appToBeOpened.clear();
        disconnect(ResourcesModel::global(), &ResourcesModel::rowsInserted, this, &MuonDiscoverMainWindow::triggerOpenApplication);
    }
}

QUrl MuonDiscoverMainWindow::featuredSource() const
{
    return MuonDataSources::featuredSource();
}

QUrl MuonDiscoverMainWindow::prioritaryFeaturedSource() const
{
    return QUrl::fromLocalFile(QStandardPaths::locate(QStandardPaths::DataLocation, QStringLiteral("featured.json")));
}

bool MuonDiscoverMainWindow::isCompact() const
{
    if (!isVisible())
        return true;

    const qreal pixelDensity = screen()->physicalDotsPerInch() / 25.4;
    return (width()/pixelDensity)<100; //we'll use compact if the width of the window is less than 7cm
}

qreal MuonDiscoverMainWindow::actualWidth() const
{
    return isCompact() ? width() : width()-std::pow(width()/70., 2);
}

void MuonDiscoverMainWindow::resizeEvent(QResizeEvent * event)
{
    QQuickView::resizeEvent(event);
    Q_EMIT compactChanged(isCompact());
    Q_EMIT actualWidthChanged(actualWidth());
}

void MuonDiscoverMainWindow::showEvent(QShowEvent * event)
{
    QQuickView::showEvent(event);
    Q_EMIT compactChanged(isCompact());
    Q_EMIT actualWidthChanged(actualWidth());
}

void MuonDiscoverMainWindow::setupActions()
{
    QAction *quitAction = KStandardAction::quit(QCoreApplication::instance(), SLOT(quit()), actionCollection());
    actionCollection()->addAction(QStringLiteral("file_quit"), quitAction);

    QAction* configureSourcesAction = new QAction(QIcon::fromTheme(QStringLiteral("repository")), i18n("Configure Sources"), this);
    connect(configureSourcesAction, &QAction::triggered, this, &MuonDiscoverMainWindow::configureSources);
    actionCollection()->addAction(QStringLiteral("configure_sources"), configureSourcesAction);

    if (KAuthorized::authorizeKAction(QStringLiteral("help_contents"))) {
        auto mHandBookAction = KStandardAction::helpContents(this, SLOT(appHelpActivated()), this);
        actionCollection()->addAction(mHandBookAction->objectName(), mHandBookAction);
    }

    if (KAuthorized::authorizeKAction(QStringLiteral("help_report_bug")) && !KAboutData::applicationData().bugAddress().isEmpty()) {
        auto mReportBugAction = KStandardAction::reportBug(this, SLOT(reportBug()), this);
        actionCollection()->addAction(mReportBugAction->objectName(), mReportBugAction);
    }

    if (KAuthorized::authorizeKAction(QStringLiteral("switch_application_language"))) {
        if (KLocalizedString::availableApplicationTranslations().count() > 1) {
            auto mSwitchApplicationLanguageAction = KStandardAction::create(KStandardAction::SwitchApplicationLanguage, this, SLOT(switchApplicationLanguage()), this);
            actionCollection()->addAction(mSwitchApplicationLanguageAction->objectName(), mSwitchApplicationLanguageAction);
        }
    }

    if (KAuthorized::authorizeKAction(QStringLiteral("help_about_app"))) {
        auto mAboutAppAction = KStandardAction::aboutApp(this, SLOT(aboutApplication()), this);
        actionCollection()->addAction(mAboutAppAction->objectName(), mAboutAppAction);
    }
    auto mKeyBindignsAction = KStandardAction::keyBindings(this, SLOT(configureShortcuts()), this);
    actionCollection()->addAction(mKeyBindignsAction->objectName(), mKeyBindignsAction);

    m_moreMenu = new QMenu;
    m_advancedMenu = new QMenu(i18n("Advanced..."), m_moreMenu);
    configureMenu();

    connect(ResourcesModel::global(), &ResourcesModel::allInitialized, this, &MuonDiscoverMainWindow::configureMenu);
}

void MuonDiscoverMainWindow::configureMenu()
{
    m_advancedMenu->clear();
    m_moreMenu->clear();
    UIHelper::setupMessageActions(m_moreMenu, m_advancedMenu, ResourcesModel::global()->messageActions());

    if (!m_moreMenu->isEmpty())
        m_moreMenu->addSeparator();

    m_moreMenu->addAction(actionCollection()->action(QStringLiteral("configure_sources")));
    m_moreMenu->addAction(actionCollection()->action(QStringLiteral("options_configure_keybinding")));
    m_moreMenu->addSeparator();
    m_moreMenu->addMenu(m_advancedMenu);
    m_moreMenu->addSeparator();
    m_moreMenu->addAction(actionCollection()->action(QStringLiteral("help_about_app")));
    m_moreMenu->addAction(actionCollection()->action(QStringLiteral("help_report_bug")));
}

void MuonDiscoverMainWindow::configureSources()
{
    openMode("Sources");
}

void MuonDiscoverMainWindow::closeEvent(QCloseEvent *e)
{
//     QQuickView::closeEvent(e);
    if (!e->isAccepted()) {
        qWarning() << "not closing because there's still pending tasks";
        Q_EMIT preventedClose();
    }
}

bool MuonDiscoverMainWindow::queryClose()
{
    return !ResourcesModel::global()->isBusy();
}

void MuonDiscoverMainWindow::showMenu(int x, int y)
{
    QPoint p = mapToGlobal(QPoint(x, y));
    m_moreMenu->exec(p);
}

void MuonDiscoverMainWindow::appHelpActivated()
{
    QDesktopServices::openUrl(QUrl(QStringLiteral("help:/")));
}

void MuonDiscoverMainWindow::aboutApplication()
{
    static QPointer<QDialog> dialog;
    if (!dialog) {
        dialog = new KAboutApplicationDialog(KAboutData::applicationData(), nullptr);
        dialog->setAttribute(Qt::WA_DeleteOnClose);
    }
    dialog->show();
}

void MuonDiscoverMainWindow::reportBug()
{
    static QPointer<QDialog> dialog;
    if (!dialog) {
        dialog = new KBugReport(KAboutData::applicationData(), nullptr);
        dialog->setAttribute(Qt::WA_DeleteOnClose);
    }
    dialog->show();
}

void MuonDiscoverMainWindow::switchApplicationLanguage()
{
//     auto langDialog = new KSwitchLanguageDialog(nullptr);
//     connect(langDialog, SIGNAL(finished(int)), this, SLOT(dialogFinished()));
//     langDialog->show();
}

void MuonDiscoverMainWindow::configureShortcuts()
{
    const bool letterCutsOk = true;

    KShortcutsDialog dlg(KShortcutsEditor::AllActions,
                         letterCutsOk ? KShortcutsEditor::LetterShortcutsAllowed : KShortcutsEditor::LetterShortcutsDisallowed,
                         qobject_cast<QWidget *>(parent()));
    dlg.setModal(true);
    dlg.addCollection(actionCollection());
    qDebug() << "saving shortcuts..." << dlg.configure(/*bSaveSettings*/);
}
