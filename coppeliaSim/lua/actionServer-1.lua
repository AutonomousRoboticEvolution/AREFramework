ActionServer = {}

function ActionServer:send(cmdName, cmdParams)
    assert(type(cmdName) == 'string')
    cmdParams = cmdParams or {}

    self.lastCmdId = self.lastCmdId + 1
    local queuedCmd = {
        id = self.lastCmdId,
        name = cmdName,
        server = self,
        state = {},
        params = cmdParams,
    }

    local info = self:_callActionMethod(queuedCmd, 'info', {})
    local api = info.api or 1

    assert(api == self.api, 'mismatching actionServer api (requires api = ' .. api .. ')')

    local accepted = self:_callActionMethod(queuedCmd, 'accept', true)
    if accepted then
        table.insert(self.queue, queuedCmd)
        self:_setStatus(queuedCmd.id, 'accepted')
        return queuedCmd.id
    else
        self:_setStatus(queuedCmd.id, 'rejected')
    end
end

function ActionServer:cancel(cmdId)
    require 'tablex'
    local index = table.find(self.queue, cmdId, function(cmd)
        return cmd.id == cmdId
    end)
    if index then
        table.remove(self.queue, index)
    elseif self.currentCmd.id == cmdId then
        local cancelOk = self:_callActionMethod(self.currentCmd, 'cancel', true)
        if not cancelOk then return end
        self.currentCmd = nil
    end
    self:_setStatus(cmdId, 'canceled')
end

function ActionServer:getStatus(cmdId)
    return self.status[cmdId]
end

function ActionServer:getFeedback(cmdId)
    return self.feedback[cmdId]
end

function ActionServer:getResult(cmdId)
    return self.result[cmdId]
end

function ActionServer:tick()
    if self.hookSysCallActuation then
        error('method tick() should not be called unless option hook_sysCall_actuation is set to false')
    end
    return self:_handleTick()
end

function ActionServer:_handleTick()
    -- if not doing anything, pick next command:
    while self.currentCmd == nil do
        if #self.queue == 0 then return end
        self.currentCmd = table.remove(self.queue, 1)
        local executeOk = self:_callActionMethod(self.currentCmd, 'execute', true)
        if executeOk then
            self:_setStatus(self.currentCmd.id, 'executing')
        else
            self:_setStatus(self.currentCmd.id, 'aborted')
            self.currentCmd = nil
        end
    end

    self.currentCmd = {
        id = self.currentCmd.id,
        name = self.currentCmd.name,
        server = self,
        state = self.currentCmd.state,
        params = self.currentCmd.params,
    }

    -- continue running current action:
    local r = self:_callActionMethod(self.currentCmd, 'tick')
    if r == nil then
        -- r == nil --> continue execution in next tick
        return
    end
    if type(r) ~= 'table' then
        r = {transition = 'succeed', result = r}
    end
    if r.feedback then
        self.feedback[self.currentCmd.id] = r.feedback
    end
    if r.transition == 'abort' then
        self:_setStatus(self.currentCmd.id, 'aborted')
        self.currentCmd = nil
    elseif r.transition == 'succeed' then
        self.result[self.currentCmd.id] = r.result
        self:_setStatus(self.currentCmd.id, 'succeeded')
        self.currentCmd = nil
    elseif r.transition then
        error(string.format('action "%s" returned invalid transition "%s"', self.currentCmd.name, r.transition))
    end
end

function ActionServer:_isModuleLoaded(n)
    for k, v in pairs(package.loaded) do
        if string.find('/' .. k, '[./]' .. n .. '%-?%d?$') then
            return true
        end
    end
end

function ActionServer:_callActionMethod(cmd, method, defaultRetIfNoSuchMethod)
    local funcName = 'action_' .. cmd.name .. '_' .. method
    if _G[funcName] == nil then
        if defaultRetIfNoSuchMethod == nil then
            if self:_isModuleLoaded(cmd.name) then
                error('function "' .. funcName .. '" not found')
            else
                error('module "' .. cmd.name .. '" not loaded')
            end
        else
            return defaultRetIfNoSuchMethod
        end
    end
    return _G[funcName](cmd)
end

function ActionServer:addListener(event, listener, cmdId)
    cmdId = cmdId or 'global'
    self.listeners[event] = self.listeners[event] or {}
    self.listeners[event][cmdId] = self.listeners[event][cmdId] or {}
    table.insert(self.listeners[event][cmdId], listener)
end

function ActionServer:_notifyListener(event, cmdId)
    self.listeners[event] = self.listeners[event] or {}
    self.listeners[event]['global'] = self.listeners[event]['global'] or {}
    self.listeners[event][cmdId] = self.listeners[event][cmdId] or {}
    for _, cmdId1 in ipairs{cmdId, 'global'} do
        for i, listener in ipairs(self.listeners[event][cmdId1]) do
            listener(event, cmdId)
        end
    end
end

function ActionServer:_setStatus(cmdId, status)
    if self.status[cmdId] ~= status then
        self.status[cmdId] = status
        self:_notifyListener(status, cmdId)
        self:_notifyListener('statusChanged', cmdId)
    end
end

function ActionServer:__index(k)
    return ActionServer[k]
end

setmetatable(ActionServer, {__call = function(meta, name_or_opts)
    local opts = nil
    if type(name_or_opts) == 'string' then
        opts = {name = name_or_opts}
    elseif type(name_or_opts) == 'table' or name_or_opts == nil then
        opts = name_or_opts or {}
    else
        error('bad argument type: ' .. type(name_or_opts))
    end

    opts.name = opts.name or 'default'
    opts.hook_sysCall_actuation = opts.hook_sysCall_actuation ~= false

    ActionServer._byName = ActionServer._byName or {}
    assert(ActionServer._byName[opts.name] == nil, 'action server name must be unique')
    local self = setmetatable({
        api = 1,
        name = opts.name,
        lastCmdId = 0,
        queue = {},
        currentCmd = nil,
        toCancel = {},
        status = {},
        result = {},
        feedback = {},
        listeners = {},
        hookSysCallActuation = opts.hook_sysCall_actuation,
    }, meta)
    ActionServer._byName[opts.name] = self

    if opts.hook_sysCall_actuation then
        sim.registerScriptFuncHook('sysCall_actuation', function() self:_handleTick() end, false)
    end

    return self
end})

function ActionServer:byName(name)
    assert(ActionServer._byName[name] ~= nil, 'invalid action server name')
    return ActionServer._byName[name]
end

function ActionServer:defineFunctionalInterface()
    local publicFunctions = {
        'send',
        'cancel',
        'getStatus',
        'getFeedback',
        'getResult',
        'tick',
    }
    for _, funcName in ipairs(publicFunctions) do
        _G['actionServer_' .. funcName] = function(name, ...)
            local as = ActionServer:byName(name)
            return as[funcName](as, ...)
        end
    end
end

function ActionServer:monitorQueue(tbl)
    tbl = tbl or {}
    tbl.seenIds = tbl.seenIds or {}
    tbl.cmds = tbl.cmds or {}
    for _, cmd in ipairs(self.queue) do
        if not tbl.seenIds[cmd.id] then
            tbl.seenIds[cmd.id] = true
            table.insert(tbl.cmds, {id = cmd.id, name = cmd.name, params = cmd.params})
        end
    end
    for i, cmd in ipairs(tbl.cmds) do
        cmd.status = self:getStatus(cmd.id)
        cmd.feedback = self:getFeedback(cmd.id)
    end
    return tbl
end
