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
    required property ControllerManager manager
    property var roomID
    property var roomsCache: ({})
    property string initalMessage: "Select room to start Messaging"
    property var roomIndex
    property var roomViewComponent
    states: [
        State {
            name: "RoomNotSelected"
            PropertyChanges {
                roomContainer.currentIndex: 0
                input.visible: false
            }
        },
        State {
            name: "Loading"
            PropertyChanges {
                input.visible: true
                input.sendingEnabled: false
                roomContainer.currentIndex: 1
            }
        },
        State {
            name: "Chat"
            PropertyChanges {
                input.visible: true
                input.sendingEnabled: true
                roomContainer.currentIndex: roomIndex
            }
            StateChangeScript {
                name: "myScript"
                script: readAll()
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

    onRoomIDChanged: {
        if (roomsCache[roomID] === undefined) {
            state = "Loading"
            var history = manager.messageController.model(roomID)
            Future.onFinished(manager.messageController.load(roomID, 0, 0, 10),
                              function () {
                                  var component = Qt.createComponent(
                                              "RoomView.qml")
                                  var obj = roomViewComponent.createObject(
                                              roomContainer, {
                                                  "manager": manager,
                                                  "messageModel": history,
                                                  "roomID": root.roomID,
                                                  "chatBox": root
                                              })
                                  obj.showProfile.connect(showUserProfile)
                                  roomsCache[roomID] = {
                                      "model": history,
                                      "item": obj
                                  }
                                  root.roomIndex = roomContainer.children.length - 1
                                  root.state = "Chat"
                              })
        } else {
            root.roomIndex = roomContainer.getIndex(roomID)
            root.state = "Chat"
        }
    }
    function readAll() {
        var model = roomsCache[roomID].model
        manager.messageController.markAsRead(root.roomID, model.data(
                                                 model.index(0, 0),
                                                 MessageModel.MessageIndexRole))
    }

    function showUserProfile(id) {
        Future.onFinished(manager.userController.get(id), function (user) {
            foreignProfileViewer.showProfle(user)
        })
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
                    if (roomContainer.children[i].roomID == id)
                        return i
                }
                return undefined
            }
        }
        ChatInput {
            id: input
            Layout.fillWidth: true
            focus: true
            visible: root.roomID != -1
            onMessageEntered: textMessage => {
                                  if (textMessage && root.roomID != -1) {
                                      manager.messageController.create(
                                          textMessage, root.roomID)
                                      input.clear()
                                  }
                              }
        }
    }
    ForeignProfileViewer {
        id: foreignProfileViewer
    }
}
