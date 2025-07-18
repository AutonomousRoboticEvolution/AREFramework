import QtQuick 2.15
import QtQuick.Controls 2.15

Slider {
    id: root
    required property string elemName
    required property var elemSchema
    property real elemValue

    from: elemSchema.minimum || 0
    to: elemSchema.maximum || 1
    value: elemValue
    stepSize: elemSchema.step || (to - from) / 1000
    onValueChanged: elemValue = value
}
