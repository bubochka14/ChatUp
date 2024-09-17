import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import ChatClient

Item {
    id: root
    required property AbstractChatController controller
    property bool roomSelected: false
    RowLayout {
        id: mainLayout
        spacing: 0
        anchors.fill: parent
        RoomList {
            id: roomList
            // Layout.minimumWidth: 120
            Layout.fillHeight: true
            Layout.preferredWidth: Math.max(220, parent.width / 4)
            Layout.maximumWidth: 420
            onSelectedRoomIDChanged: {

            }
        }
        ColumnLayout {
            id: chatColumn
            spacing: 0
            Layout.fillWidth: true
            // Layout.minimumWidth: 350
            ColoredFrame {
                id: roomHeader
                property alias title: titleLabel.text
                visible: roomSelected
                implicitHeight: 50
                Layout.fillWidth: true
                Label {
                    anchors.centerIn: parent
                    font.bold: true
                    id: titleLabel
                }
            }

            ChatBox {
                id: chatBox
                focus: true
                controller: root.controller
                Layout.fillHeight: true
                Layout.fillWidth: true
            }
        }
    }
}
