import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item
{
	id: root
	implicitHeight: inputBox.height
	signal messageEntered(string chatMessage)
	RowLayout
	{
		width: root.width
		height: inputBox.height
		spacing : 0
		InputBox
		{
			id: inputBox
			Layout.fillWidth: true
			backgroundColor : "lightgreen"
		}
		Rectangle
		{
			Layout.fillHeight: true
			color : "lightgreen"
			width:70
			Button
			{
				anchors.top :parent.top
				anchors.left :parent.left
				anchors.topMargin:10
				anchors.leftMargin:15

				text : qsTr("Send")
				onClicked : messageEntered(inputBox.inputText)
			}
		}

	}
}