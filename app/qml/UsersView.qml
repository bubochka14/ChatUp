import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs
import ChatClient.Core
import QtQuick.Controls.Material

Item {
    id: root
    signal userClicked(UserHandle handle)
    property alias model: proxy.model
    DelegateModel {
        id: proxy
        property var lessThan: [function (left, right) {
            return left.status < right.status
        }]
        groups: [
            DelegateModelGroup {
                id: online
                name: "online"
            },
            DelegateModelGroup {
                id: offline
                name: "offline"
                onChanged: (removed, inserted) => {
                               if (inserted[0]) {
                                   move(inserted[0].index, 0, inserted[0].count)
                               }
                           }
            },
            DelegateModelGroup {
                id: all
                name: "all"
                includeByDefault: true
            }
        ]
        filterOnGroup: "all"
        items.includeByDefault: false
        delegate: StandardDelegate {
            required property var handle
            required property string name
            required property var status
            required property string tag

            id: delegate
            width: root.width
            height: 40
            onClicked: root.userClicked(handle)
            label.text: name
            label.font.pointSize: 12
            subLabel.font.pointSize: 10
            subLabel.color: "gray"
            subLabel.text: "@" + tag
            Component.onCompleted: {
                delegate.DelegateModel.inOnline = delegate.status
                delegate.DelegateModel.inOffline = !delegate.status

            }
            onStatusChanged: {
                delegate.DelegateModel.inOnline = delegate.status
                delegate.DelegateModel.inOffline = !delegate.status
            }

            icon: Image {
                id: icon
                source: "pics/profile.svg"
                sourceSize.height: 30
                sourceSize.width: 30
                Rectangle {
                    width: 15
                    height: 15
                    color: Material.background
                    visible: status == 1
                    radius: 15
                    anchors {
                        bottom: parent.bottom
                        right: parent.right
                        rightMargin: -2
                        bottomMargin: -2
                    }

                    Rectangle {
                        height: 10
                        width: 10
                        radius: 10
                        anchors.centerIn: parent
                        color: "#6482f0"
                    }
                }
            }
        }
    }

    ListView {
        id: view
        boundsBehavior: Flickable.StopAtBounds
        clip: true
        anchors.fill: parent
        model: proxy
        spacing: 4
    }
}
