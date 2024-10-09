import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Dialog {
    id: root
    property alias seacrhPattern: searchField.text
    property alias usersModel : view.model
    signal userSelected(var id)
    height:400
    width:250
    standardButtons:  Dialog.Cancel
    ColumnLayout {
        anchors.fill: parent
        TextField {
            id: searchField
            Layout.fillWidth: true
            placeholderText: qsTr("Search")
        }
        UsersView {
            id: view
            onUserClicked: id => {root.userSelected(id); root.accept()}
            Layout.fillHeight: true
            Layout.fillWidth: true
        }
    }
}
