--[[

ACTION:
    waitCond

REQUIRED PARAMETERS:
    condition (function): a function to check for a condition

OPTIONAL PARAMETERS:
    -

DESCRIPTION:
    Wait for a condition to happen.
    When the function `condition` returns true, the action succeeds.

EXAMPLE:
    require 'actions/waitCond'

    actionServer:send('waitCond', {condition = function()
        return foo() > 3 and bar() == 0
    end})

--]]

function action_waitCond_accept(cmd)
    assert(cmd.params.condition, 'missing condition field')
    assert(type(cmd.params.condition) == 'function', 'condition field must be a function')
    return true
end

function action_waitCond_tick(cmd)
    if cmd.params.condition() then
        return {transition = 'succeed'}
    end
end
