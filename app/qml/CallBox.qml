import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material
import ChatClient.Network
import ChatClient.Core
import QuickFuture

Rectangle {
    id: root
    required property var roomID
    property var minimumHeight: 140
    property CallHandle callHandle
    property var participantCount: 0
    signal connect
    signal disconnect
    states: [
        State {
            name: "disconnected"
            PropertyChanges {
                root.visible: false
                mediaBtn.visible: false
                voiceBtn.visible: false
            }
        },
        State {
            name: "notJoined"
            PropertyChanges {
                root.visible: true
                interactionBtn.source: Qt.resolvedUrl("pics/startcall")
                mediaBtn.visible: false
                voiceBtn.visible: false
            }
        },
        State {
            name: "joined"
            PropertyChanges {
                root.visible: true
                interactionBtn.source: Qt.resolvedUrl("pics/endcall")
                mediaBtn.visible: true
                voiceBtn.visible: true
            }
            StateChangeScript {
                script: view.syncOutput()
            }
        }
    ]
    color: "black"
    state: {
        if (root.callHandle.state == CallHandle.InsideTheCall)
            return "joined"
        if (root.callHandle.participants.rowCount > 0)
            return "notJoined"
        else
            return "disconnected"
    }
    Component.onCompleted: {
        root.callHandle = CallController.handle(root.roomID)
        view.model = root.callHandle.participants
    }
    CallParticipantView {
        id: view
        anchors.fill: parent
        callHandle: root.callHandle
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
                    root.callHandle.disconnect()
                } else {
                    root.callHandle.join()
                }
            }
        }
        IconButton {
            id: mediaBtn
            height: 45
            width: 45
            source: root.callHandle.hasVideo ? Qt.resolvedUrl(
                                                   "pics/cameraopen") : Qt.resolvedUrl(
                                                   "pics/cameraclose")
            onClicked: {
                if (root.callHandle.hasVideo) {
                    root.callHandle.closeVideo()
                } else {
                    CameraPipeline.currentDevice = ApplicationSettings.videoDevice
                    root.callHandle.openVideo(CameraPipeline)
                }
            }
        }
        IconButton {
            id: voiceBtn
            height: 45
            width: 45
            source: root.callHandle.hasAudio ? Qt.resolvedUrl(
                                                   "pics/micround") : Qt.resolvedUrl(
                                                   "pics/nomicround")
            onClicked: {
                if (root.callHandle.hasAudio) {
                    root.callHandle.closeAudio()
                } else {
                    MicrophonePipeline.currentDevice = ApplicationSettings.audioDevice
                    root.callHandle.openAudio(MicrophonePipeline)
                }
            }
        }
    }
    Menu {
        id: partMenu
        Slider {
            anchors.fill: parent
        }
    }
}
