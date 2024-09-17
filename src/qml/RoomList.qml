import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material

ColoredFrame {
    id: root
    property int selectedRoomID: -1
    property int selectedRoomIndex: -1
    property alias roomIndex: listView.currentIndex
    property alias roomModel: listView.model
    property alias listView: listView
    ListView {
        id: listView
        currentIndex: -1
        height: parent.height
        width: parent.width
        delegate: RoomDelegate {
            id: delegate
            required property int index
            width: listView.width
            onClicked:{
                selectedRoomID = delegate.id
                selectedRoomIndex = index
                listView.currentIndex = index
            }
        }
    }
}
