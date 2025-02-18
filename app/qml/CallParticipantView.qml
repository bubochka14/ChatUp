import QtQuick
import QtQuick.Controls
import ChatClient.Network
import ChatClient.Core
import QuickFuture
import QtMultimedia

Rectangle {
    id: root
    color: "black"
    property var boxHeight
    property var boxWidth
    property alias model: view.model
    required property CallHandler callHandler

    GridView {
        id: view
        property var spacing: 15
        property var baseWidth: 300
        property var baseHeight: 200
        anchors.centerIn: parent
        cellWidth: baseWidth
        cellHeight: baseHeight
        width: Math.floor(root.boxWidth/cellWidth) * cellWidth
        height: Math.floor(root.boxHeight/cellHeight)* cellHeight
        clip: true

        delegate: RoundedFrame {
            id: delegate
            padding: 0
            color: "#19182a"
            property UserHandle user: UserController.empty
            states: [
                State {
                    name: "icon"
                    PropertyChanges {
                        profileIcon.visible: true
                        loadingIcon.visible: false
                        videoOutput.visible: false

                    }
                },
                State {
                    name: "loading"
                    PropertyChanges {
                        profileIcon.visible: false
                        loadingIcon.visible: true
                        videoOutput.visible: false
                    }
                },
                State {
                    name: "video"
                    PropertyChanges {
                        profileIcon.visible: false
                        loadingIcon.visible: false
                        videoOutput.visible: true
                    }
                    StateChangeScript {
                          name: "sinkConnectScript"
                          script: callHandler.connectVideoSink(userID, videoOutput.videoSink)

                      }
                }
            ]
            state: {
                if(hasVideo)
                {
                    return "video"
                }
                return "icon"
            }
            height: GridView.view.cellHeight - view.spacing
            width: GridView.view.cellWidth - view.spacing
            radius: 15
            Component.onCompleted: {
                Future.onFinished(UserController.get(userID), function (user) {
                    delegate.user = user
                })
            }
            Image {
                id: profileIcon
                source: "pics/profile.svg"
                sourceSize.height: 30
                sourceSize.width: 30
                anchors.centerIn: parent
            }
            AnimatedImage {
                id: loadingIcon
                visible: false
                mipmap: true
                anchors.centerIn: parent
                sourceSize.height: 90
                sourceSize.width: 135
                source: Qt.resolvedUrl("gif/loading.gif")
            }
            VideoOutput {
                id: videoOutput
                anchors.fill: parent
            }
            Label {
                id: nameLabel
                font.pointSize: 8
                anchors {
                    bottom: parent.bottom
                    left: parent.left
                    leftMargin: 10
                    bottomMargin: 2
                }

                text: delegate.user.name
            }
            Image {
                id: noMicLbl
                visible: !hasAudio
                source: Qt.resolvedUrl("pics/nomicround")
                height:36
                width:36
                mipmap: true
                anchors {
                    bottom: parent.bottom
                    right: parent.right
                    rightMargin: 10
                    bottomMargin: 2

                }
            }
        }
        // onModelChanged: resetSize()
        // function resetSize()
        // {
        //     view.width = Math.min(parent.width/baseWidth, root.model.rowCount()) * idealWidth
        //     view.height = Math.min(parent.height/baseHeight, root.model.rowCount()) * idealHeight

        // }
    }
}
