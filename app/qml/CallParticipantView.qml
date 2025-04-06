import QtQuick
import QtQuick.Controls
import ChatClient.Network
import ChatClient.Core
import QuickFuture
import QtMultimedia

Rectangle {
    id: root
    color: "black"
    readonly property var aspectRatio: 1.77778 //16:9
    property var minimumCellWidth: 128
    property alias model: view.model
    required property CallHandle callHandle
    GridView {
        id: view
        property var spacing: 15
        property bool outputSyncNeeded: false
        anchors.centerIn: parent
        property var selectedDistribution: caclWidth(root.width, root.height,
                                                     model.rowCount)
        cellWidth: Math.max(
                       Math.min(
                           root.width / view.selectedDistribution.rows,
                           root.height / view.selectedDistribution.columns * root.aspectRatio),
                       root.minimumCellWidth)
        cellHeight: cellWidth / root.aspectRatio
        width: Math.min(selectedDistribution.rows * cellWidth, root.width)
        height: Math.min(selectedDistribution.columns * cellHeight, root.height)

        clip: true
        delegate: Item {
            id: delegate
            required property var hasAudio
            required property var hasVideo
            required property var userID
            property var isMaximized: false
            height: view.cellHeight
            width: view.cellWidth

            MyAudioOutput {
                id: audioOutput
            }

            onHasAudioChanged: if (hasAudio && userID != CurrentUser.id)
                                   callHandle.connectAudioOutput(
                                               delegate.userID, audioOutput)

            Component.onCompleted: {
                syncOutput()
            }
            Rectangle {
                id: participateBox
                width: Math.max(
                           Math.min(
                               view.width / view.selectedDistribution.rows,
                               view.height / view.selectedDistribution.columns * root.aspectRatio),
                           root.minimumCellWidth) - 8
                height: width / root.aspectRatio - 8

                anchors.centerIn: parent
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
                            script: callHandle.connectVideoSink(
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
                    Future.onFinished(UserController.get(delegate.userID),
                                      function (user) {
                                          participateBox.user = user
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
                    anchors {
                        fill: parent
                        topMargin: 5
                        bottomMargin: 5
                        //rightMargin: 5
                        //leftMargin: 5
                    }
                }

                Label {
                    id: nameLabel
                    font.pointSize: 8
                    anchors {
                        bottom: parent.bottom
                        left: parent.left
                        leftMargin: 10
                        bottomMargin: 4
                    }

                    text: participateBox.user.name
                }
                Menu {
                    id: partMenu
                    MenuItem {
                        text: qsTr("Profile")
                        onTriggered: ProfileViewer.showProfile(
                                         participateBox.user)
                    }
                    MenuSeparator {}
                    Column {
                        Label {
                            anchors.left: parent.left
                            anchors.leftMargin: 5
                            text: qsTr("Volume:")
                        }
                        Slider {
                            from: 0
                            to: 1
                            value: 1
                            onPositionChanged: {
                                audioOutput.setVolume(position)
                            }
                        }
                    }
                }
                MouseArea {
                    id: partMouseArea
                    anchors.fill: parent
                    acceptedButtons: Qt.LeftButton | Qt.RightButton
                    onClicked: mouse => {
                                   if (mouse.button === Qt.RightButton)
                                   partMenu.popup()
                               }
                    cursorShape: Qt.PointingHandCursor
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
                if (hasAudio && userID != CurrentUser.id)
                    callHandle.connectAudioOutput(delegate.userID, audioOutput)
                if (hasVideo)
                    callHandle.connectVideoSink(delegate.userID,
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
            let maxInRow = Math.min(n, Math.floor(
                                        boxWidth / root.minimumCellWidth))
            let m = [1, 1]
            let minDelta = Number.MAX_SAFE_INTEGER
            for (var i = 1; i <= maxInRow; i++) {
                if (n % i == 0) {
                    var delta = Math.abs(
                                (i / (n / i) - boxRatio) / root.aspectRatio)
                    if (minDelta > delta) {
                        m = [i, n / i]
                        minDelta = delta
                    }
                }
            }
            if (isPrime(n)) {
                n++
                maxInRow = Math.min(n, Math.floor(
                                        boxWidth / root.minimumCellWidth))
                for (var i = 1; i < maxInRow; i++) {
                    if (n % i == 0) {
                        var delta = Math.abs(
                                    (i / (n / i) - boxRatio) / root.aspectRatio)
                        if (minDelta > delta) {
                            m = [i, n / i]
                            minDelta = delta
                        }
                    }
                }
            }
            return {
                "rows": m[0],
                "columns": m[1]
            }
        }
    }
    //for view
    function syncOutput() {
        for (var i = 0; i < view.count; i++) {
            console.log(view.itemAtIndex(i))
            view.itemAtIndex(i).syncOutput()
        }
    }
}
