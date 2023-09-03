import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Pane {
    id: root
    anchors.fill: parent
    signal registerUser(string username, string passw)
    signal loginUser(string username, string passw)
    Component.onCompleted: {
        setLoadingScreen(false)
    }
    Item {
        id: inputScreen
        anchors.fill: parent
        TabBar {
            id: bar
            width: parent.width
            TabButton {
                text: qsTr("Sign up")
            }
            TabButton {
                text: qsTr("Sign in")
            }
        }
        Text {
            id: errorField
            y: bar.contentHeight + 5
            color: Material.color(Material.Red)
            anchors.horizontalCenter: bar.horizontalCenter
        }

        StackLayout {
            id: layout
            currentIndex: bar.currentIndex
            anchors.horizontalCenter: bar.horizontalCenter
            anchors.bottom: parent.bottom
            anchors.top: errorField.bottom
            anchors.topMargin: 8
            Item {
                id: signUpPage
                Column {
                    anchors.horizontalCenter: parent.horizontalCenter
                    spacing:15
                    TextField {
                        id: signUpUsernameField
                        placeholderText : qsTr("Username:")
                        maximumLength: 255
                    }
                    TextField {
                        id: signUpPasswordField
                        placeholderText : qsTr("Password:")
                        echoMode: TextInput.Password
                        maximumLength: 255
                    }
                    TextField {
                        id: signUpRepeatPasswordField
                        placeholderText : qsTr("Repeat password:")
                        echoMode: TextInput.Password
                        maximumLength: 255
                    }
                }
                Button {
                    id: signUpButton
                    anchors.bottom: parent.bottom
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: qsTr("Sign Up")
                    width:200
                    Material.elevation: 6
                    onClicked: {
                        if (signUpPasswordField.text == signUpRepeatPasswordField.text) {
                            root.registerUser(signUpUsernameField.text,
                                              signUpPasswordField.text)
                        } else
                            setError(qsTr("Password missmatch!"))
                    }
                }
            }
            Item {
                id: loginPage
                Layout.alignment: Qt.AlignVCenter
                Column {
                    anchors.horizontalCenter: parent.horizontalCenter
                    spacing: 20

                    TextField {
                        id: loginUsernameField
                        placeholderText : qsTr("Username:")

                        maximumLength: 255
                    }
                    TextField {
                        id: loginPasswordField
                        placeholderText : qsTr("Password:")
                        echoMode: TextInput.Password
                        maximumLength: 255
                    }
                }
                Button {
                    id: loginButton
                    text: qsTr("Sign In")
                    width: 200
                    Material.elevation: 6
                    anchors.bottom: parent.bottom
                    anchors.horizontalCenter: parent.horizontalCenter
                    onClicked: {
                        root.loginUser(loginUsernameField.text,
                                       loginPasswordField.text)
                    }
                }
            }
        }
    }

    Pane {
        id: loadingScreen
        anchors.fill: parent
        Text {

            text: qsTr("CONNECTING")
            font.pixelSize: 45
            color: Material.color(Material.Grey)
            anchors.centerIn: parent
            RotationAnimator on rotation {
                running: true
                loops: Animation.Infinite
                from: 0
                to: 360
                duration: 2500
            }
        }
    }
    function setError(error) {
        errorField.text = error
    }
    function close() {
        Qt.callLater(Qt.quit)
    }
    function setLoadingScreen(st) {
        inputScreen.enabled = !st
        inputScreen.visible = !st
        loadingScreen.visible = st
        loadingScreen.enabled = st
    }
}
