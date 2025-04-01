import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material
import ChatClient.Network
import ChatClient.Core

RowLayout {
    id: root
    required property UserHandle user
    signal profileClicked
    Text{
        text:messageIndex
    }
    Image {
        id: icon
        source: "pics/profile.svg"
        sourceSize.height: 30
        sourceSize.width: 30
        Layout.alignment: Qt.AlignBottom
        Layout.bottomMargin: 16
        MouseArea {
            anchors.fill: parent
            cursorShape: Qt.PointingHandCursor
            onClicked: root.profileClicked()
        }
    }
    Column {
        id: column
        RoundedFrame {
            id: messageCloud
            radius: 15
            Material.elevation: 50
            Material.background: "#19182a"
            width: Math.max(messageBody.contentWidth + 24,
                            messageTime.contentWidth,
                            nameField.contentWidth + 24)
            height: bodyColumn.height + 16
            Column {
                id: bodyColumn
                spacing: 0
                TextEdit {
                    id: nameField
                    color: "gray"
                    text: user.name
                    readOnly: true
                    selectByMouse: true
                    MouseArea {
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        onClicked: root.profileClicked()
                    }
                }

                TextEdit {
                    id: messageBody
                    text: body

                    selectByMouse: true
                    width: root.width * 0.5
                    color: "white"
                    readOnly: true
                    font.pointSize: 10
                    font.hintingPreference: Font.PreferNoHinting
                    wrapMode: TextEdit.Wrap
                }
            }
        }
        TextEdit {
            id: messageTime
            anchors.topMargin: 10
            //anchors.right: parent.right
            selectByMouse: true
            x: messageCloud.x
            color: "white"
            readOnly: true
            text: time.toLocaleString(Qt.locale(Qt.uiLanguage), "MM-dd hh:mm")
            font.pointSize: 8
        }
    }
    Item {
        id: spacer
        Layout.fillWidth: true
    }
}
