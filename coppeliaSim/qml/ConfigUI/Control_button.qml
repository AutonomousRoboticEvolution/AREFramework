import QtQuick 2.15
import QtQuick.Controls 2.15

Button {
    id: root
    required property ConfigUI configUi
    required property string elemName
    required property var elemSchema
    property string elemValue
    text: elemValue || elemSchema.name || elemName
    onPressed: configUi.sendEvent(elemSchema.callback, elemName)
}
