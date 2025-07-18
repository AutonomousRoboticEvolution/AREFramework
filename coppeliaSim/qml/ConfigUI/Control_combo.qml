import QtQuick 2.15
import QtQuick.Controls 2.15

ComboBox {
    id: root
    required property string elemName
    required property var elemSchema
    property var elemValue

    model: {
        var ret = []
        for(var i = 0; i < elemSchema.choices.length; i++) {
            var k = elemSchema.choices[i]
            var v = elemSchema.labels ? elemSchema.labels[i] : k
            ret.push({key: k, value: v})
        }
        return ret
    }

    textRole: 'value'

    currentIndex: {
        for(var i = 0; i < model.length; i++)
            if(model[i].key === elemValue)
                return i
        return -1
    }
    onCurrentIndexChanged: {
        var newVal = model[currentIndex].key
        if(newVal !== elemValue) elemValue = newVal
    }
}
