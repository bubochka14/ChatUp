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
            Row {
                Column {
                    spacing: 5
                    Label {
                        text: qsTr("Input Device")
                    }
                    ComboBox {
                        id: audioInputComboBox
                        width: 200
                        model: MicrophonePipeline.availableDevices
                        onActivated: ApplicationSettings.audioDevice = currentText
                        implicitContentWidthPolicy: ComboBox.WidestText
                        Component.onCompleted: {
                            let idx = find(ApplicationSettings.audioDevice)
                            if (idx != -1)
                                currentIndex = idx
                        }
                    }
                    Column {
                        Label {
                            text: qsTr("Output Device")
                        }
                        ComboBox {
                            id: audioOutputComboBox
                            width: 200
                            model: MyAudioOutput.availableDevices
                            onActivated: ApplicationSettings.outputAudioDevice = currentText
                            implicitContentWidthPolicy: ComboBox.WidestText
                            Component.onCompleted: {
                                let idx = find(
                                        ApplicationSettings.outputAudioDevice)
                                if (idx != -1)
                                    currentIndex = idx
                            }
                        }
                    }
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
                        let idx = find(ApplicationSettings.videoDevice)
                        if (idx != -1)
                            currentIndex = idx
                    }
                }
            }
        }
    }
}
