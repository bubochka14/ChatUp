import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
Item
{
    id : root
    property int contentHeight
    property color backgroundColor
    property var inputText : textEdit.text
    implicitHeight  : box.height
    Rectangle
    {
        id: box
        width: root.width
        color: backgroundColor
        height:textEdit.height +30
        Rectangle
        {
            id: cloud
            radius:10
            x:10
            y:5

            width: box.width -20
            height: box.height -10
            TextEdit
            {
                id: textEdit
                anchors.centerIn :parent
                width: cloud.width-6
                wrapMode: TextEdit.Wrap
            }
        }
    }
}
