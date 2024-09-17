import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material

Page {
    id: root
    signal registerUser(string username, string passw)
    signal loginUser(string username, string passw)
    property alias errorString: errorField.text
    header: TabBar {
        id: bar
        height: 60
        TabButton {
            text: qsTr("Sign up")
        }
        TabButton {
            text: qsTr("Sign in")
        }
    }
    ColumnLayout {
        anchors.fill: parent
        spacing: 50
        Label {
            id: errorField
            Layout.alignment: Qt.AlignHCenter
            color: Material.color(Material.Red)
        }
        StackLayout {
            id: layout
            Layout.fillHeight: true
            Layout.fillWidth: true
            currentIndex: bar.currentIndex
            Item {
                ColumnLayout {
                    anchors.fill: parent
                    spacing: 20
                    TextField {
                        id: loginUsernameField
                        Layout.alignment: Qt.AlignHCenter
                        placeholderText: qsTr("Username:")
                        Layout.preferredWidth: 150
                        maximumLength: 64
                    }
                    TextField {
                        id: loginPasswordField
                        Layout.alignment: Qt.AlignHCenter
                        placeholderText: qsTr("Password:")
                        echoMode: TextInput.Password
                        Layout.preferredWidth: 150
                        maximumLength: 64
                    }
                    Item {
                        Layout.fillHeight: true
                        Layout.fillWidth: true
                        Button {
                            id: loginButton
                            focus:true
                            anchors.bottom: parent.bottom
                            anchors.horizontalCenter: parent.horizontalCenter
                            anchors.bottomMargin: 10
                            text: qsTr("Sign In")
                            width: 200
                            height: 50
                            Layout.alignment: Qt.AlignBottom | Qt.AlignHCenter
                            onClicked: {
                                root.loginUser(loginUsernameField.text,
                                               loginPasswordField.text)
                            }
                        }
                    }
                }
            }
            Item {
                ColumnLayout {
                    anchors.fill: parent
                    spacing: 20
                    TextField {
                        id: signUpUsernameField
                        Layout.alignment: Qt.AlignHCenter
                        placeholderText: qsTr("Username:")
                        Layout.preferredWidth: 150
                        maximumLength: 64
                    }
                    TextField {
                        id: signUpPasswordField
                        Layout.alignment: Qt.AlignHCenter
                        placeholderText: qsTr("Password:")
                        echoMode: TextInput.Password
                        Layout.preferredWidth: 150
                        maximumLength: 64
                    }
                    TextField {
                        id: signUpRepeatPasswordField
                        Layout.alignment: Qt.AlignHCenter
                        placeholderText: qsTr("Repeat password:")
                        echoMode: TextInput.Password
                        Layout.preferredWidth: 150
                        maximumLength: 64
                    }
                    Item {
                        Layout.fillHeight: true
                        Layout.fillWidth: true
                        Button {
                            id: signUpButton
                            focus:true
                            anchors.bottom: parent.bottom
                            anchors.horizontalCenter: parent.horizontalCenter
                            anchors.bottomMargin: 10
                            text: qsTr("Sign Up")
                            width: 200
                            height: 50
                            onClicked: {
                                if (signUpPasswordField.text == signUpRepeatPasswordField.text) {
                                    root.registerUser(signUpUsernameField.text,
                                                      signUpPasswordField.text)
                                } else
                                    setError(qsTr("Password missmatch"))
                            }
                        }
                    }
                }
            }
        }
    }
}
