import org.kde.plasma.components 0.1
import org.kde.qtextracomponents 0.1

ToolButton
{
    property alias icon: iconItem.icon
    property alias text: labelItem.text
    width: height+(labelItem.text=="" ? 0 : labelItem.width)
    anchors.margins: 5
    
    QIconItem {
        id: iconItem
        anchors {
            top: parent.top
            bottom: parent.bottom
            left: parent.left
            margins: 5
        }
        width: height
    }
    
    Label {
        id: labelItem
        anchors {
            top: parent.top
            bottom: parent.bottom
            right: parent.right
            margins: 5
        }
    }
}