import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material
import ChatClient.Network
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
                interactionBtn.text: "Call"
            }
        },
        State {
            name: "notJoined"
            PropertyChanges {
                root.visible: true
                interactionBtn.text: "Join"
            }
        },
        State {
            name: "joined"
            PropertyChanges {
                root.visible: true
                interactionBtn.text: "Disconnect"
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
    ColumnLayout {
        anchors.fill: parent
        CallParticipantView {
            id: view
            boxHeight: root.height
            boxWidth: root.width
            Layout.fillHeight: true
            Layout.fillWidth: true
            callHandler: root.callHandler
        }
        Row {
            spacing: 5
            Layout.alignment: Qt.AlignHCenter
            Button {
                id: interactionBtn
                onClicked: {
                    if (root.state == "joined") {
                        root.callHandler.disconnect()
                    } else {
                        root.callHandler.join()
                    }
                }
            }
            Button {
                text: root.callHandler.hasVideo ? "Media OFF" : "Media ON"
                onClicked: {
                    if (root.callHandler.hasVideo) {
                        root.callHandler.closeVideo()
                    } else {
                        CameraPipeline.currentDevice = CameraPipeline.availableDevices[0]
                        root.callHandler.openVideo(CameraPipeline)
                    }
                }
            }
            Button {
                text: root.callHandler.hasAudio ? "Audio OFF" : "Audio ON"
                onClicked: root.callHandler.hasAudio = !root.callHandler.hasAudio
            }
        }
    }
}
