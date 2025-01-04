import QtQuick

Rectangle {
    id:root
    color: "black"
    property var boxHeight
    property var boxWidth
    property alias model: view.model
    onModelChanged: console.log("change",model)
    GridView {
        id: view
        property var spacing: 15
        anchors.fill: parent
        property var baseWidth : 150
        property var baseHeight : 100
        // Component.onCompleted:resetSize()
        cellWidth: baseWidth
        cellHeight: baseHeight
        Component.onCompleted :console.log("Model",root.model)
        delegate:RoundedFrame
        {
            height: GridView.view.cellHeight - view.spacing
            width: GridView.view.cellWidth - view.spacing

            radius: 15
        }
        // onModelChanged: resetSize()
        // function resetSize()
        // {
        //     view.width = Math.min(parent.width/baseWidth, root.model.rowCount()) * idealWidth
        //     view.height = Math.min(parent.height/baseHeight, root.model.rowCount()) * idealHeight

        // }
    }
}
