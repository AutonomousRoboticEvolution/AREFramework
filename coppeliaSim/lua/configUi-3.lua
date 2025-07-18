ConfigUI = {}

function ConfigUI:validateElemSchema(elemName, elemSchema)
    -- try to fix what is possible to fix:
    --   - infer missing information
    --   - migrate deprecated notations to current
    -- anything else -> error()

    elemSchema.key = elemSchema.key or elemName

    elemSchema.name = elemSchema.name or elemName

    elemSchema.ui = elemSchema.ui or {}

    if elemSchema.choices and #elemSchema.choices == 0 then
        error('field "choices" cannot be empty')
    end

    -- auto-guess type if missing:
    if not elemSchema.type then
        if elemSchema.choices then
            elemSchema.type = 'choices'
        elseif elemSchema.callback then
            elemSchema.type = 'callback'
        else
            error('missing type')
        end
    end

    -- standard default value if not given:
    if elemSchema.default == nil then
        if elemSchema.type == 'string' then
            elemSchema.default = ''
        elseif elemSchema.type == 'int' or elemSchema.type == 'float' then
            elemSchema.default = 0
        elseif elemSchema.type == 'color' then
            elemSchema.default = {0.85, 0.85, 1.0}
        elseif elemSchema.type == 'bool' then
            elemSchema.default = false
        elseif elemSchema.choices then
            elemSchema.default = elemSchema.choices[1]
        elseif elemSchema.type == 'callback' then
            elemSchema.default = ''
        end
    end

    if elemSchema.default == nil then
        error('missing "default" for key "' .. elemName .. '"')
    end

    if elemSchema.choices and not table.find(elemSchema.choices, elemSchema.default) then
        error('the given default for key "' .. elemName .. '" is not contained in "choices"')
    end

    -- auto-guess control if missing:
    if not elemSchema.ui.control then
        if elemSchema.type == 'string' then
            elemSchema.ui.control = 'edit'
        elseif elemSchema.type == 'float' and elemSchema.minimum and elemSchema.maximum then
            elemSchema.ui.control = 'slider'
        elseif elemSchema.type == 'int' or elemSchema.type == 'float' then
            elemSchema.ui.control = 'spinbox'
        elseif elemSchema.type == 'bool' then
            elemSchema.ui.control = 'checkbox'
        elseif elemSchema.type == 'color' then
            elemSchema.ui.control = 'color'
        elseif elemSchema.type == 'choices' then
            elemSchema.ui.control = 'radio'
        elseif elemSchema.type == 'callback' then
            elemSchema.ui.control = 'button'
        else
            error('missing "ui.control" and cannot infer it from type')
        end
    end
end

function ConfigUI:validateSchema()
    assert(self.schema ~= nil, 'missing schema')
    for elemName, elemSchema in pairs(self.schema) do
        local success, errorMessage = pcall(function()
            self:validateElemSchema(elemName, elemSchema)
        end)
        if not success then
            error('element "' .. elemName .. '": ' .. errorMessage)
        end
    end
end

function ConfigUI:getObject()
    if self.getObjectCallback then
        return self:getObjectCallback()
    end
    return self.targetObject or sim.getObject '.'
end

function ConfigUI:getObjectName()
    if self.getObjectNameCallback then
        return self:getObjectNameCallback()
    end
    return sim.getObjectAlias(self:getObject(), 1)
end

function ConfigUI:readSchema()
    local schema = sim.getTableProperty(self:getObject(), 'customData.' .. self.propertyName.schema, {noError=true}) or {}
    if next(schema) ~= nil then
        self.schema = {}
        for k, v in pairs(schema) do self.schema[k] = v end
    elseif self.schema == nil then
        error('schema not provided, and not found in the custom data block ' ..  self.propertyName.schema)
    end
end

function ConfigUI:getConfigProperty(k)
    local nsConfig = self.propertyNamespace.config or ''
    if nsConfig ~= '' then nsConfig = nsConfig .. '.' end
    return 'customData.' .. nsConfig .. k
end

function ConfigUI:writeMissingConfigEntries()
    assert(self.schema ~= nil, 'missing schema')
    for k, v in pairs(self.schema) do
        if sim.getPropertyInfo(self:getObject(), self:getConfigProperty(k)) == nil then
            self:writeProperty(k, self:readProperty(k))
        end
    end
end

function ConfigUI:readConfigConst()
    assert(self.schema ~= nil, 'missing schema')
    local config = {}
    for k, v in pairs(self.schema) do
        config[k] = self:readProperty(k)
    end
    return config
end

function ConfigUI:readProperty(k)
    assert(self.schema ~= nil, 'missing schema')
    local elemSchema = self.schema[k]
    if not elemSchema then return end
    local val = sim.getProperty(self:getObject(), self:getConfigProperty(k), {noError = true})
    if val == nil then val = elemSchema.default end
    return val
end

function ConfigUI:writeProperty(k, v)
    assert(self.schema ~= nil, 'missing schema')
    local elemSchema = self.schema[k]
    if not elemSchema then return end
    if elemSchema.type == 'float' then
        -- because of the JSON serialization between CoppeliaSim and QML,
        -- 0.0 becomes 0; so use explicit float setter:
        sim.setFloatProperty(self:getObject(), self:getConfigProperty(k), v)
    elseif elemSchema.type == 'color' then
        sim.setColorProperty(self:getObject(), self:getConfigProperty(k), v)
    else
        sim.setProperty(self:getObject(), self:getConfigProperty(k), v)
    end
end

function ConfigUI:createUi()
    if self.uiHandle then return end
    self.uiHandle = simQML.createEngine()
    simQML.setEventHandler(self.uiHandle, 'dispatchEventsToFunctions')
    local qmlFile = sim.getStringParam(sim.stringparam_resourcesdir) .. '/qml/ConfigUI/ConfigUIWindow.qml'
    simQML.load(self.uiHandle, qmlFile)
    simQML.sendEvent(self.uiHandle, 'setConfigAndSchema', {
        config = self:readConfigConst(),
        schema = self.schema,
        objectName = sim.getObjectAlias(self:getObject(), 1),
    })
    simQML.sendEvent(self.uiHandle, 'setUiState', self.uiState)
end

function ConfigUI:destroyUi()
    if not self.uiHandle then return end
    simQML.destroyEngine(self.uiHandle)
    self.uiHandle = nil
end

function ConfigUI:showUi()
    self:createUi()
end

function ConfigUI:hideUi()
    self:destroyUi()
end

function ConfigUI_uiElemChanged(kv)
    if ConfigUI.instance then
        ConfigUI.instance:uiElemChanged(table.unpack(kv))
    end
end

function ConfigUI_uiState(info)
    if ConfigUI.instance then
        ConfigUI.instance:uiStateChanged(info)
    end
end

function ConfigUI:uiElemChanged(k, v)
    local elemSchema = self.schema[k]
    if not elemSchema then return end
    if v == nil then return end
    if elemSchema.type == 'int' then
        v = math.floor(v)
    end
    self:writeProperty(k, v)
    self:itemChanged(k, v)
end

function ConfigUI:uiStateChanged(uiState)
    for k, v in pairs(uiState) do
        self.uiState[k] = v
    end

    if self.uiHandle and not uiState.opened then
        -- UI is closing
        self:destroyUi()
    end
end

function ConfigUI:itemChanged(k, v)
    if type(self.itemChangedCallback) == 'function' then
        self.itemChangedCallback(k, v)
    end
    self:configChanged()
end

function ConfigUI:configChanged()
    if type(self.configChangedCallback) == 'function' then
        self.configChangedCallback(self:readConfigConst())
    end

    -- call generate callback only if config actually changed, to save some CPU...
    local cfg = self:readConfigConst()
    local cfgPack = sim.packTable(cfg)
    if cfgPack ~= self.generatedForConfig then
        self:generateLater()
        self.generatedForConfig = cfgPack
        -- sim.announceSceneContentChange() leave this out for now
    end
end

function ConfigUI:sysCall_init()
    self:readSchema()
    self:validateSchema()
    self:writeMissingConfigEntries()

    self.uiState = self.uiState or {}
    local uiState = sim.getTableProperty(self:getObject(), 'customData.configUi.uistate', {noError=true})
    if uiState then
        sim.removeProperty(self:getObject(), 'customData.configUi.uistate')
        for k, v in pairs(uiState) do
            self.uiState[k] = v
        end
    end
    if self.uiState.opened then
        self:showUi()
    end
end

function ConfigUI:sysCall_cleanup()
    -- save uistate here so it can persist a script restart:
    sim.setTableProperty(self:getObject(), 'customData.configUi.uistate', self.uiState)

    self:destroyUi()
end

function ConfigUI:sysCall_userConfig()
    if self.userConfig == false then return end
    if self.allowDuringSimulation or sim.getSimulationState() == sim.simulation_stopped then
        self:showUi()
    end
end

function ConfigUI:sysCall_data(changedNames, ns)
    for changedName, g in pairs(changedNames) do
        local typ, name
        if changedName:startswith '&' then
            typ, name = changedName:match("&(.-)&%.(.*)")
        else
            typ, name = nil, changedName
        end
        if self.propertyNamespace.config ~= '' and name then
            assert(string.startswith(name, self.propertyNamespace.config .. '.'))
            name = name:sub(1 + #self.propertyNamespace.config + 1)
        end
        if ns == 'customData' and self.schema[name] then
            if self.uiHandle then
                simQML.sendEvent(self.uiHandle, 'setConfig', {name, self:readProperty(name)})
            end
            self:itemChanged(name, self:readProperty(name))
        end
    end
end

function ConfigUI:sysCall_nonSimulation()
    self:generateIfNeeded()
end

function ConfigUI:sysCall_beforeSimulation()
    if self.uiHandle then
        if self.allowDuringSimulation then
            simQML.sendEvent(self.uiHandle, 'beforeSimulation', self:readConfigConst())
        else
            self.reopenAfterSimulation = true
            self:hideUi()
        end
    end
end

function ConfigUI:sysCall_sensing()
    self:generateIfNeeded()
end

function ConfigUI:sysCall_afterSimulation()
    if self.reopenAfterSimulation then
        self.reopenAfterSimulation = nil
        self:showUi()
    end

    if self.uiHandle then
        simQML.sendEvent(self.uiHandle, 'afterSimulation', self:readConfigConst())
    end
end

function ConfigUI:generateLater()
    if self.generateCallback then
        self.generatePending = true
    end
end

function ConfigUI:generateIfNeeded()
    if self.userConfig == false and self.allowDuringSimulation then
        local sel = sim.getObjectSel()
        if #sel == 1 and sel[1] == self.targetObject then
            self:showUi()
        else
            self:hideUi()
        end
    end

    if self.generateCallback then
        if self.generatePending then --and (self.generatePending + self.generationTime)<sim.getSystemTime() then
            self.generatePending = false
            self.generateCallback(self:readConfigConst())
            -- sim.announceSceneContentChange() leave this out for now
        end
    end
end

function ConfigUI:__index(k)
    return ConfigUI[k]
end

setmetatable(ConfigUI, {__call = function(meta, schema)
    sim = require 'sim'
    if sim.getProperty(sim.handle_app, 'productVersionNb') < 4090001 then
        error('CoppeliaSim V4.9.0 rev1 or later is required')
    end
    simQML = require 'simQML'
    if table.compare(simQML.qtVersion(), {5, 15}) < 0 then
        error('Qt version 5.15 or greater is required (have ' .. table.join(simQML.qtVersion(), '.') .. ')')
    end
    if ConfigUI.instance then
        error('multiple instances of ConfigUI not supported')
    end
    local self = setmetatable({
        propertyNamespace = {
            config = '',
        },
        propertyName = {
            schema = '__schema__',
        },
        targetObject = sim.getObject '.',
        schema = schema,
        generatePending = false,
        allowDuringSimulation = false,
    }, meta)
    sim.registerScriptFuncHook('sysCall_init', function() self:sysCall_init() end)
    sim.registerScriptFuncHook('sysCall_cleanup', function() self:sysCall_cleanup() end)
    sim.registerScriptFuncHook('sysCall_userConfig', function() self:sysCall_userConfig() end, true)
    sim.registerScriptFuncHook('sysCall_data', function(...) self:sysCall_data(...) end)
    sim.registerScriptFuncHook('sysCall_nonSimulation', function() self:sysCall_nonSimulation() end)
    sim.registerScriptFuncHook('sysCall_beforeSimulation', function() self:sysCall_beforeSimulation() end, true)
    sim.registerScriptFuncHook('sysCall_sensing', function() self:sysCall_sensing() end)
    sim.registerScriptFuncHook('sysCall_afterSimulation', function() self:sysCall_afterSimulation() end, true)
    ConfigUI.instance = self
    return self
end})
