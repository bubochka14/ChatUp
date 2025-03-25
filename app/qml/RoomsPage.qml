import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material
import ChatClient.Network
import ChatClient.Core
import QuickFuture
import ObjectConverter

SplitView {
    id: root
    spacing: 0
    required property ControllerManager manager
    property bool roomSelected: false
    property alias selectedRoomID: chatBox.roomID
    Component.onCompleted: {
        manager.groupController.load()
    }

    RoomList {
        id: roomList
        //SplitView.minimumWidth: 120
        SplitView.maximumWidth: root.width / 2

        SplitView.fillHeight: true
        SplitView.preferredWidth: Math.max(220, parent.width / 4)
        Layout.maximumWidth: 420
        roomModel: manager.groupController.model
        onSelectedRoomChanged: {
            if (!roomSelected)
                roomSelected = true
            chatBox.roomID = roomList.selectedRoom.id
            roomHeader.title = roomList.selectedRoom.name
        }
        listView.footer: StandardDelegate {
            id: createRoomFooter
            label.text: qsTr("Create")
            icon: Rectangle {
                height: 50
                width: 50
                radius: 20
                color: "#19182a"
                Label {
                    text: "+"
                    color: "white"
                    font.pixelSize: 40
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.bottom: parent.bottom
                    anchors.bottomMargin: 3
                }
            }
            width: roomList.listView.width
            onClicked: createRoomDialog.open()
        }
    }
    ColumnLayout {
        id: chatColumn
        spacing: 0
        SplitView.fillWidth: true
        // Layout.minimumWidth: 350
        ColoredFrame {
            id: roomHeader
            property alias title: titleLabel.text
            leftInset: -1
            visible: roomSelected
            implicitHeight: 50
            Layout.fillWidth: true
            Label {
                anchors.centerIn: parent
                font.bold: true
                id: titleLabel
            }
            Row {
                anchors.right: parent.right
                anchors.rightMargin: 15
                spacing: 10
                IconButton {
                    anchors.verticalCenter: parent.verticalCenter
                    source: Qt.resolvedUrl("pics/call")
                    height: 20
                    width: 20
                    onClicked: manager.callController.handler(
                                   root.selectedRoomID).join()
                }
                MouseArea {
                    id: addUserBtn
                    anchors.verticalCenter: parent.verticalCenter
                    height: 30
                    width: 30
                    cursorShape: Qt.PointingHandCursor
                    onClicked: drawer.open()
                    Image {
                        id: addUserIcon
                        anchors.centerIn: parent
                        height: 26
                        width: 26
                        mipmap: true
                        source: Qt.resolvedUrl("pics/add-user.svg")
                    }
                }
            }
        }

        ChatBox {
            id: chatBox
            focus: true
            leftInset: -1
            initalMessage: qsTr("Select ChatRoom to start messaging")
            manager: root.manager
            Layout.fillHeight: true
            Layout.fillWidth: true
        }
    }

    CreateRoomDialog {
        id: createRoomDialog
        anchors.centerIn: parent
        onAccepted: manager.groupController.create(roomName)
    }
    Drawer {
        id: drawer
        property alias model: view.model
        width: 300
        padding: 20
        height: root.height
        edge: Qt.RightEdge
        y: 31 // sysbar + border
        ColumnLayout {
            anchors.fill: parent
            Label {
                text: qsTr("Users:")
                font.pointSize: 20
            }
            Button {
                text: "Add user"
                onClicked: selectUserDialog.open()
            }
            UsersView {
                id: view
                Layout.fillWidth: true
                Layout.fillHeight: true
            }
        }
    }
    SelectUserDialog {
        id: selectUserDialog
        anchors.centerIn: parent
        title: qsTr("Add memder:")
        onSeacrhPatternChanged: {
            let future = manager.userController.find(seacrhPattern, 5)
            Future.onFinished(future, function (users) {
                selectUserDialog.usersModel = users
            })
        }
        onUserSelected: id => GroupController.addUser(id,
                                                      roomList.selectedRoom.id)
    }
    onSelectedRoomIDChanged: {
        Future.onFinished(UserController.getGroupUsers(root.selectedRoomID),
                          function (users) {
                              drawer.model = users
                          })
    }
}
