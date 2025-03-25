import QtQuick
Image {
    id: root
    property alias iconSource : root.source
    signal clicked()
    sourceSize.width : width
    sourceSize.height : height

    MouseArea {
        anchors.fill: parent
        onClicked: root.clicked()
        cursorShape: Qt.PointingHandCursor
        preventStealing:true
    }
}
