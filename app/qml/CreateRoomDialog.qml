import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material
import ChatClient.Network
import ChatClient.Core

Dialog {
    id: root
    property alias roomName :roomNameField.text
    width: 350
    height: 200
    modal: true
    title: qsTr("Room Creation")
    standardButtons: Dialog.Ok | Dialog.Cancel
    RoundedFrame {
        anchors.fill: parent
        Row {
            anchors.fill: parent
            spacing: 30
            Rectangle {
                height:70
                width:70
                radius: 40
                color:"#19182a"
                anchors.verticalCenter: parent.verticalCenter
                scale: !mouseArea.containsMouse?1:1.2
                Image {
                    id: icon
                    anchors.centerIn: parent
                    source: "pics/photo.svg"
                    sourceSize.height: 50
                    sourceSize.width: 50
                }
                MouseArea
                {
                    id: mouseArea
                    hoverEnabled: true
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                }
            }
            TextField {
                id: roomNameField
                width: 125
                anchors.verticalCenter: parent.verticalCenter
                placeholderText: qsTr("Room name:")
            }
        }
    }
}
