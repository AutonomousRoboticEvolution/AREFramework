import QtQuick 2.15
import QtQuick.Controls 2.15

TextField {
    id: root
    required property string elemName
    required property var elemSchema
    property string elemValue
    text: elemValue
    onTextChanged: elemValue = text
}
