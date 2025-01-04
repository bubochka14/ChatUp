import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material
import ChatClient.Network
import ChatClient.Core

Dialog {
    id: root
    width: 250
    height: 400
    modal: true
    background: Rectangle {
        radius: 30
        color: Material.background
        Rectangle {

            gradient: Gradient {
                GradientStop {
                    position: 0
                    color: "#6058fb"
                }
                GradientStop {
                    position: 1
                    color: "#3193ec"
                }
            }
            radius: 30
            width: parent.width
            height: 165
            anchors.top: parent.top
            Rectangle {
                width: parent.width
                height: 200
                color: Material.background
                anchors.top: parent.verticalCenter
            }
        }
    }

    ColumnLayout {
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.left: parent.left
        anchors.topMargin: 15
        spacing: 25
        Image {
            id: icon
            Layout.alignment: Qt.AlignHCenter
            source: "pics/profile.svg"
            height: 75
            width: 75
        }
        Label {
            id: tagLbl
            property string tag
            Layout.alignment: Qt.AlignHCenter
            text: "Tag: " + tag
        }

        Label {
            id: nameLbl
            property string name
            Layout.alignment: Qt.AlignHCenter
            text: "Name: " +name
        }
    }
    function show(user)
    {
        tagLbl.tag = user.tag
        nameLbl.name = user.name
        open()
    }
}
