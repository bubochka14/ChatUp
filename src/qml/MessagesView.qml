import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs
import QtQuick.Controls.Material
import ChatClient
import QuickFuture

Item {
    id: root
    required property UserController userController
    property alias listView: listView
    property alias model: listView.model
    property var bottomVisibleMessageIndex
    signal userProfileClicked(var id)
    signal unreadWasRead(var count)
    ListView {
        id: listView
        property var usersCache: ({})
        verticalLayoutDirection: ListView.BottomToTop
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        anchors.left: parent.left
        height: Math.min(contentHeight + 5, parent.height)
        anchors.rightMargin: 15
        reuseItems: true
        spacing: 15
        ScrollBar.vertical: bar
        clip: true
        boundsBehavior: Flickable.StopAtBounds
        delegate: Loader {
            id: delegateLoader
            property int modelIndex: index
            property bool isVisible
            isVisible: {
                listView.contentItem.x + delegateLoader.x >= 0
                        && listView.contentItem.y + delegateLoader.y
                        >= 0 && listView.contentItem.y + delegateLoader.y + delegateLoader.height
                        <= listView.height && listView.contentItem.x
                        + delegateLoader.x + delegateLoader.width <= listView.width
            }
            onIsVisibleChanged: {
                if (messageStatus == MessageModel.Sent
                        && userID != userController.currentUser.id
                        && isVisible
                        && enabled)
                    root.unreadWasRead(messageIndex)
            }
            onEnabledChanged: console.log("ENABLED",enabled)
            ListView.onPooled: if (item)
                                   item.statusIcon.paused = true
            ListView.onReused: {
                if (!item)
                    return
                item.statusIcon.paused = false
                if (listView.usersCache[userID]) {
                    item.user = listView.usersCache[userID]
                    item.currentUser = userController.currentUser.id === item.user.id
                } else
                    Future.onFinished(userController.getUserInfo(userID),
                                      function (user) {
                                          if (user) {
                                              listView.usersCache[userID] = user
                                              item.user = user
                                              item.currentUser = userController.currentUser.id
                                                      === item.user.id
                                          } else
                                              console.error(
                                                          "Cannot hanlde received UserInfo")
                                      })
            }

            width: ListView.view.width
            Component.onCompleted: {
                Future.onFinished(userController.getUserInfo(userID),
                                  function (user) {
                                      if (user) {
                                          listView.usersCache[userID] = user
                                          setSource("MessageDelegate.qml", {
                                                        "currentUser": userController.currentUser.id
                                                                       === user.id,
                                                        "user": user
                                                    })
                                      } else
                                          console.error(
                                                      "Cannot hanlde received UserInfo")
                                  })
            }
            Connections {
                target: item
                function onProfileClicked() {
                    root.userProfileClicked(userID)
                }
            }
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
