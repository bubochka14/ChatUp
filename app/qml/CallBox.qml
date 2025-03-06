import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material
import ChatClient.Network
import ChatClient.Core
import QuickFuture

Rectangle {
    id: root
    required property ControllerManager manager
    required property var roomID
    property var minimumHeight: 140
    property CallHandler callHandler
    property var participantCount: 0
    signal connect
    signal disconnect
    // Component.onCompleted:
    // {
    //     root.participantCount = root.callHandler.participants.rowCount()
    // }
    // Connections {
    //     target: callHandler
    //     function onProfileClicked() {
    //         root.userProfileClicked(userID)
    //     }
    // }
    states: [
        State {
            name: "disconnected"
            PropertyChanges {
                root.visible: false
            }
        },
        State {
            name: "notJoined"
            PropertyChanges {
                root.visible: true
                interactionBtn.source: Qt.resolvedUrl("pics/startcall")
            }
        },
        State {
            name: "joined"
            PropertyChanges {
                root.visible: true
                interactionBtn.source: Qt.resolvedUrl("pics/endcall")
            }
        }
    ]
    color: "black"
    state: {
        if (root.callHandler.state == CallHandler.InsideTheCall)
            return "joined"
        if (root.callHandler.participants.rowCount > 0)
            return "notJoined"
        else
            return "disconnected"
    }
    Component.onCompleted: {
        root.callHandler = manager.callController.handler(root.roomID)
        view.model = root.callHandler.participants
    }
    CallParticipantView {
        id: view
        boxHeight: root.height
        boxWidth: root.width
        anchors.fill: parent
        callHandler: root.callHandler
    }
    Row {
        spacing: 5
        anchors {
            bottom: view.bottom
            bottomMargin: 5
            horizontalCenter: parent.horizontalCenter
        }
        IconButton {
            id: interactionBtn
            height: 45
            width: 45
            onClicked: {
                if (root.state == "joined") {
                    root.callHandler.disconnect()
                } else {
                    root.callHandler.join()
                }
            }
        }
        IconButton {
            height: 45
            width: 45
            source: root.callHandler.hasVideo ? Qt.resolvedUrl(
                                                    "pics/cameraopen") : Qt.resolvedUrl(
                                                    "pics/cameraclose")
            onClicked: {
                if (root.callHandler.hasVideo) {
                    root.callHandler.closeVideo()
                } else {
                    CameraPipeline.currentDevice = ApplicationSettings.videoDevice
                    root.callHandler.openVideo(CameraPipeline)
                }
            }
        }
        IconButton {
            height: 45
            width: 45
            source: root.callHandler.hasAudio ? Qt.resolvedUrl(
                                                    "pics/micround") : Qt.resolvedUrl(
                                                    "pics/nomicround")
            onClicked: {
                if (root.callHandler.hasAudio) {
                    root.callHandler.closeAudio()
                } else {
                    MicrophonePipeline.currentDevice = ApplicationSettings.audioDevice
                    root.callHandler.openAudio(MicrophonePipeline)
                }
            }
        }
    }
}
