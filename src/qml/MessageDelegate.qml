import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material
import ChatClient

RowLayout {
    property UserInfo user
    property bool currentUser: false
    property alias statusIcon: statusIcon
    signal profileClicked
    id: root
    layoutDirection: currentUser ? Qt.RightToLeft : Qt.LeftToRight
    Image {
        id: icon
        source: "pics/profile.svg"
        sourceSize.height: 30
        sourceSize.width: 30
        visible: !currentUser
        Layout.alignment: Qt.AlignBottom
        MouseArea {
            anchors.fill: parent
            cursorShape: Qt.PointingHandCursor
            onClicked: root.profileClicked()
        }
    }
    RoundedFrame {
        id:messageCloud
        radius: 15
        Material.elevation: 50
        Material.background: currentUser ? "#3193ec" : "#19182a"
        Layout.preferredWidth: Math.max(messageBody.contentWidth,
                                        messageTime.contentWidth) + 24
        Layout.preferredHeight: bodyColumn.height + 16
        Column {
            id:bodyColumn
            spacing:0
            TextEdit
            {
                id:nameField
                visible: !root.currentUser
                color: "gray"
                text: user.name
                readOnly: true
                selectByMouse: true
                MouseArea
                {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onClicked: root.profileClicked()                }

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
        TextEdit {
            id: messageTime
            anchors.topMargin: 10
            anchors.right: parent.right
            anchors.top: parent.bottom
            selectByMouse: true
            color: "white"
            readOnly: true
            text: time.toLocaleString(Qt.locale(Qt.uiLanguage),
                                      "dddd MM-dd hh:mm")
            font.pointSize: 6
        }
    }
    AnimatedImage {
        id: statusIcon
        mipmap: true
        Layout.preferredHeight: 20
        Layout.preferredWidth: 20
        visible: currentUser
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
