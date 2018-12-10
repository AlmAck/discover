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

import QtQuick 2.1
import QtQuick.Controls 2.1
import org.kde.discover 2.0
import org.kde.discover.app 1.0
import org.kde.kirigami 2.0 as Kirigami

Kirigami.OverlaySheet {
    id: page
    property alias model: reviewsView.model
    readonly property QtObject reviewsBackend: resource.backend.reviewsBackend
    readonly property var resource: model.resource

    readonly property var rd: ReviewDialog {
        id: reviewDialog
        application: page.resource
        parent: overlay
        backend: page.reviewsBackend
        onAccepted: backend.submitReview(resource, summary, review, rating)
    }

    function openReviewDialog() {
        reviewDialog.sheetOpen = true
        page.sheetOpen = false
    }

    ListView {
        id: reviewsView

        clip: true
        spacing: Kirigami.Units.smallSpacing
        cacheBuffer: Math.max(0, contentHeight)

        header: Item {
            width: parent.width
            height: reviewButton.implicitHeight + 2 * Kirigami.Units.largeSpacing
            Button {
                id: reviewButton

                anchors {
                    verticalCenter: parent.verticalCenter
                    left: parent.left
                    leftMargin: Kirigami.Units.largeSpacing
                }

                visible: page.reviewsBackend != null
                enabled: page.resource.isInstalled
                text: i18n("Review...")
                onClicked: page.openReviewDialog()
            }
        }

        delegate: ReviewDelegate {
            anchors {
                left: parent.left
                right: parent.right
            }
            separator: index != ListView.view.count-1
            onMarkUseful: page.model.markUseful(index, useful)
        }
    }
}
