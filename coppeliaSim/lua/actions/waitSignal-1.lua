--[[

ACTION:
    waitSignal

REQUIRED PARAMETERS:
    signalName (string): name of a signal
    signalType (string): either 'int', 'float' or 'string'

OPTIONAL PARAMETERS:
    signalValue (string, int or float, depending on signalType)
    clearSignal (bool): if true, the signal will be cleared upon succeeding

DESCRIPTION:
    Wait for a signal to be set.
    If `signalValue` is specified, will wait for the signal to have a specific
    value, otherwise the action will succeed as soon as the signal is set with
    any value.

    If `clearSignal` is true, the signal will be cleared upon succeeding.

EXAMPLE:
    require 'actions/waitSignal'

    actionServer1:send('waitSignal', {signalName = 'A', signalType = 'int'})

--]]

sim = require('sim')

function action_waitSignal_accept(cmd)
    local function checkField(fieldName, fieldType, mathType)
        assert(cmd.params[fieldName] ~= nil, 'missing ' .. fieldName .. ' field')
        if fieldType then
            assert(type(cmd.params[fieldName]) == fieldType, fieldName .. ' field must be a ' .. fieldType)
            if mathType then
                assert(math.type(cmd.params[fieldName]) == mathType, fieldName .. ' field must be a ' .. mathType)
            end
        end
    end

    checkField('signalName', 'string')

    checkField('signalType', 'string')
    require 'tablex'
    local v = {'int', 'float', 'string'}
    assert(table.find(v, cmd.params.signalType), 'invalid signal type. must be one of: ' .. table.join(v))

    if cmd.params.signalValue ~= nil then
        if cmd.params.signalType == 'int' then
            checkField('signalValue', 'number', 'integer')
        elseif cmd.params.signalType == 'float' then
            checkField('signalValue', 'number')
        elseif cmd.params.signalType == 'string' then
            checkField('signalValue', 'string')
        end
    end

    return true
end

function action_waitSignal_tick(cmd)
    local value = nil
    if cmd.params.signalType == 'int' then
        value = sim.getInt32Signal(cmd.params.signalName)
    elseif cmd.params.signalType == 'float' then
        value = sim.getFloatSignal(cmd.params.signalName)
    elseif cmd.params.signalType == 'string' then
        value = sim.getStringSignal(cmd.params.signalName)
    end
    if value ~= nil and (cmd.params.signalValue == value or cmd.params.signalValue == nil) then
        if clearSignal then
            if cmd.params.signalType == 'int' then
                sim.clearInt32Signal(cmd.params.signalName)
            elseif cmd.params.signalType == 'float' then
                sim.clearFloatSignal(cmd.params.signalName)
            elseif cmd.params.signalType == 'string' then
                sim.clearStringSignal(cmd.params.signalName)
            end
        end
        return {transition = 'succeed'}
    end
end
