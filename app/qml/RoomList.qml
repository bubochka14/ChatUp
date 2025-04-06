import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material
import QuickFuture

ListView {
    id: listView

    currentIndex: -1
    boundsBehavior: Flickable.StopAtBounds
    clip: true
    reuseItems: false
    spacing: 1
    delegate: Rectangle {
        id: delegate
        required property int index
        required property string name
        required property int id
        required property int foreignReadings
        required property int localReadings
        required property int messageCount
        required property string lastMessageBody
        required property date lastMessageTime
        required property int lastMessageSender
        property bool hovered: false
        height: 52
        radius: 14
        width: listView.width
        color: hovered || ListView.isCurrentItem ? "#303552" : "transparent"
        onLastMessageSenderChanged: {
            Future.onFinished(UserController.get(delegate.lastMessageSender),
                              user => {
                                  senderLbl.text = user.name + ": "
                              })
        }
        Component.onCompleted: {
            Future.onFinished(UserController.get(lastMessageSender), user => {
                                  senderLbl.text = user.name + ": "
                              })
        }
        RowLayout {
            anchors.fill: parent
            Rectangle {
                Layout.preferredHeight: 40
                Layout.preferredWidth: 40
                Layout.alignment: Qt.AlignLeft
                Layout.leftMargin: 4
                radius: 15
                color: "#19182a"
                Image {
                    id: icon
                    anchors.centerIn: parent
                    source: "pics/users.svg"
                    sourceSize.height: 26
                    sourceSize.width: 26
                }
            }

            Column {
                Layout.fillWidth: true
                Label {
                    id: label
                    Layout.fillWidth: true
                    text: delegate.name
                    font.bold: true
                    elide: Text.ElideRight
                    font.pixelSize: 16
                    width: parent.width
                }
                Row {
                    width: parent.width

                    Label {
                        id: senderLbl
                        elide: Text.ElideRight
                        visible: delegate.lastMessageBody != ""
                        font.pixelSize: 13
                        color: "lightgray"
                    }
                    Label {
                        id: bodyLbl
                        elide: Text.ElideRight
                        visible: delegate.lastMessageBody != ""
                        font.pixelSize: 13
                        color: "lightgray"
                        text: delegate.lastMessageBody
                    }
                }
            }
            Rectangle {
                id: unreadIndicator
                property int messages: delegate.messageCount - delegate.localReadings
                visible: messages > 0
                Layout.alignment: Qt.AlignRight | Qt.AlignVCenter

                Layout.rightMargin: 4

                Label {
                    id: ureadLbl
                    anchors.centerIn: parent
                    text: unreadIndicator.messages > 99 ? "99+" : unreadIndicator.messages
                    color: "white"
                }

                color: "#6482f0"
                height: 20
                width: ureadLbl.contentWidth + 14
                radius: height / 2
            }
        }
        MouseArea {
            hoverEnabled: true
            anchors.fill: parent
            preventStealing: true
            cursorShape: Qt.PointingHandCursor
            onEntered: hovered = true
            onExited: hovered = false
            onClicked: {
                listView.currentIndex = index
            }
        }
    }
}
