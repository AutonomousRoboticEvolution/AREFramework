--[[

ACTION:
    openGripper, closeGripper

REQUIRED PARAMETERS:
    handle (int): handle of the gripper

OPTIONAL PARAMETERS:
    scriptType (int): the type of the gripper script (default: sim.scripttype_simulation)

DESCRIPTION:
    Control a gripper using the standard gripper interface.
    The standard gripper interface consists of implementing the three functions:
        - open()
        - close()
        - getStatus() -> 'opened' || 'closed'

EXAMPLE:
    require 'actions/gripper'

    local h = sim.getObject '/path/to/gripper'
    actionServer:send('openGripper', {handle = h})
    actionServer:send('closeGripper', {handle = h})

--]]

function action_gripper_accept(cmd)
    assert(type(cmd.params.handle) == 'number' and sim.isHandle(cmd.params.handle), 'invalid handle')
    cmd.params.scriptType = cmd.params.scriptType or sim.scripttype_simulation
    cmd.state.script = sim.getScript(cmd.params.scriptType, cmd.params.handle)
    assert(cmd.state.script ~= -1, 'no such gripper script')
    cmd.state.gripper = sim.getScriptFunctions(cmd.state.script)
    return true
end

function action_openGripper_accept(cmd)
    return action_gripper_accept(cmd)
end

function action_closeGripper_accept(cmd)
    return action_gripper_accept(cmd)
end

function action_openGripper_execute(cmd)
    cmd.state.gripper:open()
    return true
end

function action_closeGripper_execute(cmd)
    cmd.state.gripper:close()
    return true
end

function action_openGripper_tick(cmd)
    if cmd.state.gripper:getStatus() == 'opened' then
        return {transition = 'succeed'}
    end
end

function action_closeGripper_tick(cmd)
    if cmd.state.gripper:getStatus() == 'closed' then
        return {transition = 'succeed'}
    end
end
