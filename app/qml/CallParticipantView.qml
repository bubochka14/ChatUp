import QtQuick
import QtQuick.Controls
import ChatClient.Network
import ChatClient.Core
import QuickFuture
import QtMultimedia

Rectangle {
    id: root
    color: "black"
    readonly property var aspectRatio: 1.77778
    property var minimumCellWidth: 128
    property var minimumCellHeight: 72
    property alias model: view.model
    required property CallHandler callHandler

    GridView {
        id: view
        property var spacing: 15
        property bool outputSyncNeeded: false
        anchors.centerIn: parent
        cellWidth: caclWidth(root.width - 25, root.height - 25, model.rowCount)
        cellHeight: cellWidth / root.aspectRatio
        clip: true
        delegate: Item {
            id:root
            required property var hasAudio
            required property var hasVideo
            required property var userID
            property var isMaximized: false
            height: GridView.view.cellHeight
            width: GridView.view.cellWidth
            MyAudioOutput
            {
                id:audioOutput
            }

            onHasAudioChanged: if (hasAudio)
                                   callHandler.connectAudioOutput(root.userID,
                                                                  audioOutput)

            Component.onCompleted: {
                syncOutput()
            }
            RoundedFrame {
                id: delegate
                anchors.centerIn: parent
                height: parent.height - view.spacing
                width: parent.width - view.spacing

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
                            script: callHandler.connectVideoSink(
                                        userID, videoOutput.videoSink)
                        }
                    }
                ]
                state: {
                    if (hasVideo) {
                        return "video"
                    }
                    return "icon"
                }
                radius: 15
                Component.onCompleted: {
                    Future.onFinished(UserController.get(root.userID),
                                      function (user) {
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
                    height: 36
                    width: 36
                    mipmap: true
                    anchors {
                        bottom: parent.bottom
                        right: parent.right
                        rightMargin: 10
                        bottomMargin: 2
                    }
                }
            }
            //for delegate
            function syncOutput() {
                console.log("inside")
                if (hasAudio)
                    callHandler.connectAudioOutput(root.userID, audioOutput)
                if (hasVideo)
                    callHandler.connectVideoSink(root.userID,
                                                 videoOutput.videoSink)
            }
        }
        function isPrime(num) {
            for (var i = 2, s = Math.sqrt(num); i <= s; i++) {
                if (num % i === 0)
                    return false
            }
            return num > 2
        }
        function caclWidth(boxWidth, boxHeight, n) {
            let boxRatio = boxWidth / boxHeight
            let m = [1, 1]
            let minDelta = Number.MAX_SAFE_INTEGER
            for (var i = 1; i <= n; i++) {
                if (n % i == 0) {
                    var delta = Math.abs(
                                i * i / n - boxRatio / root.aspectRatio)
                    if (minDelta > delta) {
                        m = [i, n / i]
                        minDelta = delta
                    }
                }
            }
            let newWidth = Math.max(Math.min(
                                        boxWidth / m[0],
                                        boxHeight / m[1] * root.aspectRatio),
                                    minimumCellWidth)
            let newHeight = Math.max(Math.min(
                                         boxWidth / m[1],
                                         boxHeight / m[0] / root.aspectRatio),
                                     minimumCellHeight)
            view.width = Math.min(newWidth * m[0], boxWidth)
            view.height = Math.min(newWidth * m[1], boxHeight)

            return newWidth
        }

        // onModelChanged: resetSize()
        // function resetSize()
        // {
        //     view.width = Math.min(parent.width/baseWidth, root.model.rowCount()) * idealWidth
        //     view.height = Math.min(parent.height/baseHeight, root.model.rowCount()) * idealHeight

        // }
    }
    //for view
    function syncOutput() {
        console.log("SYNC")
        for (var i = 0; i < view.count; i++) {
            console.log(view.itemAtIndex(i))
            view.itemAtIndex(i).syncOutput()
        }
    }
}
