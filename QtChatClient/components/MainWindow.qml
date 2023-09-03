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
        Menu {
            title: qsTr("&Room")
            Action {
                text: "v razrabodke))"
            }
        }
        Menu {
            title: qsTr("&Account")
            Action {
                text: "da."
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
            onSelectedRoomIndexChanged: {
                console.log("selected changed" + selectedRoomIndex);
                chatBox.model = roomModel.getRoomHistory(selectedRoomIndex);
            }
        }
        ColumnLayout {
            id: chatColumn
            Layout.fillHeight: true
            Layout.fillWidth: true

            ChatBox {
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    id: chatBox
                    width:parent.width
             }
            
            ChatInput {
                id: inputBox
                Layout.fillWidth: true
                enabled: roomList.selectedRoomID != -1
                visible: roomList.selectedRoomID != -1
                onMessageEntered: textMessage => {
                    if (textMessage && roomList.selectedRoomID && roomList.selectedRoomID != -1) {
                        root.messageSent(textMessage, roomList.selectedRoomID);
                        console.log("Send message: " + textMessage + " to " + roomList.selectedRoomID);
                    }
                }
            }
        }
    }
}
