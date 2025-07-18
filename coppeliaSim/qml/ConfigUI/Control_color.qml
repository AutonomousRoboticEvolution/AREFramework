import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.12
import QtQuick.Dialogs 1.2

RowLayout {
    id: root
    required property string elemName
    required property var elemSchema
    property var elemValue

    onElemValueChanged: rectColor.color = Qt.rgba(elemValue[0], elemValue[1], elemValue[2], 1)

    Rectangle {
        id: rectColor
        width: button.height
        height: width
        border.color: 'black'
    }

    Button {
        id: button
        text: "..."
        onClicked: colorDialog.open()
    }

    ColorDialog {
        id: colorDialog
        color: rectColor.color
        onAccepted: {
            elemValue = [currentColor.r, currentColor.g, currentColor.b]
        }
    }
}
