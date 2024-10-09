import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material
import ChatClient
import QuickFuture
import ObjectConverter

ColoredFrame {
    id: root
    //for messages
    required property MessageController messageController
    required property UserController userController
    property string initalMessage
    property int roomID: -1
    function setRoomId(id) {
        if (_private.history[id] === undefined) {
            container.currentIndex = 1
            input.enabled = false
            Future.onFinished(messageController.getRoomHistory(id),
                              function (history) {
                                  var component = Qt.createComponent(
                                              "MessagesView.qml")
                                  if (component.status === Component.Ready) {
                                      var obj = component.createObject(
                                                  container, {
                                                      "userController": userController,
                                                      "model": history
                                                  })
                                      if(_private.history[roomID]!== undefined)
                                          _private.history[root.roomID].item.enabled = false
                                      root.roomID = id
                                      obj.userProfileClicked.connect(
                                                  showUserProfile)
                                      obj.unreadWasRead.connect(markAsRead)
                                      _private.history[id] = {
                                          "containerIndex": container.children.length - 1,
                                          "model": history,
                                          "item": obj
                                      }
                                      markAsRead(history.data(
                                                     history.index(0, 0),
                                                     MessageModel.MessageIndexRole))
                                      container.currentIndex = _private.history[id].containerIndex
                                  } else
                                      console.error(
                                                  "Cannot create MessageView component:",
                                                  component.errorString())
                              })
        } else {

            _private.history[root.roomID].item.enabled = false
            root.roomID = id
            _private.history[root.roomID].item.enabled = true
            container.currentIndex = _private.history[id].containerIndex
            var model = _private.history[id].model
            markAsRead(model.data(model.index(0, 0),
                                  MessageModel.MessageIndexRole))
        }
        input.enabled = true
    }
    function showUserProfile(id) {
        Future.onFinished(userController.getUserInfo(id), function (user) {

            foreignProfileViewer.showProfle(user)
        })
    }
    function markAsRead(messageIndex) {
        messageController.markAsRead(root.roomID, messageIndex)
    }

    Item {
        id: _private
        property var history: ({})
    }
    padding: 0
    bottomPadding: 0
    ColumnLayout {
        anchors.fill: parent
        spacing: 0
        StackLayout {
            id: container
            Layout.margins: 10
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
        }
        ChatInput {
            id: input
            Layout.fillWidth: true
            Layout.margins: 0
            focus: true
            visible: root.roomID != -1
            onMessageEntered: textMessage => {
                                  if (textMessage && root.roomID != -1) {
                                      root.messageController.createMessage(
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
