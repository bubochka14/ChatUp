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
    signal connect
    signal disconnect
    states: [
        State {
            name: "disconnected"
            PropertyChanges {
                root.visible: false
                interactionBtn.text: "Call"
            }
        },
        State{
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
        if(root.callHandler.state == CallHandler.HasCall)
            return "notJoined"
        if(root.callHandler.state == CallHandler.InsideTheCall)
            return "joined"
        else return "disconnected"
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
                text: "Media"
            }
        }
    }
}
