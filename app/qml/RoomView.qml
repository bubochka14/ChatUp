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
    required property var room
    padding: 0
    bottomPadding: 0
    topPadding: 0
    topInset: 0
    MessagesView {
        id: view
        room: root.room
        anchors {
            fill: parent
            leftMargin: 8
            rightMargin: 8
            topMargin: 4
        }
        //onUnreadWasRead: index => GroupController.setLocalReadings(
        //                    root.roomID, index)
    }
    SplitView {
        id: split
        orientation: Qt.Vertical
        anchors.fill: parent
        CallBox {
            id: callBox
            roomID: root.room.id
            SplitView.minimumHeight: callBox.minimumHeight
        }Item
        {
            SplitView.fillHeight:true
        }
    }
}
