import QtQuick

TextEdit
{
    id: root
    readOnly: true
    selectByMouse: true
    text: user.name + ":"
    font.hintingPreference: Font.PreferNoHinting
//    font.pointSize: 13
}
