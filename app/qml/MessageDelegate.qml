import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material
import ChatClient.Network
import ChatClient.Core

RowLayout {
    id: root
    property alias statusIcon: statusIcon
    required property int foreignReadings
    layoutDirection: Qt.RightToLeft
    Column {
        id: column

        ColoredFrame {
            id: messageCloud
            background: Rectangle {
                radius: 16
                color:"#6482f0"
            }
            width: Math.max(messageBody.contentWidth + 24,
                            messageTime.contentWidth)
            height: messageBody.height + 16
            TextEdit {
                id: messageBody
                text: body
                selectByMouse: true
                width: root.width * 0.5
                color: "white"
                readOnly: true
                font.pointSize: 10
                anchors.verticalCenter:parent.verticalCenter
                font.hintingPreference: Font.PreferNoHinting
                wrapMode: TextEdit.Wrap
            }
        }

        TextEdit {
            id: messageTime
            anchors.topMargin: 10
            //anchors.right: parent.right
            selectByMouse: true
            x: messageCloud.x + messageCloud.width - contentWidth
            color: "white"
            readOnly: true
            text: time.toLocaleString(Qt.locale(Qt.uiLanguage), "MM.dd hh:mm")
            font.pointSize: 8
        }
    }
    AnimatedImage {
        id: statusIcon
        mipmap: true
        sourceSize.width: 20
        sourceSize.height: 20
        Layout.bottomMargin: 14
        Layout.alignment: Qt.AlignLeft | Qt.AlignBottom
        fillMode: Image.PreserveAspectFit
        visible: messageIndex >= 0
        source: {
            if (foreignReadings > messageIndex)
                return Qt.resolvedUrl("pics/readmsg.svg")
            return Qt.resolvedUrl("pics/sent.svg")
        }
    }
    Item {
        id: spacer
        Layout.fillWidth: true
    }
}
