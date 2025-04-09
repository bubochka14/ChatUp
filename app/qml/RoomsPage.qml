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
    property bool roomSelected: false
    Component.onCompleted: {
        Future.onFinished(GroupController.load(), function (){
            console.log("HGI")
        })
    }
    handle: Rectangle {
        implicitWidth: 5
        color: Material.background
    }
    ColumnLayout {
        SplitView.maximumWidth: root.width / 2
        SplitView.fillHeight: true
        SplitView.preferredWidth: Math.max(220, parent.width / 4)
        spacing: 0

        ColoredFrame {
            color: "#2a2e3f"
            Layout.preferredHeight: 50
            Layout.fillWidth: true
            Label {
                text: qsTr("Your rooms")
                font.pixelSize: 19
                anchors.verticalCenter: parent.verticalCenter
                font.bold: true
            }
            Label {
                text: "+"
                font.pixelSize: 30
                anchors.verticalCenter: parent.verticalCenter
                anchors.right: parent.right
                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onClicked: createRoomDialog.open()
                }
            }
        }
        ColoredFrame {
            color: "#262a39"
            Layout.fillWidth: true
            Layout.fillHeight: true
            topPadding: 0
            RoomList {
                id: roomList
                anchors.fill: parent
                model: GroupController.model
                onCurrentItemChanged: {
                    if (!roomSelected)
                        roomSelected = true
                    chatBox.room = roomList.currentItem
                }
                header:Item{
                    id:spacer
                    height:5
                    width:roomList.width
                }
            }
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
            visible: roomSelected
            implicitHeight: 50
            color: "#2a2e3f"
            Layout.fillWidth: true
            Label {
                anchors.centerIn: parent
                font.bold: true
                id: titleLabel
                text: roomList.currentItem.name
                MouseArea {
                    cursorShape: Qt.PointingHandCursor
                    anchors.fill:parent
                    onClicked: drawer.open()

                }
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
                    onClicked: CallController.handle(chatBox.room.id).join()
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
            initalMessage: qsTr("Select ChatRoom to start messaging")
            Layout.fillHeight: true
            Layout.fillWidth: true
        }
    }

    CreateRoomDialog {
        id: createRoomDialog
        anchors.centerIn: parent
        onAccepted: GroupController.create(roomName)
    }
    Drawer {
        id: drawer
        property alias model: view.model
        property int lastID: -1
        width: root.width*0.4
        padding: 20
        height: root.height
        edge: Qt.RightEdge
        y: 31 // sysbar + border
        background: Rectangle {
            color: "#262a39"
        }

        onAboutToShow: {
            if (drawer.lastID != roomList.currentItem.id) {
                drawer.model = undefined
                drawer.lastID = roomList.currentItem.id
                Future.onFinished(UserController.getGroupUsers(
                                      roomList.currentItem.id),
                                  function (users) {
                                      drawer.model = users
                                  })
            }
        }

        ColumnLayout {
            anchors.fill: parent
            RowLayout {
                Layout.fillWidth:true
                Label {
                    text: qsTr("Users in this room")
                    Layout.fillWidth: true
                    font.pixelSize: 20
                    font.bold: true

                }
                Label {
                    text: "+"
                    font.pixelSize: 35
                    Layout.alignment: Qt.AlignBottom
                    MouseArea {
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        onClicked: selectUserDialog.open()
                    }
                }
            }
            UsersView {
                id: view
                Layout.fillWidth: true
                Layout.fillHeight: true
                onUserClicked: handle => ProfileViewer.showProfile(handle)
            }
        }
    }
    SelectUserDialog {
        id: selectUserDialog
        anchors.centerIn: parent
        title: qsTr("Add memder:")
        onSeacrhPatternChanged: {
            let future = UserController.find(seacrhPattern, 5)
            Future.onFinished(future, function (users) {
                selectUserDialog.usersModel = users
            })
        }
        onUserSelected: id => GroupController.addUser(id,
                                                      roomList.currentItem.id)
    }
}
