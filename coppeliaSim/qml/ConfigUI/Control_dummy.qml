import QtQuick 2.15
import QtQuick.Controls 2.15

Rectangle {
    id: root
    implicitWidth: txt.implicitWidth
    implicitHeight: txt.implicitHeight
    color: 'yellow'
    required property string elemName
    required property var elemSchema
    property var elemValue

    Text {
        id: txt
        text: `tab["${elemSchema.ui.tab || "Main"}"].group[${elemSchema.ui.group || 0}].col[${elemSchema.ui.col || 0}].elem[order=${elemSchema.ui.order || 0}]`
    }
}
