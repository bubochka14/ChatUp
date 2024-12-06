import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material

Button {
    id: minButton
    leftPadding: 0
    topPadding: 0
    rightPadding: 0
    bottomPadding: 0
    leftInset: 0
    topInset: 0
    rightInset: 0
    bottomInset: 0
    height: 50
    text: "🗕"
    onClicked: window.showMinimized()
    background: Rectangle {
        color: {
            if (!root.enabled) {
                return "gray"
            }
            if (root.pressed) {
                return Qt.rgba(0, 0, 0, 0.15)
            }
            if (root.hovered) {
                return Qt.rgba(0, 0, 0, 0.15)
            }
            return "transparent"
        }
    }
}
