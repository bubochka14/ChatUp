import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material

Frame {
    id:root
    property alias color: back.color
    property alias border :back.border
    background: Rectangle
    {
        id: back
        color: Material.background
        border.width: 1
        border.color: "#7451f7"
    }
}
