import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.12

ColumnLayout {
    id: root
    required property string elemName
    required property var elemSchema
    property var elemValue
    spacing: 0

    Repeater {
        model: elemSchema.choices

        RadioButton {
            required property int index
            required property var modelData
            text: (elemSchema.labels ? elemSchema.labels[index] : modelData) || modelData

            checked: root.elemValue === modelData
            onCheckedChanged: {
                if(checked && root.elemValue !== modelData)
                    root.elemValue = modelData
            }
        }
    }
}
