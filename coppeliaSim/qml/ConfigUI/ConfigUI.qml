import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.12

Rectangle {
    id: root
    implicitWidth: topLayout.implicitWidth
    implicitHeight: topLayout.implicitHeight
    color: systemPalette.window

    SystemPalette {
        id: systemPalette
        colorGroup: SystemPalette.Active
    }

    property var config: ({})
    property var schema: ({})
    property var schemaSorted: {
        var ss = []
        for(var elemName in schema) {
            var elemSchema = schema[elemName]
            var ui = elemSchema.ui || {}
            var tab = ui.tab || "Main"
            var group = ui.group || 0
            var col = ui.col || 0
            var order = ui.order || 0
            ss.push([tab, group, col, order, elemName, elemSchema])
        }
        ss.sort()
        return ss
    }

    signal updateConfig(changedKey: string, value: var)

    function setConfig(kv) {
        const [k, v] = kv;
        config[k] = v
        updateConfig(k, v)
    }

    function setSchema(s) {
        schema = s
        selectedTab = tabs()[0] || "Main"
    }

    function setConfigAndSchema(o) {
        for(var k of Object.keys(o.config))
            setConfig([k, o.config[k]])
        setSchema(o.schema)
    }

    function tabs() {
        var s = new Set()
        var ss = []
        for(var elemName in schema) {
            var elemSchema = schema[elemName]
            var ui = elemSchema.ui || {}
            var tab = ui.tab || "Main"
            var order = ui.order || 0
            ss.push([order, tab])
        }
        ss.sort()
        for(var o of ss)
            s.add(o[1])
        return Array.from(s)
    }

    function groups(tab) {
        var s = new Set()
        for(var o of schemaSorted)
            if(o[0] === tab)
                s.add(o[1])
        return Array.from(s)
    }

    function cols(tab, group) {
        var s = new Set()
        for(var o of schemaSorted)
            if(o[0] === tab && o[1] === group)
                s.add(o[2])
        return Array.from(s)
    }

    function elems(tab, group, col) {
        var s = []
        for(var o of schemaSorted)
            if(o[0] === tab && o[1] === group && o[2] === col)
                s.push([o[4], o[5]])
        return s
    }

    property string selectedTab: "Main"
    onSelectedTabChanged: tabBar.currentIndex = tabs().indexOf(root.selectedTab)

    ColumnLayout {
        id: topLayout

        TabBar {
            id: tabBar
            Layout.fillWidth: true
            visible: tabButtonsRepeater.model.length > 1
            Repeater {
                id: tabButtonsRepeater
                model: tabs()
                TabButton {
                    text: modelData
                    onClicked: root.selectedTab = modelData
                    width: implicitWidth
                }
            }
        }

        Frame {
            background: Item { }
            padding: 5

            ColumnLayout {
                id: groupsLayout
                spacing: 15
                anchors.centerIn: parent

                Repeater {
                    id: groupsRepeater
                    model: groups(root.selectedTab)

                    RowLayout {
                        id: colsLayout
                        spacing: 5

                        Repeater {
                            id: colsRepeater
                            readonly property string tab: root.selectedTab
                            readonly property int group: modelData
                            model: cols(tab, group)

                            ColumnLayout {
                                id: elemsLayout
                                spacing: 5
                                required property var modelData

                                Repeater {
                                    id: elemsRepeater
                                    readonly property string tab: colsRepeater.tab
                                    readonly property int group: colsRepeater.group
                                    readonly property int col: modelData
                                    model: elems(tab, group, col)

                                    ColumnLayout {
                                        id: elemLayout
                                        required property var modelData
                                        readonly property string tab: elemsRepeater.tab || 'Main'
                                        readonly property int group: elemsRepeater.group || 0
                                        readonly property int col: elemsRepeater.col || 0
                                        readonly property var elemSchema: modelData[1]
                                        readonly property string elemName: elemSchema.key || modelData[0]
                                        readonly property int order: elemSchema.ui.order || 0

                                        Label {
                                            Layout.fillWidth: true
                                            visible: elemSchema.ui.control !== 'checkbox' && elemSchema.ui.control !== 'button'
                                            text: `${elemSchema.name || elemName}:`
                                        }

                                        Loader {
                                            id: loader
                                        }

                                        Connections {
                                            id: uiChangedConnection
                                            target: loader.item
                                            function onElemValueChanged() {
                                                root.config[loader.item.elemName] = loader.item.elemValue
                                                simBridge.sendEvent('ConfigUI_uiElemChanged', [loader.item.elemName, loader.item.elemValue])
                                            }
                                        }

                                        Connections {
                                            id: updateConfigConnection
                                            target: root
                                            function onUpdateConfig(k, v) {
                                                if(k !== loader.item.elemName) return
                                                uiChangedConnection.enabled = false
                                                loader.item.elemValue = v
                                                uiChangedConnection.enabled = true
                                            }
                                        }

                                        Component.onCompleted: {
                                            loader.setSource(`Control_${elemSchema.ui.control || 'dummy'}.qml`, {
                                                configUi: root,
                                                elemName: elemLayout.elemName,
                                                elemSchema: elemLayout.elemSchema,
                                                elemValue: config[elemName],
                                            })
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    // interface for controls:

    function sendEvent(n, o) {
        if(simBridge)
            simBridge.sendEvent(n, o)
    }

    property var simBridge
}
