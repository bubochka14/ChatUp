import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material
import ChatClient.Network
import ChatClient.Core
import QuickFuture

Item {
    id: root
    function showProfile(user: UserHandle) {
            localProfileViewer.showProfle(user)
    }

    Dialog {
        id: localProfileViewer
        property UserHandle user
        width: 250
        height: 400
        anchors.centerIn: parent
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
                        color: "#6482f0"
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

        RowLayout {
            anchors.top: parent.top
            anchors.right: parent.right
            anchors.left: parent.left
            anchors.topMargin: 30
            Rectangle {
                height: 70
                width: 70
                radius:70
                color:Material.background
                Image {
                    id: icon
                    anchors.centerIn:parent
                    source: "pics/profile.svg"
                    sourceSize.height: 60
                    sourceSize.width: 60
                    Rectangle{
                        height:24
                        width:24
                        visible: localProfileViewer.user.status == 1
                        color:Material.background
                        anchors.bottom:parent.bottom
                        anchors.right:parent.right
                        anchors.rightMargin:-4
                        anchors.bottomMargin:-4

                        radius:26
                        Rectangle{
                            height:16
                            width:16
                            radius:18
                            anchors.centerIn:parent
                            color:"#6482f0"
                        }
                    }
                }
            }
            Column {
                spacing:5
                Layout.fillWidth:true
                Label {
                    id: nameLbl
                    elide: Qt.ElideRight
                    width:parent.width
                    text: localProfileViewer.user.name
                    font.pointSize: 14
                    font.bold: true
                }
                Label {
                    id: tagLbl
                    width:parent.width
                    elide: Qt.ElideRight
                    property string tag
                    text: "@" + localProfileViewer.user.tag
                    font.pointSize: 10
                }
            }
        }
        function showProfle(user) {
            localProfileViewer.user = user
            open()
        }
    }

}
