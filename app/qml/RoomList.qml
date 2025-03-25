import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material

ColoredFrame {
    id: root
    property alias selectedRoom : listView.currentItem
    property alias selectedIndex: listView.currentIndex
    property alias roomModel: listView.model
    property alias listView: listView
    signal voiceRoomSelected
    ListView {
        id: listView
        currentIndex: -1
        boundsBehavior: Flickable.StopAtBounds
        clip: true
        spacing: 0
        height: parent.height
        width: parent.width
        delegate: ColumnLayout {
            id: delegate
            width: listView.width
            spacing: 5
            required property int index
            required property var name
            required property int id
            StandardDelegate {
                id: roomInfo
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignLeft
                Layout.minimumHeight: 70
                icon: Rectangle {
                    height: 50
                    width: 50
                    radius: 20
                    color: "#19182a"
                    Image {
                        id: icon
                        anchors.centerIn: parent
                        source: "pics/users.svg"
                        sourceSize.height: 30
                        sourceSize.width: 30
                    }
                }
                label.text: delegate.name
                label.font.bold: true
                onClicked: {
                    listView.currentIndex = index
                }
            }
        }
    }
}
