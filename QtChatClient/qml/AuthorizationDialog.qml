import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root
    anchors.fill: parent
    signal registerUser(string username, string passw)
    signal loginUser(string username, string passw)
    Component.onCompleted: {
        setLoadingScreen(false);
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
            anchors.top: bar.bottom
            anchors.topMargin: 10
            font.family: "Helvetica"
            font.pointSize: 10
            color: "red"
            anchors.horizontalCenter: bar.horizontalCenter
        }
        StackLayout {
            id: layout
            y: bar.contentHeight
            anchors.fill: parent
            currentIndex: bar.currentIndex
            Item {
                id: signUpPage

                Column {
                    id: signUpFields
                    spacing: 5
                    anchors.centerIn: parent
                    Label {
                        id: signUpUsernameLabel
                        text: qsTr("Username:")
                    }
                    TextField {
                        id: signUpUsernameField
                        maximumLength: 255
                    }
                    Label {
                        id: signUpPasswordLabel
                        text: qsTr("Password:")
                    }
                    TextField {
                        id: signUpPasswordField
                        echoMode: TextInput.Password
                        maximumLength: 255
                    }
                    Label {
                        id: signUpRepeatPasswordLabel
                        text: qsTr("Repeat password:")
                    }
                    TextField {
                        id: signUpRepeatPasswordField
                        echoMode: TextInput.Password
                        maximumLength: 255
                    }
                }
                Button {
                    id: signUpButton
                    anchors.bottom: parent.bottom
                    anchors.bottomMargin: 10
                    anchors.horizontalCenter: parent.horizontalCenter
                    implicitWidth: parent.width - 40
                    text: qsTr("Sign Up")
                    onClicked: {
                        if (signUpPasswordField.text == signUpRepeatPasswordField.text) {
                            root.registerUser(signUpUsernameField.text, signUpPasswordField.text);
                        } else
                            setError(qsTr("Password missmatch!"));
                    }
                }
            }
            Item {
                id: loginPage
                Column {
                    id: loginFields
                    spacing: 5
                    anchors.centerIn: parent
                    Label {
                        id: loginUsernameLabel
                        text: qsTr("Username:")
                    }
                    TextField {
                        id: loginUsernameField
                        maximumLength: 255
                    }
                    Label {
                        id: loginPasswordLabel
                        text: qsTr("Password:")
                    }
                    TextField {
                        id: loginPasswordField
                        echoMode: TextInput.Password
                        maximumLength: 255
                    }
                }
                Button {
                    id: loginButton
                    anchors.bottom: parent.bottom
                    anchors.bottomMargin: 10
                    anchors.horizontalCenter: parent.horizontalCenter
                    implicitWidth: parent.width - 40
                    text: qsTr("Sign In")
                    onClicked: {
                        root.loginUser(loginUsernameField.text, loginPasswordField.text);
                    }
                }
            }
        }
    }
    Item {
        id: loadingScreen
        anchors.fill: parent
        Rectangle {
            color: "lightgreen"
            anchors.fill: parent
            Text {
                text: qsTr("CONNECTING")
                font.pixelSize: 45
                color: "white"
                anchors.centerIn: parent
                RotationAnimator on rotation  {
                    running: true
                    loops: Animation.Infinite
                    from: 0
                    to: 360
                    duration: 2500
                }
            }
        }
    }
    function setError(error: string) {
        errorField.text = error;
    }
    function close() {
        Qt.callLater(Qt.quit);
    }
    function setLoadingScreen(st: bool) {
        inputScreen.enabled = !st;
        inputScreen.visible = !st;
        loadingScreen.visible = st;
        loadingScreen.enabled = st;
    }
}
