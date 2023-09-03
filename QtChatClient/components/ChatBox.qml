import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Pane {
    id: root
    Material.background: "#788C7F"
    property alias model: listView.model
    ScrollView {
        anchors.fill :parent

        ListView {
            id: listView
            spacing: 15
            delegate: Item {
                id: messageBox
                height: cloud.height
                Text {
                    id: userLabel
                    anchors.bottom: parent.bottom
                    text: user.name + ":"
                    font.pointSize: 11
                }
                Rectangle {
                    id: cloud
                    anchors.left: userLabel.right
                    anchors.leftMargin: 10
                    width: messageBody.contentWidth + messageTime.contentWidth + 20
                    height: messageBody.contentHeight + 20
                    color: "darkgrey"
                    border.width: 1
                    radius: 12
                    Label {
                        id: messageBody
                        Material.foreground: "#1F2320"
                        anchors.left: cloud.left
                        anchors.leftMargin: 5
                        anchors.verticalCenter: cloud.verticalCenter
                        text: body
                     //   font.pointSize: 9
                    }
                    Label {
                        id: messageTime
                        Material.foreground: "#1F2320"

                        anchors.bottom: cloud.bottom
                        anchors.left: messageBody.right
                        anchors.leftMargin: 5
                        text: time
                        font.pointSize: 5
                    }
                }
            }
        }
    }
}
