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
    property int topLoaded: 0
    property alias listView: listView
    property alias model: listView.model
    property var bottomVisibleMessageIndex
    signal userProfileClicked(var id)
    signal unreadWasRead(var count)
    signal loadingMessagesNeeded
    ListView {
        id: listView
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
        synchronousDrag : true
        flickDeceleration:10000
        boundsBehavior: Flickable.StopAtBounds
        WheelHandler{
            id: root_item

            property int speed: 5
            property var flickable: listView

            onWheel: (event) => {

                let scroll_flick = event.angleDelta.y * speed*2.5;

                if(flickable.verticalOvershoot != 0.0 ||
                  (scroll_flick>0 && (flickable.verticalVelocity<=0)) ||
                  (scroll_flick<0 && (flickable.verticalVelocity>=0)))
                {
                    flickable.flick(0, (scroll_flick));
                    return;
                }
                else
                {
                    flickable.cancelFlick();
                    return;
                }
            }
        }
        delegate: Loader {
            id: delegateLoader
            required property int userID
            required property int messageStatus
            required property int messageIndex
            required property string body
            required property var time
            property bool isVisible
            isVisible: {
                listView.contentItem.y + delegateLoader.y >= 0
                        // && listView.contentItem.y + delegateLoader.y
                        // + delegateLoader.height <= listView.height
            }
            onIsVisibleChanged: {
                if (isVisible && enabled) {
                    if (messageStatus == MessageModel.Sent
                            && userID != CurrentUser.id)
                        root.unreadWasRead(messageIndex)
                    // if (messageIndex == topLoaded && topLoaded != 0)
                    //     root.loadingMessagesNeeded()
                }
            }

            ListView.onReused: {
                if (messageIndex == topLoaded && topLoaded != 0)
                    root.loadingMessagesNeeded()
                sync()
            }
            width: ListView.view.width
            Component.onCompleted: {
                sync()
            }
            Connections {
                target: item
                function onProfileClicked() {
                    root.userProfileClicked(delegateLoader.userID)
                }
            }
            function sync() {
                if (delegateLoader.userID === CurrentUser.id) {
                    setSource("MessageDelegate.qml")
                    return
                }
                Future.onFinished(UserController.get(delegateLoader.userID),
                                  function (user) {
                                      if(!delegateLoader)
                                          return
                                      if (user) {
                                          setSource("ForeignMessageDelegate.qml",
                                                    {
                                                        "user": user
                                                    })
                                      } else
                                          console.error(
                                                      "Cannot hanlde received UserInfo")
                                  })
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
        stepSize:100
        anchors.right: parent.right
        height: parent.height
        width: 5
        active: true
    }

}
