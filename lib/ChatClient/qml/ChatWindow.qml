import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import ChatClient

ApplicationWindow {
    id: root
    width: 400
    height: 600
    visible: true
    signal languageSet(string language)
    signal messageSent(string message, int roomID)
    required property AbstractChatController controller
    header: MenuBar {
        Menu {
            title: qsTr("&Settings")
            Menu {
                id: languageMenu
                title: qsTr("&Language")
                Action {
                    text: "English"
                    onTriggered: {
                        root.languageSet("en")
                    }
                }
                Action {
                    text: "Russian"
                    onTriggered: {
                        root.languageSet("ru")
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
    SplitView {
        id: mainLayout
        anchors.fill: parent

        RoomList {
            id: roomList
            SplitView.minimumWidth: 120
            // Layout.preferredWidth: Math.max(220,parent.width/4)
            SplitView.maximumWidth: 420
            roomModel: controller.userRooms
            onSelectedRoomIndexChanged: {
                Future.onFinished(controller.getRoomHistory(selectedRoomID),
                                  function (value) {
                                      chatBox.model = value
                                  })
                chatBox.showStartMessage(false)
                chatBox.model
            }
        }
        ColumnLayout {
            id: chatColumn
            spacing: 0
            SplitView.fillWidth: true
            SplitView.minimumWidth: 350
            ChatBox {
                id: chatBox
                controller: root.controller
                Layout.fillHeight: true
                Layout.fillWidth: true
            }

            ChatInput {
                id: inputBox
                Layout.fillWidth: true

                visible: roomList.selectedRoomIndex == -1 ? false : true
                onMessageEntered: textMessage => {
                                      if (textMessage
                                          && roomList.selectedRoomID != -1) {
                                          controller.createMessage(
                                              textMessage,
                                              roomList.selectedRoomID)
                                          inputBox.clear()
                                      }
                                  }
            }
        }
    }
}
