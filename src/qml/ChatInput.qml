import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs
import QtQuick.Controls.Material

ColoredFrame {
    id: root
    property alias inputMessage: inputBox.text
    property int maximumHeight: 400
    signal messageEntered(string chatMessage)
    implicitHeight: layout.height + 30
    function clear() {
        inputBox.text = ""
    }

    RowLayout {
        id: layout
        anchors.right: parent.right
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.topMargin: 5
        Image {
            id: addFileBtn
            source: Qt.resolvedUrl("pics/paper")
            Layout.alignment: Qt.AlignBottom
            Layout.bottomMargin: 5
            sourceSize.width: 25
            sourceSize.height: 25
            MouseArea {
                anchors.fill: parent
                cursorShape: Qt.PointingHandCursor
            }
        }
        Rectangle {
            id: cloud
            color: "#19182a"
            Layout.preferredHeight: inputBox.contentHeight + 30
            Layout.alignment: Qt.AlignBottom
            Layout.maximumHeight: root.maximumHeight
            Layout.fillWidth: true
            radius: 15
            TextEdit {
                id: inputBox
                focus: true
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: parent.left
                anchors.leftMargin: 5
                anchors.right: parent.right
                anchors.rightMargin: 5
                color: "white"
                Material.containerStyle: Material.containerStyle
                wrapMode: TextEdit.Wrap
                Keys.onReturnPressed: event => {
                                          if (event.modifiers & Qt.ControlModifier)
                                          inputBox.append("\n")
                                          else
                                          sendButton.clicked()
                                      }

                Keys.onEnterPressed: event => {
                                         sendButton.clicked()
                                     }
            }
            Row {
                id: buttonsRow
                spacing: 5
                anchors.rightMargin: 5
                anchors.right: parent.right
                Image {
                    id: emojiBtn
                    source: Qt.resolvedUrl("pics/emoji")
                    sourceSize.width: 25
                    sourceSize.height: 30

                    MouseArea {
                        anchors.fill: parent
                        onClicked: emojiList.popup()
                        cursorShape: Qt.PointingHandCursor
                    }
                    EmojiList {
                        id: emojiList
                    }
                }
                Label {
                    id: clearBtn
                    enabled: inputBox.text == "" ? 0 : 1
                    visible: enabled
                    font.pointSize: 12
                    text: "🗙"
                    MouseArea {
                        anchors.fill: parent
                        onClicked: root.clear()
                        cursorShape: Qt.PointingHandCursor
                    }
                }
            }
        }

        Button {
            id: sendButton
            implicitWidth: 45
            implicitHeight: 45
            Layout.alignment: Qt.AlignBottom
            icon.source: Qt.resolvedUrl("pics/send")
            background: Rectangle {
                anchors.fill: parent
                gradient: Gradient {
                    GradientStop {
                        position: 0
                        color: "#6058fb"
                    }
                    GradientStop {
                        position: 1
                        color: "#3193ec"
                    }
                }
                opacity: enabled ? 1 : 0.3
                border.width: 1
                radius: 15
            }
            onClicked: {
                messageEntered(inputBox.text)
            }
        }
    }
    DropArea {
        id: dropArea
        anchors.fill: parent
        onEntered: drag => {
                       cloud.color = "blue"
                       drag.accept(Qt.LinkAction)
                   }
        onDropped: drop => {
                       console.log(drop.urls)
                       cloud.color = "gray"
                   }
        onExited: {
            cloud.color = "gray"
        }
    }
}
