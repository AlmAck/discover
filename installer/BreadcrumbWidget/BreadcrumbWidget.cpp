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

#include "BreadcrumbWidget.h"

#include <QtGui/QPushButton>

#include <KStandardGuiItem>

#include "../AbstractViewBase.h"
#include "BreadcrumbItem.h"

BreadcrumbWidget::BreadcrumbWidget(QWidget *parent)
    : KHBox(parent)
{
    setSpacing(2);
    KGuiItem backButton = KStandardGuiItem::back();
    KGuiItem forwardButton = KStandardGuiItem::forward();

    m_backButton = new QPushButton(this);
    m_backButton->setToolTip(backButton.toolTip());
    m_backButton->setIcon(backButton.icon());
    m_backButton->setEnabled(false);

    m_forwardButton = new QPushButton(this);
    m_forwardButton->setToolTip(forwardButton.toolTip());
    m_forwardButton->setIcon(forwardButton.icon());
    m_forwardButton->setEnabled(false);

    m_breadcrumbArea = new KHBox(this);
    m_breadcrumbArea->setSpacing(2);

    QWidget *spacer = new QWidget(this);
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

    connect(m_backButton, SIGNAL(clicked()), this, SLOT(goBack()));
    connect(m_forwardButton, SIGNAL(clicked()), this, SLOT(goForward()));
}

BreadcrumbWidget::~BreadcrumbWidget()
{
}

void BreadcrumbWidget::setRootItem(BreadcrumbItem *root)
{
    clearCrumbs();
    addLevel(root);

    m_forwardButton->setEnabled(false);
    m_backButton->setEnabled(false);
}

void BreadcrumbWidget::clearCrumbs()
{
    if (!m_items.isEmpty()) {
      foreach(BreadcrumbItem *item, m_items) {
          item->hide();
          item->deleteLater();
      }
    }

    m_currentItem = 0;
    m_items.clear();
}

void BreadcrumbWidget::addLevel(BreadcrumbItem *item)
{
    // If we are activating a new subView from a view that already has
    // children, the old ones must go
    if (m_currentItem && m_currentItem->hasChildren()) {
        removeItem(m_currentItem->childItem());
    }

    if (!m_items.isEmpty()) {
        m_items.last()->setChildItem(item);
    }

    item->setParent(m_breadcrumbArea);
    item->show();

    m_items.append(item);
    setCurrentItem(item);

    m_forwardButton->setEnabled(false);
    m_backButton->setEnabled(true);

    connect(item, SIGNAL(activated(BreadcrumbItem *)), this, SLOT(onItemActivated(BreadcrumbItem *)));
    connect(item, SIGNAL(activated(BreadcrumbItem *)), this, SLOT(setCurrentItem(BreadcrumbItem *)));
}

void BreadcrumbWidget::removeItem(BreadcrumbItem *item)
{
    // Recursion ftw
    if (item->hasChildren()){
        removeItem(item->childItem());
    }

    item->hide();
    item->associatedView()->deleteLater();
    item->deleteLater();
    m_items.removeLast();
}

void BreadcrumbWidget::goForward()
{
    setCurrentItem(m_items.at(m_items.indexOf(m_currentItem) + 1));
    onItemActivated(m_currentItem);
}

void BreadcrumbWidget::goBack()
{
    setCurrentItem(m_items.at(m_items.indexOf(m_currentItem) - 1));
    onItemActivated(m_currentItem);
}

void BreadcrumbWidget::setCurrentItem(BreadcrumbItem *itemToBold)
{
    foreach(BreadcrumbItem *item, m_items) {
        item->setActive(item == itemToBold);
    }

    m_currentItem = itemToBold;
}

void BreadcrumbWidget::onItemActivated(BreadcrumbItem *item)
{
    m_backButton->setEnabled(m_items.indexOf(item) > 0);
    m_forwardButton->setEnabled(item != m_items.last());

    emit itemActivated(item);
}

BreadcrumbItem *BreadcrumbWidget::breadcrumbForView(AbstractViewBase *view)
{
    BreadcrumbItem *itemForView = 0;

    foreach (BreadcrumbItem *item, m_items) {
         if (item->associatedView() == view) {
             itemForView = item;
         }
    }

    return itemForView;
}

#include "BreadcrumbWidget.moc"
