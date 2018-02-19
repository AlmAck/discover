/***************************************************************************
 *   Copyright (C) 2013 by Aleix Pol Gonzalez <aleixpol@blue-systems.com>  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA .        *
 ***************************************************************************/

import QtQuick 2.1
import QtQuick.Layouts 1.1
import org.kde.plasma.extras 2.0 as PlasmaExtras
import org.kde.plasma.components 2.0
import org.kde.discovernotifier 1.0

ColumnLayout
{
    PlasmaExtras.Heading {
        Layout.fillWidth: true
        level: 3
        wrapMode: Text.WordWrap
        text: DiscoverNotifier.message
    }

    Label {
        visible: !DiscoverNotifier.isSystemUpToDate
        Layout.fillWidth: true
        Layout.fillHeight: true
        wrapMode: Text.WordWrap
        horizontalAlignment: Text.AlignHCenter
        text: DiscoverNotifier.extendedMessage
    }
    Button {
        visible: !DiscoverNotifier.isSystemUpToDate
        anchors.horizontalCenter: parent.horizontalCenter
        text: i18n("Update")
        tooltip: i18n("Launches the software to perform the update")
        onClicked: DiscoverNotifier.showMuon()
    }
    Item {
        Layout.fillHeight: true
        width: 5
    }
}
