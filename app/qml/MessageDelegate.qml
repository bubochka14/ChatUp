import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material
import ChatClient.Network
import ChatClient.Core

RowLayout {
    id: root
    property alias statusIcon: statusIcon
    signal profileClicked
    layoutDirection:Qt.RightToLeft
    Text{
        text:messageIndex
    }

    Column {
        id: column

        RoundedFrame {
            id: messageCloud
            radius: 15
            Material.elevation: 50
            Material.background: "#3193ec"
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
            text: time.toLocaleString(Qt.locale(Qt.uiLanguage), "MM-dd hh:mm")
            font.pointSize: 8
        }
    }
    AnimatedImage {
        id: statusIcon
        mipmap: true
        height: 20
        width: 20
        Layout.bottomMargin: 14
        Layout.alignment: Qt.AlignLeft | Qt.AlignBottom
        fillMode: Image.PreserveAspectFit
        source: {
            if (messageStatus === MessageModel.Loading)
                return Qt.resolvedUrl("gif/duck.gif")
            if (messageStatus === MessageModel.Read)
                return Qt.resolvedUrl("pics/read.svg")
            if (messageStatus === MessageModel.Error)
                return Qt.resolvedUrl("pics/error.svg")
            if (messageStatus === MessageModel.Sent)
                return Qt.resolvedUrl("pics/sent.svg")
        }
    }
    Item {
        id: spacer
        Layout.fillWidth: true
    }
}
