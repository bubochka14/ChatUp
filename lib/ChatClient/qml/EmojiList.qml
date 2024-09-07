import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
Menu
{
    id:root
    width:200
    height:300
    property var emojiString
    GridView
    {

        id:view
        anchors.fill: parent
        //model:Qt.resolvedUrl("emojies/emojies.txt")
        model:"123"
        delegate:
        Item{
        }
    }
    Repeater
    {
        id:emojiModel

    }
}
