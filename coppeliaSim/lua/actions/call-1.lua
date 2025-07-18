--[[

ACTION:
    call

REQUIRED PARAMETERS:
    func (function): function to call when the action executes;

OPTIONAL PARAMETERS:
    args (table): a list of arguments to pass to the function;

DESCRIPTION:
    call a function when the action executes

EXAMPLE:
    require 'actions/call'

    id = actionServer:send('call', {func = print, args = {'done'}})

--]]

function action_call_accept(cmd)
    assert(cmd.params.func, 'missing func field')
    assert(type(cmd.params.func) == 'function', 'func field must be a function')
    return true
end

function action_call_tick(cmd)
    cmd.params.func(table.unpack(cmd.params.args or {}))
    return {transition = 'succeed'}
end
