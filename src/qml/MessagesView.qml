import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs
import QtQuick.Controls.Material
import ChatClient
import QuickFuture

Item {
    id: root
    required property MessageModel messageModel
    required property AbstractChatController controller //for users
    property alias listView: listView
    ListView {
        id: listView
        property var usersCache: ({})
        verticalLayoutDirection:ListView.BottomToTop
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        anchors.left: parent.left
        height: Math.min(contentHeight+5, parent.height)
        anchors.rightMargin: 15
        reuseItems: true
        model: root.messageModel
        spacing: 15
        ScrollBar.vertical: bar
        clip: true
        boundsBehavior: Flickable.StopAtBounds
        delegate: Loader {
            id: delegateLoader
            property int modelIndex: index
            ListView.onPooled: if(item)
                                   item.statusIcon.paused = true
            ListView.onReused: {
                if(!item)
                    return
                item.statusIcon.paused = false
                if (listView.usersCache[userId]) {
                    item.user = listView.usersCache[userId]
                    item.currentUser = controller.currentUser.id === item.user.id
                } else
                    Future.onFinished(controller.getUserInfo(userId),
                                      function (user) {
                                          if (user) {
                                              ListView.view.usersCache[userId] = user
                                              item.user = user
                                              item.currentUser = controller.currentUser.id === item.user.id

                                          } else
                                              console.error(
                                                          "Cannot hanlde received UserInfo")
                                      })
            }

            width: ListView.view.width
            Component.onCompleted: {
                Future.onFinished(controller.getUserInfo(userId),
                                  function (user) {
                                      if (user) {
                                          ListView.view.usersCache[userId] = user
                                          setSource("MessageDelegate.qml", {
                                                        "currentUser": controller.currentUser.id
                                                                       === user.id,
                                                        "user": user
                                                    })
                                      } else
                                          console.error(
                                                      "Cannot hanlde received UserInfo")
                                  })
            }
        }
    }
    Connections {
        target: root.messageModel
        function onRowsInserted() {
            bar.setPosition(1)
        }
    }
    ScrollBar {
        id: bar
        anchors.right: parent.right
        height: parent.height
        width: 5
        active: true
    }
}
