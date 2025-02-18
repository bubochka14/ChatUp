import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs
import QtQuick.Controls.Material
import ChatClient.Network
import ChatClient.Core
import QuickFuture

Item {
    id: root
    required property ControllerManager manager
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
        height:  parent.height
        anchors.rightMargin: 15
        spacing: 10
        reuseItems: true
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
                        && userID != CurrentUser.id
                        && isVisible
                        && enabled)
                    root.unreadWasRead(messageIndex)
            }
            ListView.onPooled: if (item)
                                   item.statusIcon.paused = true
            ListView.onReused: {
                if (!item)
                    return
                item.statusIcon.paused = false
                if (listView.usersCache[userID]) {
                    item.user = listView.usersCache[userID]
                } else
                    Future.onFinished(manager.userController.get(userID),
                                      function (user) {
                                          if (user) {
                                              listView.usersCache[userID] = user
                                              item.user = user
                                          } else
                                              console.error(
                                                          "Cannot hanlde received UserInfo")
                                      })
            }

            width: ListView.view.width
            Component.onCompleted: {
                Future.onFinished(manager.userController.get(userID),
                                  function (user) {
                                      if (user) {
                                          listView.usersCache[userID] = user
                                          setSource("MessageDelegate.qml", {
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
        header:Item{
            height:10
            width: listView.width
        }
        footer:Item{
            height:10
            width: listView.width
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
