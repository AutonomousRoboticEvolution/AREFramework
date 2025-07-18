--[[

ACTION:
    wait

REQUIRED PARAMETERS:
    duration (float): number of seconds to wait

OPTIONAL PARAMETERS:
    useSimTime (bool): if true, simulation time will be used, otherwise real time

DESCRIPTION:
    Wait for the specified amount of seconds, then succeed.

EXAMPLE:
    require 'actions/wait'

    actionServer:send('wait', {duration = 2})

--]]

sim = require('sim')

function action_wait_accept(cmd)
    assert(cmd.params.duration, 'missing duration field')
    assert(type(cmd.params.duration) == 'number', 'duration field must be a number')
    assert(cmd.params.useSimTime == nil or type(cmd.params.useSimTime) == 'boolean', 'useSimTime field must be a boolean')
    return true
end

function action_wait_get_time(cmd)
    if cmd.params.useSimTime then
        return sim.getSimulationTime()
    else
        return sim.getSystemTime()
    end
end

function action_wait_execute(cmd)
    cmd.state.startTime = action_wait_get_time(cmd)
    return true
end

function action_wait_tick(cmd)
    local remaining = math.max(0, cmd.state.startTime + cmd.params.duration - action_wait_get_time(cmd))
    local r = {}
    r.feedback = {
        progress = 1 - remaining / cmd.params.duration,
        remainingTime = remaining,
    }
    if remaining < 1e-9 then
        r.transition = 'succeed'
    end
    return r
end
