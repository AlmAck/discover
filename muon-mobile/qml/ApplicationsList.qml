import QtQuick 1.1
import org.kde.plasma.components 0.1
import org.kde.qtextracomponents 0.1
import org.kde.muon 1.0
import "navigation.js" as Navigation

Item {
    property QtObject category
    property alias sortRole: apps.sortRole
    property int elemHeight: 40
    property alias stateFilter: apps.stateFilter

    function searchFor(text) {
        apps.search(text)
    }
    
    ListView
    {
        id: view
        anchors {
            top: parent.top
            left: parent.left
            bottom: parent.bottom
            right: scroll.left
        }
        Component {
            id: delegate
            ListItem {
                property real contHeight: elemHeight*0.7
                height: elemHeight
                Row {
                    spacing: 10
                    QIconItem {
                        icon: model["icon"]; width: contHeight; height: contHeight
                        anchors.verticalCenter: parent.verticalCenter
                    }
                    Label { text: name }
                }
                Rating {
                    anchors.right: parent.right
                    rating: model["rating"]
                    height: contHeight*.7
                }
                
                MouseArea {
                    anchors.fill: parent
                    onClicked: Navigation.openApplication(application)
                }
            }
        }
        delegate: delegate
        
        model: ApplicationProxyModel {
            id: apps
            sortRole: 37
            dynamicSortFilter: true
            
            Component.onCompleted: {
                if(category)
                    setFiltersFromCategory(category)
                sortModel(0, 1)
            }
        }
    }
    
    ScrollBar {
        id: scroll
        orientation: Qt.Vertical
        flickableItem: view
        stepSize: 40
        scrollButtonInterval: 50
        anchors {
                top: parent.top
                right: parent.right
                bottom: parent.bottom
        }
    }
}