import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Page {
    id: root
    property alias progress : progressBar.value
    property alias status : status.text
    ColumnLayout {
        anchors.topMargin: 50
        spacing:30
        anchors.top: parent.top
        anchors.horizontalCenter: parent.horizontalCenter
        AnimatedImage {
            id: animation

            source: Qt.resolvedUrl("resources/gif/duck.gif")
            Layout.preferredHeight: 205
            Layout.preferredWidth: 144
            Layout.alignment: Qt.AlignHCenter
        }
        Label {
            id: status
            font.pixelSize: 22
            Layout.alignment: Qt.AlignHCenter
        }
        ProgressBar
        {
            id:progressBar
            Layout.alignment: Qt.AlignHCenter
        }
    }
}
