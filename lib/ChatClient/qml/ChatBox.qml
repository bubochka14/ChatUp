import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material
import ChatClient

Pane {
    Material.background: "#39423B"
    id: root
    property MessageModel model
    function showStartMessage(st) {
        startMessage.visible = st
    }

    Label {
        Material.foreground: Material.Grey
        id: startMessage
        anchors.centerIn: parent
        font.pixelSize: 16
        text: qsTr("Select ChatRoom to start messaging")
    }

    ListView {
        anchors.fill: parent
        id: listView
        model: root.model
        spacing: 15
        ScrollBar.vertical: ScrollBar {
            id: bar
            active: true
        }
        onCountChanged: {
            bar.setPosition(1)
        }

        delegate: Item {
            id: fr
            height: messageBox.height
            property var side
            width: listView.width

            RowLayout {
                id: messageBox
                anchors.top: parent.top
                width: parent.width * 0.8
                spacing: 10
                TextEdit {
                    id: userLabel
                    readOnly: true
                    Material.foreground: "#2c6337"
                    selectByMouse: true
                    text: user.name + ":"
                    Layout.alignment: Qt.AlignBottom

                    font.pointSize: 13
                }
                Item {
                    id: messageFrame

                    Layout.preferredHeight: messageBody.contentHeight
                                            + messageTime.contentHeight + 16
                    Layout.preferredWidth: parent.width
                    Column {
                        id: col
                        anchors.fill: parent
                        anchors.margins: 8
                        width: parent.width
                        TextEdit {
                            Material.foreground: "Grey"
                            id: messageBody
                            text: body
                            width: parent.width
                            selectByMouse: true
                            readOnly: true
                            font.pointSize: 10
                            font.hintingPreference: Font.PreferNoHinting
                            wrapMode: TextEdit.Wrap
                        }
                        TextEdit {
                            Material.foreground: "Grey"

                            id: messageTime
                            selectByMouse: true
                            readOnly: true
                            text: time.toLocaleString(
                                      Qt.locale(Qt.uiLanguage),
                                      "dddd MM-dd hh:mm")
                            font.pointSize: 6
                        }
                    }

                    RoundedFrame {
                        anchors.fill: messageFrame
                        rightInset: parent.width - Math.max(
                                        messageBody.contentWidth,
                                        messageTime.contentWidth) - 20
                        radius: 15
                        z: -1
                        Material.elevation: 50
                        Material.background: "DarkGrey"
                    }
                }
                Item {
                    id: spacer
                    Layout.fillWidth: true
                }
            }
        }
    }
}
