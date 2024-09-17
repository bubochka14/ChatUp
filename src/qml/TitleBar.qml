import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material
import QWindowKit 1.0

Rectangle {
    id: titleBar
    required property WindowAgent agent
    required property Window window
    color: Material.background

    Image {
        id: iconButton
        anchors {
            verticalCenter: parent.verticalCenter
            left: parent.left
            leftMargin: 10
        }
        width: 18
        height: 18
        mipmap: true
        source: Qt.resolvedUrl("pics/send.svg")
        fillMode: Image.PreserveAspectFit
        Component.onCompleted: agent.setSystemButton(WindowAgent.WindowIcon,
                                                     iconButton)
    }

    Text {
        anchors {
            verticalCenter: parent.verticalCenter
            left: iconButton.right
            leftMargin: 10
        }
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        text: window.title
        font.pixelSize: 14
        color: "#ECECEC"
    }

    Row {
        anchors {
            top: parent.top
            right: parent.right
        }
        height: parent.height

        SysButton {
            id: minButton
            height: parent.height
            width:height*1.3
            text: "🗕"
            onClicked: window.showMinimized()
            background: Rectangle {
                color: {
                    if (minButton.hovered) {
                        return "#gray"
                    }
                    return "transparent"
                }
            }
            Component.onCompleted: agent.setSystemButton(WindowAgent.Minimize,
                                                         minButton)
        }

        SysButton {
            id: maxButton
            height: parent.height
            width:height*1.3
            text: window.visibility == Window.Maximized ? "🗗" : "🗖"
            onClicked: {
                if (window.visibility === Window.Maximized) {
                    window.showNormal()
                } else {
                    window.showMaximized()
                }
            }
            background: Rectangle {
                color: {
                    if (maxButton.hovered) {
                        return "#gray"
                    }
                    return "transparent"
                }
            }
            Component.onCompleted: agent.setSystemButton(WindowAgent.Maximize,
                                                         maxButton)
        }

        SysButton {
            id: closeButton
            height: parent.height
            width:height*1.3
            text: "🗙"
            background: Rectangle {
                color: {
                    if (!closeButton.enabled) {
                        return "gray"
                    }
                    if (closeButton.hovered) {
                        return "#e81123"
                    }
                    return "transparent"
                }
            }
            onClicked: window.close()
            Component.onCompleted: agent.setSystemButton(WindowAgent.Close,
                                                         closeButton)
        }
    }
}
