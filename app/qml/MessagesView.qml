import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs
import QtQuick.Controls.Material
import ChatClient.Network
import ChatClient.Core
import QuickFuture
import QtQml
ListView {
    id: root
    required property var room
    property int topLoaded: 0
    property var bottomVisibleMessageIndex
    property int uploadCount: 50
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
    interactive: true
    synchronousDrag: true
    flickDeceleration: 10000
    boundsBehavior: Flickable.StopAtBounds
    WheelHandler {
        id: root_item

        property int speed: 5
        property var flickable: root

        onWheel: event => {
                     let scroll_flick = event.angleDelta.y * speed * 2.5

                     if (flickable.verticalOvershoot != 0.0
                         || (scroll_flick > 0
                             && (flickable.verticalVelocity <= 0))
                         || (scroll_flick < 0
                             && (flickable.verticalVelocity >= 0))) {
                         flickable.flick(0, (scroll_flick))
                         return
                     } else {
                         flickable.cancelFlick()
                         return
                     }
                 }
    }
    delegate: Loader {
        id: delegateLoader
        required property int userID
        required property int messageIndex
        required property string body
        required property var time
        property bool isVisible
        isVisible: {
            root.contentItem.y + delegateLoader.y >= 0
                    && root.contentItem.y + delegateLoader.y + delegateLoader.height <= root.height
        }
        onEnabledChanged: {
            if (isVisible && enabled) {
                if (room.localReadings <= messageIndex
                        && userID != CurrentUser.id)
                    root.readMessages(messageIndex + 1)
            }
        }

        onIsVisibleChanged: {
            if (isVisible && enabled) {
                if (room.localReadings <= messageIndex
                        && userID != CurrentUser.id)
                    root.readMessages(messageIndex + 1)
            }
        }
        Component.onCompleted: {
            if (isVisible && enabled) {
                if (room.localReadings <= messageIndex
                        && userID != CurrentUser.id) {

                    root.readMessages(messageIndex + 1)
                }
            }
            sync()
        }
        ListView.onReused: {
            if (messageIndex == topLoaded && topLoaded != 0)
                root.loadMessages()
            sync()
        }
        width: ListView.view.width-6
        function sync() {
            if (delegateLoader.userID === CurrentUser.id) {
                setSource("MessageDelegate.qml", {foreignReadings: Qt.binding(function() { return root.room.foreignReadings})})
                return
            }
            Future.onFinished(UserController.get(delegateLoader.userID),
                              function (user) {
                                  if (!delegateLoader)
                                      return
                                  if (user) {
                                      setSource("ForeignMessageDelegate.qml", {
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
        width: root.width
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
        width: root.width
    }

    ScrollBar {
        id: bar
        stepSize: 100
        anchors.right: parent.right
        height: parent.height
        width: 5
        active: true
    }
    function readMessages(count) {
        if (count < room.localReadings)
            console.log("reading messages", count)
        GroupController.setLocalReadings(room.id, count)
    }
    Timer
    {
        id:readingsTimer
        interval:500
        onTriggered: time.text = Date().toString()

    }

    function loadMessages() {
        if(!room.messageCount)
            return
        console.log("loading messages", Math.max(0, topLoaded - uploadCount),
                    topLoaded, model.rowCount)
        Future.onFinished(MessageController.load(
                              room.id, model.rowCount,
                              Math.max(0, topLoaded - uploadCount), topLoaded),
                          function () {
                              topLoaded = Math.max(0, topLoaded - uploadCount)
                          })
    }
    Component.onCompleted: {
        topLoaded = room.messageCount
        model = MessageController.model(room.id)
        loadMessages()
    }
}
