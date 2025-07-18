--[[

ACTION:
    setSignal

REQUIRED PARAMETERS:
    signalName (string): name of a signal
    signalType (string): either 'int', 'float' or 'string'
    signalValue (string, int or float, depending on signalType)

OPTIONAL PARAMETERS:
    -

DESCRIPTION:
    Sets a signal when the action executes.

EXAMPLE:
    require 'actions/setSignal'

    actionServer:send('setSignal', {signalName = 'A', signalType = 'int', signalValue = 2})

--]]

sim = require('sim')

function action_setSignal_accept(cmd)
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

    if cmd.params.signalValue ~= nil then -- if we leave signalValue empty, we clear the signal
        if cmd.params.signalType == 'int' then
            checkField('signalValue', 'number', 'integer')
        elseif cmd.params.signalType == 'float' then
            checkField('signalValue', 'number')
        elseif cmd.params.signalType == 'string' then
            if (not isBuffer(cmd.params.signalValue)) and type(cmd.params.signalValue) ~= 'string' then
                assert(false, 'signalValue field must be a buffer or a string')
            end
        end
    end

    return true
end

function action_setSignal_tick(cmd)
    local value = nil
    if cmd.params.signalType == 'int' then
        if cmd.params.signalValue then
            sim.setInt32Signal(cmd.params.signalName, cmd.params.signalValue)
        else
            sim.clearInt32Signal(cmd.params.signalName)
        end
    elseif cmd.params.signalType == 'float' then
        if cmd.params.signalValue then
            sim.setFloatSignal(cmd.params.signalName, cmd.params.signalValue)
        else
            sim.clearFloatSignal(cmd.params.signalName)
        end
    elseif cmd.params.signalType == 'string' then
        if cmd.params.signalValue then
            sim.setStringSignal(cmd.params.signalName, cmd.params.signalValue)
        else
            sim.clearStringSignal(cmd.params.signalName)
        end
    end
    return {transition = 'succeed'}
end
