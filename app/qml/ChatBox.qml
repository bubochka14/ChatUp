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
    property var roomID
    property int messageUploadCount: 50
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
            var history = MessageController.model(roomID)
            var msgCount = GroupController.model.data(
                        GroupController.model.idToIndex(roomID),
                        GroupModel.MessageCountRole)
            console.log("co", msgCount)
            Future.onFinished(MessageController.load(roomID, 0,
                                                     Math.max(0,
                                                              msgCount - messageUploadCount),
                                                     msgCount), function () {
                                                         var component = Qt.createComponent(
                                                                     "RoomView.qml")
                                                         var obj = roomViewComponent.createObject(
                                                                     roomContainer,
                                                                     {
                                                                         "messageModel": history,
                                                                         "roomID": root.roomID,
                                                                         "chatBox": root,
                                                                         "topLoaded": Math.max(0, msgCount - messageUploadCount)
                                                                     })
                                                         obj.showProfile.connect(
                                                                     showUserProfile)
                                                         obj.loadingMessagesNeeded.connect(
                                                                     loadMessages)

                                                         roomsCache[roomID] = {
                                                             "model": history,
                                                             "item": obj
                                                         }
                                                         root.roomIndex
                                                                 = roomContainer.children.length - 1
                                                         root.state = "Chat"
                                                     })
        } else {
            root.roomIndex = roomContainer.getIndex(roomID)
            root.state = "Chat"
        }
    }
    function readAll() {
        var model = roomsCache[roomID].model
       MessageController.markAsRead(root.roomID, model.data(
                                                 model.index(0, 0),
                                                 MessageModel.MessageIndexRole))
    }

    function showUserProfile(id) {
        Future.onFinished(UserController.get(id), function (user) {
            foreignProfileViewer.showProfle(user)
        })
    }
    function loadMessages() {
        var roomView = roomsCache[roomID].item
        console.log("loding", Math.max(0, roomView.topLoaded - messageUploadCount),
                    roomView.topLoaded, roomsCache[roomID].model.rowCount)
        Future.onFinished(MessageController.load(
                              roomID, roomsCache[roomID].model.rowCount,
                              Math.max(0, roomView.topLoaded - messageUploadCount),
                              roomView.topLoaded), function () {
                                  roomView.topLoaded = Math.max(
                                              0, roomView.topLoaded - messageUploadCount)
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
                                      MessageController.create(
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
