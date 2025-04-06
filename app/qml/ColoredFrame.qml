import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material

Pane {
    id:root
    property alias color: back.color
    property alias border :back.border
    background: Rectangle
    {
        id: back
        color: Material.background
    }
}
