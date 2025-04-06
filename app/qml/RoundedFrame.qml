import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import QtQuick.Controls.Material.impl 2.12

Frame {
    id: control
    property alias color: back.color
    property int radius: 2
    background: Rectangle {
        id: back
        color: control.Material.backgroundColor
        radius: control.radius

        // layer.enabled: control.enabled && control.Material.elevation > 0
        // layer.effect: ElevationEffect {
        //     elevation: control.Material.elevation
        // }
    }
}
