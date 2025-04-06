import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import ChatClient.Network
import ChatClient.Core

Rectangle {
    id: root
    signal profileClicked
    signal logoutClicked
    signal directClicked
    signal roomsClicked
    signal settingsClicked
    gradient: Gradient {
        GradientStop {
            position: 0
            color: "#6058fb"
        }
        GradientStop {
            position: 1
            color: "#6482f0"
        }
    }
    ColumnLayout {
        id: layout
        spacing: 10
        anchors.fill: parent
        MenuPanelItem {
            Layout.topMargin: 20
            name: qsTr("Rooms")
            iconSource: Qt.resolvedUrl("pics/users.svg")
            onClicked: root.roomsClicked()
        }
        MenuPanelItem {
            name: qsTr("Direct")
            iconSource: Qt.resolvedUrl("pics/direct.svg")
            onClicked: root.directClicked()
        }
        MenuPanelItem {
            name: qsTr("Settings")
            iconSource: Qt.resolvedUrl("pics/settings.svg")
            onClicked: root.settingsClicked()
        }
        Item {
            id: spacer
            Layout.fillWidth: true
            Layout.fillHeight: true
        }
        MenuPanelItem {
            name: qsTr("Profile")
            iconSource: "pics/profile.svg"
            onClicked: root.profileClicked()

        }
        MenuPanelItem {
            Layout.bottomMargin: 20
            name: qsTr("Logout")
            iconSource: "pics/logout.svg"
            onClicked: root.logoutClicked()
        }
    }
}
