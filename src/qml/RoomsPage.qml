import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material
import ChatClient
import QuickFuture


RowLayout {
    id: root
    spacing: 0
    required property AbstractChatController controller
    property bool roomSelected: false
    RoomList {
        id: roomList
        // Layout.minimumWidth: 120
        Layout.fillHeight: true
        Layout.preferredWidth: Math.max(220, parent.width / 4)
        Layout.maximumWidth: 420
        roomModel: controller.userRooms
        onSelectedRoomIDChanged: {
            if(!roomSelected)
                roomSelected= true
            var roomsModel = controller.userRooms
            chatBox.setRoomId(roomList.selectedRoomID)
            roomHeader.title = roomsModel.data(roomModel.idToIndex(roomList.selectedRoomID),RoomModel.NameRole)
        }
        listView.footer: RoomDelegate{
            id:createRoomFooter
            name: qsTr("Create")
            icon: Rectangle {
                height: 50
                width: 50
                radius: 20
                color: "#19182a"
                Label {
                    text: "+"
                    color:"white"
                    font.pixelSize: 40
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.bottom: parent.bottom
                    anchors.bottomMargin: 3
                }
            }
            width:  roomList.listView.width
            onClicked: createRoomDialog.open()
        }

    }
    ColumnLayout {
        id: chatColumn
        spacing: 0
        Layout.fillWidth: true
        // Layout.minimumWidth: 350
        ColoredFrame
        {
            id: roomHeader
            property alias title : titleLabel.text
            visible: roomSelected
            implicitHeight:50
            Layout.fillWidth: true
            Label
            {
                anchors.centerIn: parent
                font.bold: true
                id: titleLabel
            }
            MouseArea {
                id: addUserBtn
                height:30
                width: 30
                anchors.verticalCenter: parent.verticalCenter
                anchors.right: parent.right
                anchors.rightMargin: 15
                cursorShape: Qt.PointingHandCursor
                Image {
                    id: addUserIcon
                    anchors.centerIn: parent
                    height:25
                    width: 25
                    mipmap: true
                    source: Qt.resolvedUrl("pics/add-user.svg")
                }
            }
        }

        ChatBox {
            id: chatBox
            focus: true
            initalMessage: qsTr("Select ChatRoom to start messaging")
            controller: root.controller
            Layout.fillHeight: true
            Layout.fillWidth: true
        }
    }
    CreateRoomDialog
    {
        anchors.centerIn: parent
        id: createRoomDialog
        onAccepted: controller.createRoom(roomName)
    }
}
