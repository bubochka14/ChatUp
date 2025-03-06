import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material
import ChatClient.Network
import ChatClient.Core
import QuickFuture
import ObjectConverter

Control {
    id: root
    required property ControllerManager manager
    required property var roomID
    property alias topLoaded : view.topLoaded
    required property MessageModel messageModel
    required property ChatBox chatBox
    signal showProfile(var id)
    signal loadingMessagesNeeded()
    padding: 0
    bottomPadding: 0
    topPadding: 0
    topInset: 0
    enabled: root.roomID == chatBox.roomID

    SplitView {
        id: split
        orientation: Qt.Vertical
        anchors.fill: parent
        CallBox {
            id: callBox
            roomID : root.roomID
            manager: root.manager
            SplitView.minimumHeight: callBox.minimumHeight
        }
        Item {
            MessagesView {
                id:view
                model: messageModel
                manager: root.manager
                anchors {
                    fill: parent
                    leftMargin: 8
                    rightMargin: 8
                }
                onLoadingMessagesNeeded: {root.loadingMessagesNeeded()}
                onUserProfileClicked: id => root.showProfile(id)
                onUnreadWasRead: index => manager.messageController.markAsRead(
                                     root.roomID, index)
            }
        }
    }
}
