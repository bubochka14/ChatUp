import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material
import ChatClient.Network
import ChatClient.Core

Dialog {
    id: root
    signal sendMessageClicked
    width: 250
    height: 400
    modal: true
    padding:0
    topPadding: 0
    function showProfle(user) {
        tagLabel.text = "Tag: " + user.tag
        nameLabel.text = "Name: " + user.name
        open()
    }


        background: Rectangle
        {
            radius:30
            color: Material.background
            Rectangle
            {

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
                height:165
                anchors.top: parent.top
                Rectangle
                {
                    width: parent.width
                    height:200
                    color: Material.background
                    anchors.top: parent.verticalCenter
                }
            }
        }

        ColumnLayout {
            anchors.top: parent.top
            anchors.right: parent.right
            anchors.left: parent.left
            anchors.topMargin: 30
            spacing: 25
            Image {
                id: icon
                Layout.alignment: Qt.AlignHCenter
                source: "pics/profile.svg"
                height: 75
                width: 75
            }
            Label {
                id: tagLabel
                Layout.alignment: Qt.AlignHCenter
            }

            Label {
                id: nameLabel
                Layout.alignment: Qt.AlignHCenter
            }
            Button {
                text: "Send Message"
                Layout.alignment: Qt.AlignHCenter
                onClicked: {
                    root.sendMessageClicked()
                    accept()
                }
            }
        }

    }
