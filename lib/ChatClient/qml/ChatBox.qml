import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material
import ChatClient

Pane {
    property MessageModel model
    required property AbstractChatController controller
    Material.background: "#39423B"
    id: root

    function showStartMessage(st) {
        startMessage.visible = st
    }

    Label {
        Material.foreground: Material.Grey
        id: startMessage
        anchors.centerIn: parent
        font.pixelSize: 16
        text: qsTr("Select ChatRoom to start messaging")
    }

    ListView {
        anchors.fill: parent
        id: listView
        model: root.model
        spacing: 15
        ScrollBar.vertical: ScrollBar {
            id: bar
            active: true
        }
        onCountChanged: {
            bar.setPosition(1)
        }
        reuseItems: true
        delegate: Loader {
            id: delegateLoader
            width: listView.width * 0.8
            Component.onCompleted: {
                Future.onFinished(controller.getUserInfo(userId),
                                  function (user) {
                                      if(user)
                                      delegateLoader.setSource(
                                                  "MessageDelegate.qml", {
                                                      "user": user,
                                                      "currentUser":controller.currentUser.id === user.id
                                                  })
                                      else
                                          console.log("Cannot hanlde received UserInfo")
                                  })
            }
        }
    }
}
