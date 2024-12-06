import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material
import ChatClient.Network
import ChatClient.Core
import QuickFuture
import ObjectConverter

Frame {
    id: root
    required property ControllerManager manager
    required property var roomID
    required property MessageModel messageModel
    required property ChatBox chatBox
    signal showProfile(var id)
    padding: 0
    bottomPadding: 0
    topPadding: 0
    topInset: 0
    enabled: root.roomID == chatBox.roomID

    SplitView {
        id:split
        orientation: Qt.Vertical
        anchors.fill: parent
        CallBox {
            id: callBox
            manager: root.manager
            SplitView.minimumHeight: callBox.minimumHeight
        }
        Item{
        MessagesView {

            model: messageModel
            manager: root.manager
            anchors{
                fill:parent
                leftMargin:8
                rightMargin:8
            }

            onUserProfileClicked: id => root.showProfile(id)
            onUnreadWasRead: index => manager.messageController.markAsRead(
                                 root.roomID, index)
        }
        }
    }
}
