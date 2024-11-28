import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material
import ChatClient

Rectangle {
    id: root
    color:"black"
    ColumnLayout {
        anchors.fill: parent
        Item {
            Layout.fillHeight: true
            Layout.fillWidth: true
        }
        Row
        {
            spacing:5
            Layout.alignment: Qt.AlignHCenter
            Button
            {
                text: "Call"
            }
            Button{
                text: "Media"
            }
        }
    }
}
