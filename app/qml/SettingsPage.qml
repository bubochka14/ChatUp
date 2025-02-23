import QtCore
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material
import ChatClient.Core

ColoredFrame {
    id: root
    padding: 30
    topPadding: 50
    ColumnLayout {
        anchors {
            right: parent.right
            left: parent.left
        }

        GroupBox {
            title: qsTr("Audio")
            Layout.fillWidth: true
            Column {
                spacing: 5
                Label {
                    text: qsTr("Input Device")
                }
                ComboBox {
                    id: audioInputComboBox
                    width: 200
                    model: LocalStreamSource.availableAudioDevices
                    implicitContentWidthPolicy: ComboBox.WidestText
                }
            }
        }
        GroupBox {
            title: qsTr("Video")
            Layout.fillWidth: true
            Column {
                spacing: 5
                Label {
                    text: qsTr("Input Device")
                }
                ComboBox {
                    id: videoInputComboBox
                    width: 200
                    model: CameraPipeline.availableDevices
                    onActivated: ApplicationSettings.videoDevice = currentText
                    implicitContentWidthPolicy: ComboBox.WidestText
                    Component.onCompleted: {
                        let idx =find(ApplicationSettings.videoDevice)
                        if(idx!=-1)
                            currentIndex = idx
                    }
                }
            }
        }
    }
}
