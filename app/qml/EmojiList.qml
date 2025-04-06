import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Menu {
    id: root
    width: 200
    height: 300

    property var emojiString
    GridView {
        id: view
        anchors.fill: parent
        model: []
        delegate: Label {
            text: disp
        }
    }
    Component.onCompleted: {
        var xhr = new XMLHttpRequest
        xhr.open("GET", "emoji.txt")
        xhr.onreadystatechange = function () {
            if (xhr.readyState == XMLHttpRequest.DONE) {
                // use file contents as required
                let sp =xhr.responseText.split(",")
                for (let i =0; i< sp.length; i++) {
                    view.model.append({
                                          "disp": sp[i]
                                      })

                    console.log(sp)

                }
            }
        }
        xhr.send()
    }

    Repeater {
        id: emojiModel
    }
}
