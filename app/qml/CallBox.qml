import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material
import ChatClient.Network
import QuickFuture

Rectangle {
    id: root
    required property ControllerManager manager
    property var minimumHeight: 140
    signal connect
    signal disconnect
    states: [
        State {
            name: "disconnected"
            PropertyChanges {
                target: interactionBtn
                text: "Call"
            }
        },
        State {
            name: "connected"
            PropertyChanges {
                target: myRect
                color: "Disconnect"
            }
        }
    ]
    state: "disconnected"
    color: "black"
    ColumnLayout {
        anchors.fill: parent
        CallParticipantView {
            id: view
            boxHeight: root.height
            boxWidth: root.width
            Component.onCompleted: {
                Future.onFinished(manager.callController.getCallParticipants(
                                      0), function (model) {
                                          view.model = model
                                      })
            }
            Layout.fillHeight: true
            Layout.fillWidth: true
        }
        Row {
            spacing: 5
            Layout.alignment: Qt.AlignHCenter
            Button {
                id: interactionBtn
                onClicked: {
                    if (root.state == "connected")
                        root.connect()
                    else
                        root.disconnect()
                }
            }
            Button {
                text: "Media"
            }
        }
    }
}
