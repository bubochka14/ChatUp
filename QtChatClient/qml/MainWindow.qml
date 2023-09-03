import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Page {
    id: root
    anchors.fill: parent
    signal languageSet(string language)
    signal messageSent(string message, int roomID)
    header: MenuBar {    
       Menu {
            title: qsTr("&Settings")
            Menu {
                id: languageMenu
                title: qsTr("&Language")
                Action {
                    text: "English"
                }
                Action {
                    text: "Russian"
                }
            }
        }
    }
    RowLayout {
        id: mainLayout
        anchors.fill: parent
        RoomList {
            id: roomList
            Layout.minimumWidth: 220
            Layout.preferredWidth: root.width / 4
            Layout.fillHeight: true
        }
        Item {
            id: chatColumn
            Layout.fillHeight: true
            Layout.fillWidth: true
            Rectangle {
                id: chatHistoryBox
                anchors.fill: parent
                color: "green"
            }
            ChatInput {
                id: inputBox
                enabled: roomList.currentRoomID!=-1
                visible: roomList.currentRoomID!=-1
                onMessageEntered: (textMessage) => {
                    if (textMessage && roomList.currentRoomID && roomList.currentRoomID != -1) {
                        root.messageSent(textMessage, roomList.currentRoomID);
                        console.log("Send message: " + textMessage + " to " + roomList.currentRoomID);
                    }
                }
                anchors.bottom: chatColumn.bottom
                anchors.right: chatColumn.right
                anchors.left: chatColumn.left
            }
        }
    }
}
