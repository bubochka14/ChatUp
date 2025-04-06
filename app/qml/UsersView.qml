import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs
import ChatClient.Core
import QtQuick.Controls.Material

Item {
    id: root
    signal userClicked(UserHandle handle)
    property alias model: proxy.model
    DelegateModel {
        id: proxy
        property var lessThan: [function (left, right) {
            return left.status < right.status
        }]
        items.includeByDefault: false
        filterOnGroup: "all"

        groups: [
            DelegateModelGroup {
                id: online
                name: "online"
                onChanged: (removed, inserted) => {
                               console.log("ONLINE DATA",removed,inserted,inserted[0],removed[0])
                               // allGr.move(inserted[0].index, allGr.get(0).itemsIndex)
                           }
            },
            DelegateModelGroup {
                id: allGr
                name: "all"
                includeByDefault: true
                onChanged: (removed, inserted) => {
                               if(inserted[0])
                               for (var i = inserted[0].index; i < inserted[0].count; i++) {
                                   let item = get(i)
                                   if (item.model.status == 1) {
                                       addGroups(i, 1, "online")
                                       move(i,0);
                                   }
                               }
                           }
            }
        ]
        delegate: StandardDelegate {
            required property var handle
            required property string name
            required property var status
            required property string tag


            id: delegate
            width: root.width
            height: 40
            onClicked: root.userClicked(handle)
            label.text: name
            label.font.pointSize: 12
            subLabel.font.pointSize:10
            subLabel.color:"gray"
            subLabel.text: "@"+tag
//            subLabel.color: "#6482f0"
            icon: Image {
                id: icon
                source: "pics/profile.svg"
                sourceSize.height: 30
                sourceSize.width: 30
                Rectangle{
                    width:15
                    height:15
                    color:Material.background
                    visible: status ==1
                    radius:15
                    anchors{
                        bottom:parent.bottom
                        right:parent.right
                        rightMargin:-2
                        bottomMargin:-2
                    }

                    Rectangle
                    {
                        height:10
                        width:10
                        radius:10
                        anchors.centerIn:parent
                        color:"#6482f0"
                    }
                }
            }
        }
    }

    ListView {
        id: view
        boundsBehavior: Flickable.StopAtBounds
        clip: true
        anchors.fill: parent
        model: proxy
        spacing:4
    }
}
