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
                    onTriggered: {
                    root.languageSet("en");
                    }
                }
                Action {
                    text: "Russian"
                    onTriggered: {
                    root.languageSet("ru");
                    }

                }
            }
        }
        Menu {
            title: qsTr("&Room")
 
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
        orientation: Qt.Horizontal
        anchors.fill: parent

        RoomList {
            id: roomList
            SplitView.minimumWidth: 120
            SplitView.preferredWidth: Math.max(220,parent.width/4)
            SplitView.maximumWidth: 420
            onSelectedRoomIndexChanged: {
                chatBox.model = roomModel.getRoomHistory(selectedRoomIndex)
                chatBox.showStartMessage(false)
            }
        }
        ColumnLayout {
            id: chatColumn
            spacing:0
            SplitView.fillWidth: true
            SplitView.minimumWidth: 350
            ChatBox {
                id: chatBox
                Layout.fillHeight: true
                Layout.fillWidth: true

            }

            ChatInput {
                id: inputBox
                Layout.fillWidth: true

                visible: roomList.selectedRoomIndex == -1 ? false : true
                onMessageEntered: textMessage => {
                                      if (textMessage && roomList.selectedRoomID
                                          && roomList.selectedRoomID != -1) {
                                          root.messageSent(
                                              textMessage,
                                              roomList.selectedRoomID)
                                          inputBox.clear()
                                          console.log(
                                              "Send message: " + textMessage
                                              + " to " + roomList.selectedRoomID)
                                      }
                                  }
            }
        }
        UserList
        {
            Layout.fillHeight: true
            visible: roomList.selectedRoomIndex == -1 ? false : true

            id: userList
            SplitView.minimumWidth: 120
            SplitView.maximumWidth: 350
        }
    }
}
