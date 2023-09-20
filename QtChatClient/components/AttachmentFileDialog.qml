import QtQuick
import QtQuick.Dialogs

FileDialog {
    id: root
    fileMode: FileDialog.OpenFiles
    title: qsTr("Please choose a file")
    onAccepted: {
        console.log("User attachments: " + selectedFiles)
    }
    onRejected: {
        console.log("Canceled")
    }
    Component.onCompleted: visible = false
}
