import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material
ColoredFrame {
    id:root
    padding: 30
    topPadding:50
    ColumnLayout
    {
        anchors{
            right:parent.right
            left:parent.left
        }

        GroupBox
        {
            title: qsTr("Audio")
            Layout.fillWidth: true
            Column
            {
                spacing:5
                Label
                {
                    text: qsTr("Input Device")
                }
                ComboBox
                {
                    width: 200
                    model: LocalStreamSource.availableAudioDevices
                    implicitContentWidthPolicy:ComboBox.WidestText
                }
            }
        }
        GroupBox
        {
            title: qsTr("Video")
            Layout.fillWidth: true
            Column
            {
                spacing:5
                Label
                {
                    text: qsTr("Input Device")
                }
                ComboBox
                {
                    width: 200
                    model: CameraPipeline.availableDevices
                    implicitContentWidthPolicy:ComboBox.WidestText
                }
                Component.onCompleted : console.log(CameraPipeline)
            }
        }
    }
}
