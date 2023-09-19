import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs

Pane {
    layer.enabled: true
    function clear() {
        inputBox.text = ""
    }
    id: root
    contentHeight: cloud.height
    property var inputMessage: inputBox.text
    signal messageEntered(string chatMessage)
    Rectangle {
        id: cloud
        color: Material.color(Material.Grey)
        height: inputBox.contentHeight + 30
        anchors.left: parent.left
        anchors.right: sendButton.left
        anchors.rightMargin: 10
        radius: 6
        TextEdit {
            id: inputBox
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 5
            anchors.right: parent.right
            anchors.rightMargin: 5
            Material.containerStyle: Material.containerStyle
            wrapMode: TextEdit.Wrap
        }
        Row {
            id: buttonsRow
            spacing: 5
            anchors.rightMargin: 5
            anchors.right: parent.right
            Image {
                id: emojiBtn
                source: Qt.resolvedUrl("pics/emoji")
                opacity: 0.5
                sourceSize.width: 25
                sourceSize.height: 25

                MouseArea {
                    anchors.fill: parent
                    onClicked: emojiList.popup()
                    cursorShape: Qt.PointingHandCursor
                }
                EmojiList {
                    id: emojiList
                }
            }
            Image{
                id:addFileBtn
                source: Qt.resolvedUrl("pics/fileicon")
                sourceSize.width:25
                sourceSize.height:25
                MouseArea
                {
                    anchors.fill:parent
                    onClicked: fileDialog.open()
                    cursorShape : Qt.PointingHandCursor
                }
                }
            Image {
                id: clearBtn
                enabled: inputBox.text == "" ? 0 : 1
                visible: enabled
                source: Qt.resolvedUrl("pics/clear")
                opacity: 0.5
                width: 25
                height: 25
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
        padding: 0
        anchors.right: parent.right
        anchors.top: parent.top
        height: 50
        width: text.width
        text: qsTr("Send")
        onClicked: {
            messageEntered(inputBox.text)
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
    FileDialog {
        id: fileDialog
        fileMode: FileDialog.OpenFiles
        title: "Please choose a file"
        onAccepted: {
            console.log("You chose: " + selectedFiles)
        }
        onRejected: {
            console.log("Canceled")
        }
        Component.onCompleted: visible = false;
    }
}
