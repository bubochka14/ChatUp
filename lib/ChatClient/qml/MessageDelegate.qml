import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material
import ChatClient

RowLayout {
    property UserInfo user
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
        Layout.preferredHeight: messageBody.contentHeight + messageTime.contentHeight + 16
        Column {
            anchors.fill: parent
            anchors.margins: 8
            width: parent.width
            TextEdit {
                Material.foreground: "Grey"
                id: messageBody
                text: body
                width: parent.width
                selectByMouse: true
                readOnly: true
                font.pointSize: 10
                font.hintingPreference: Font.PreferNoHinting
                wrapMode: TextEdit.Wrap
            }
            TextEdit {
                id:messageTime
                Material.foreground: "Grey"
                selectByMouse: true
                readOnly: true
                text: time.toLocaleString(Qt.locale(Qt.uiLanguage),
                                          "dddd MM-dd hh:mm")
                font.pointSize: 6
            }
        }
    }
}
