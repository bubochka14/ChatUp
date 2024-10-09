import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs
ListView {
    id: root
    signal userClicked(var id)
    boundsBehavior: Flickable.StopAtBounds
    clip: true
    delegate: StandardDelegate {
        required property var id
        required property var name
        id: delegate
        width: root.width
        height: 40
        onClicked: root.userClicked(id)
        label.text: name
        icon:    Image {
            id: icon
            source: "pics/profile.svg"
            sourceSize.height: 30
            sourceSize.width: 30
        }
    }
}
