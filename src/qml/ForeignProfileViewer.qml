import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material
import ChatClient

Dialog {
    id: root
    signal sendMessageClicked()
    width:250
    height:400
    modal: true
    function showProfle(user)
    {
        tagLabel.text = "Tag: "+user.tag
        nameLabel.text = "Name: " +user.name
        open()
    }

    RoundedFrame {
        anchors.fill: parent
        ColumnLayout {
            anchors.top: parent.top
            anchors.right: parent.right
            anchors.left: parent.left
            anchors.topMargin: 40
            spacing: 25
            Image {
                id: icon
                Layout.alignment: Qt.AlignHCenter
                source: "pics/profile.svg"
                height: 75
                width: 75
            }
            Label
            {
                id:tagLabel
                Layout.alignment: Qt.AlignHCenter
            }

            Label {
                id:nameLabel
                Layout.alignment: Qt.AlignHCenter
            }
            Button{
                text:"Send Message"
                onClicked: {root.sendMessageClicked();accept()}
            }
        }
    }
}
