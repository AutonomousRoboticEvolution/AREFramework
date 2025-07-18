--[[

ACTION:
    join

REQUIRED PARAMETERS:
    id (string): matching identifier
    n (int): number of servers that have to execute the action

OPTIONAL PARAMETERS:
    -

DESCRIPTION:
    Wait for n servers executing this action, then succeeed (useful for
    synchronization of multiple action servers).

    Note: currently, only multiple action servers within the same script are
    supported.

EXAMPLE:
    require 'actions/join'

    actionServer1:send('join', {id = 'A', n = 2})
    actionServer2:send('join', {id = 'A', n = 2})

--]]

function action_join_tick(cmd)
    action_join_joined = action_join_joined or {}
    action_join_joined[cmd.id] = action_join_joined[cmd.id] or {}
    action_join_joined[cmd.id][cmd.server.name] = true
    local numJoined = 0
    for k, v in pairs(action_join_joined[cmd.id]) do
        numJoined = numJoined + 1
    end
    if numJoined == cmd.params.n then
        return {transition = 'succeed'}
    end
end
