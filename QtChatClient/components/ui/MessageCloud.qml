import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

RoundedFrame {
    id: fr
    radius: 15
    z: -1
    Material.elevation: 50
    Material.background: "DarkGrey"
    //    height: messageBox.height
    //    width: listView.width
    property alias messageTime: timeLabel.text
    property alias messageBody: messageBody.text
    property int maxWidth
    ColumnLayout {
        id: col
        anchors.fill: parent
        TextLabel {
            id: messageBody
            Layout.preferredHeight: contentHeight
            Layout.maximumWidth: maxWidth
            Material.foreground: "Grey"
            text: body
            width: parent.width
            wrapMode: TextEdit.Wrap
        }
        TextLabel {
            id: timeLabel
            Layout.maximumWidth: maxWidth
            Material.foreground: "Grey"
            font.pointSize: 6
        }
    }
}
