import QtQuick 1.1
import org.kde.plasma.components 0.1
import org.kde.qtextracomponents 0.1
import org.kde.muon 1.0
import "navigation.js" as Navigation

ListView {
    id: view
    property alias sortRole: appsModel.stringSortRole
    property alias filteredCategory: appsModel.filteredCategory
    property Component roleDelegate: null
    
    function sortModel() { appsModel.sortModel() }
    
    interactive: false
    model: ApplicationProxyModel {
        id: appsModel
        sortOrder: Qt.DescendingOrder
    }
    delegate: ListItem {
                width: view.width
                height: 30
                enabled: true
                QIconItem {
                    id: iconItem
                    anchors { left: parent.left; verticalCenter: parent.verticalCenter }
                    height: parent.height*0.9
                    width: height
                    icon: model["icon"]
                }
                Label {
                    anchors { left: iconItem.right; right: pointsLabel.left; verticalCenter: parent.verticalCenter }
                    text: name
                    elide: Text.ElideRight
                }
                Loader {
                    anchors { right: parent.right; verticalCenter: parent.verticalCenter }
                    id: pointsLabel
                    sourceComponent: view.roleDelegate
                    onItemChanged: item.model=model
                }
                onClicked: Navigation.openApplication(application)
            }
}