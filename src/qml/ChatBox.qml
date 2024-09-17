import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material
import ChatClient
import QuickFuture

ColoredFrame {
    id: root
    //for messages
    required property AbstractChatController controller
    property string initalMessage
    property int roomId: -1
    function setRoomId(id) {
        if (_private.history[id] === undefined) {
            container.currentIndex = 1
            input.enabled =false
            Future.onFinished(controller.getRoomHistory(id), function (value) {
                var component = Qt.createComponent("MessagesView.qml")
                if (component.status === Component.Ready) {
                    var obj = component.createObject(container, {
                                                         "messageModel": value,
                                                         "controller": controller
                                                     })
                    _private.history[id] = container.children.length-1
                    container.currentIndex = _private.history[id]
                } else
                    console.error("Cannot create MessageView component:",component.errorString())
            })
        } else {
            container.currentIndex = _private.history[id]
        }
        root.roomId = id
        input.enabled =true
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
            Item{
                AnimatedImage
                {
                    id:loadingIcon
                    mipmap:true
                    anchors.centerIn: parent
                    sourceSize.height: 180
                    sourceSize.width:270
                    source: Qt.resolvedUrl("gif/loading.gif")
                }
            }
        }
        ChatInput {
            id: input
            Layout.fillWidth: true
            Layout.margins: 0
            focus: true
            visible: root.roomId != -1
            onMessageEntered: textMessage => {
                                  if (textMessage && root.roomId != -1) {
                                      controller.createMessage(textMessage,
                                                               root.roomId)
                                      input.clear()
                                  }
                              }
        }
    }
}
