import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material
import ChatClient.Network
import ChatClient.Core
import QuickFuture
import ObjectConverter

ColoredFrame {
    id: root
    //for messages
    property var room
    property string initalMessage: "Select room to start Messaging"
    property var roomViewComponent
    states: [
        State {
            name: "RoomNotSelected"
            PropertyChanges {
                input.visible: false
            }
        },
        State {
            name: "Loading"
            PropertyChanges {
                input.visible: true
                input.sendingEnabled: false
            }
        },
        State {
            name: "Chat"
            PropertyChanges {
                input.visible: true
                input.sendingEnabled: true
            }
        }
    ]
    state: "RoomNotSelected"
    Component.onCompleted: {
        roomViewComponent = Qt.createComponent("RoomView.qml")
        if (roomViewComponent.status !== Component.Ready) {
            console.error("Cannot create MessageView component:",
                          roomViewComponent.errorString())
        }
    }
    onRoomChanged: {
        roomContainer.children[roomContainer.currentIndex].enabled = false
        if (roomContainer.getIndex(room.id) === undefined) {
            state = "Loading"
            var obj = roomViewComponent.createObject(roomContainer, {
                                                         "room": root.room
                                                     })
            roomContainer.currentIndex = roomContainer.children.length - 1
        } else {
            roomContainer.currentIndex = roomContainer.getIndex(room.id)
        }
        root.state = "Chat"
    }
    padding: 0
    bottomPadding: 0
    topPadding: 0
    topInset: 0
    ColumnLayout {
        anchors.fill: parent
        spacing: 0
        StackLayout {
            id: roomContainer
            Layout.fillHeight: true
            Layout.fillWidth: true
            Item {
                Label {
                    id: startMessage
                    anchors.centerIn: parent
                    font.pixelSize: 16
                    text: root.initalMessage
                }
            }
            Item {
                AnimatedImage {
                    id: loadingIcon
                    mipmap: true
                    anchors.centerIn: parent
                    sourceSize.height: 180
                    sourceSize.width: 270
                    source: Qt.resolvedUrl("gif/loading.gif")
                }
            }
            function getIndex(id) {
                for (var i = 2; i < roomContainer.children.length; i++) {
                    if (roomContainer.children[i].room.id == id)
                        return i
                }
                return undefined
            }
            onCurrentIndexChanged: children[currentIndex].enabled = true
        }
        ChatInput {
            id: input
            Layout.fillWidth: true
            focus: true
            //visible: root.room.id != -1
            onMessageEntered: textMessage => {
                                  if (textMessage && root.room.id != -1) {
                                      GroupController.setLocalReadings(
                                          root.room.id, room.messageCount + 1)
                                      GroupController.incrementMessageCount(
                                          root.room.id, 1)
                                      Future.onFinished(
                                          MessageController.create(
                                              textMessage, root.room.id),
                                          function (id) {
                                            let model = MessageController.model(root.room.id)
                                            let index = model.idToModelIndex(id)
                                            GroupController.setLastMessageBody(root.room.id,model.data(index,MessageModel.BodyRole))
                                            GroupController.setLastMessageTime(root.room.id,model.data(index,MessageModel.TimeRole))
                                            GroupController.setLastSender(root.room.id,CurrentUser.id)

                                          })
                                      input.clear()
                                  }
                              }
        }
    }
    ForeignProfileViewer {
        id: foreignProfileViewer
    }
}
