import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material
import ChatClient

Dialog {
    id: root
    property UserInfo user
    width:250
    height:400
    modal: true
    RoundedFrame {
        anchors.fill: parent
        ColumnLayout {
            anchors.top: parent.top
            anchors.right: parent.right
            anchors.left: parent.left
            anchors.topMargin: 40
            spacing: 25
            Image {
                id: icon
                Layout.alignment: Qt.AlignHCenter
                source: "pics/profile.svg"
                height: 75
                width: 75
            }
            Label {
                Layout.alignment: Qt.AlignHCenter
                text: root.user.name
            }
            Label {
                Layout.alignment: Qt.AlignHCenter
                text: root.user.id
            }
        }
    }
}
