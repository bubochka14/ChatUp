import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material
import ChatClient

RowLayout {
    property UserInfo user
    property bool currentUser: false
    id: messageBox
    spacing: 10
    TextEdit {
        id: userLabel
        readOnly: true
        text: user.name
        Material.foreground: "#2c6337"
        selectByMouse: true
        Layout.alignment: Qt.AlignBottom
        font.pointSize: 13
    }
    RoundedFrame {
        id: messageCloud
        radius: 15
        z: -1
        Material.elevation: 50
        Material.background: "DarkGrey"
        Layout.minimumWidth: Math.max(messageBody.contentWidth,messageTime.contentWidth)+16
        Layout.preferredWidth: Math.max(messageBody.contentWidth,messageTime.contentWidth)+16
        Layout.minimumHeight: messageBody.contentHeight + messageTime.contentHeight + 16
        Layout.preferredHeight: messageBody.contentHeight + messageTime.contentHeight + 16
        TextEdit {
            Material.foreground: "Grey"
            id: messageBody
            text: body
            selectByMouse: true
            width: messageBox.width*0.5
            readOnly: true
            font.pointSize: 10
            font.hintingPreference: Font.PreferNoHinting
            wrapMode: TextEdit.Wrap
        }
        TextEdit {
            id: messageTime
            Material.foreground: "Grey"
            selectByMouse: true
            readOnly: true
            text: time.toLocaleString(Qt.locale(Qt.uiLanguage),
                                      "dddd MM-dd hh:mm")
            font.pointSize: 6
        }
    }

    AnimatedImage {
        id: loadingAnimation
        mipmap: true
        Layout.preferredHeight: 25
        Layout.preferredWidth: 25
        visible: currentUser
        Layout.alignment: Qt.AlignLeft | Qt.AlignBottom
        fillMode: Image.PreserveAspectFit
        source: {
            if (messageStatus === MessageModel.Loading)
                return Qt.resolvedUrl("resources/gif/duck.gif")
            if (messageStatus === MessageModel.Read)
                return Qt.resolvedUrl("resources/pics/read.svg")
            if (messageStatus === MessageModel.Error)
                return Qt.resolvedUrl("resources/pics/error.svg")
            if (messageStatus === MessageModel.Sent)
                return Qt.resolvedUrl("resources/pics/sent.svg")
        }
    }
    Item{
        id:spacer
        Layout.fillWidth: true
    }
}
