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
    property int topLoaded: 0
    property alias listView: listView
    property alias model: listView.model
    property var bottomVisibleMessageIndex
    signal userProfileClicked(var id)
    signal unreadWasRead(var count)
    signal loadingMessagesNeeded()
    ListView {
        id: listView
        property var usersCache: ({})
        verticalLayoutDirection: ListView.BottomToTop
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        anchors.left: parent.left
        height: parent.height
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
                if (isVisible && enabled) {
                    if (messageStatus == MessageModel.Sent
                            && userID != CurrentUser.id)
                        root.unreadWasRead(messageIndex)
                    if(messageIndex == topLoaded && topLoaded !=0)
                        root.loadingMessagesNeeded()
                }
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
        footer: Item {
            height: 35
            width: listView.width
            AnimatedImage {
                id: loadingIcon
                anchors.centerIn: parent
                visible: topLoaded != 0
                mipmap: true
                sourceSize.height: 72
                sourceSize.width: 108
                source: Qt.resolvedUrl("gif/loading.gif")
            }
        }
        header: Item {
            height: 10
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
