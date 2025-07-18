import QtQuick 2.15
import QtQuick.Controls 2.15

CheckBox {
    id: root
    required property string elemName
    required property var elemSchema
    property bool elemValue
    text: elemSchema.name
    checked: elemValue
    onCheckedChanged: elemValue = checked
}
