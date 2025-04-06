import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import ChatClient.Core
import QuickFuture
import ChatClient.Network
import QWindowKit 1.0

ApplicationWindow {
    id: root
    width: 650
    height: 600
    minimumWidth: 650
    minimumHeight: 600
    visible: false
    signal languageSet(string language)
    signal logout
    color: "transparent"
    WindowAgent {
        id: windowAgent
    }
    background: Item {}
    ColumnLayout {
        anchors.fill: parent
        spacing: 0
        TitleBar {
            id: titleBar
            Layout.fillWidth: true
            agent: windowAgent
            window: root
            Layout.preferredHeight: 32
        }
        RowLayout {
            id: mainLayout
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.minimumWidth: 650
            spacing: 0
            MenuPanel {
                Layout.fillHeight: true
                width: 66
                onLogoutClicked: root.logout()
                onProfileClicked: {

                    ProfileViewer.showProfile(CurrentUser)
                }
                onRoomsClicked: stack.currentIndex = 0
                onDirectClicked: stack.currentIndex = 1
                onSettingsClicked: stack.currentIndex = 2
            }
            StackLayout {
                id: stack
                RoomsPage {
                    id: roomsPage
                }
                Page {
                    Label {
                        anchors.centerIn: parent
                        text: "Coming soon"
                    }
                }
                SettingsPage
                {

                }
            }
        }
    }
    ProfileDialog {
        id: profileDialog
        anchors.fill: parent
    }
    Rectangle
    {
        anchors.fill:parent
        z:-1000
        color: Material.background

    }

    Component.onCompleted: {
        ProfileViewer.view =profileDialog
        windowAgent.setup(root)
        windowAgent.setTitleBar(titleBar)
    }
}
