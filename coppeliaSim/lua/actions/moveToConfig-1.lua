--[[ USAGE EXAMPLE
function sysCall_init()
    sim = require('sim')
    require'actionServer-1'
    require'actions/moveToConfig-1'

    actionServer = ActionServer()
    
    -- Following is required:
    -------------------------
    local params = {}
    params.joints = {}
    for i = 1, 6, 1 do
        params.joints[i] = sim.getObject('./UR5_joint*', {index = i - 1})
    end
    params.targetPos = {1.4981571262069, 0.23091510688649, 1.1006232214541, 1.8100532979545, 1.4981571702606, -3.1415916760289}
    -------------------------
    
    refer to sim.moveToConfig documentation for additional, optional parameters

    id = actionServer:send({cmd = 'moveToConfig', params = params})
end
--]]

sim = require('sim')

function action_moveToConfig_accept(cmd)
    cmd.params = cmd.params or {}
    cmd.params = table.deepcopy(cmd.params)
    cmd.state = {}
    return true
end

function action_moveToConfig_execute(cmd)
    cmd.state.motionObject = sim.moveToConfig_init(cmd.params)
    return true
end

function action_moveToConfig_cancel(cmd)
    action_moveToConfig_cleanup(cmd)
    return true
end

function action_moveToConfig_cleanup(cmd)
    if cmd.state.motionObject then
        sim.moveToConfig_cleanup(cmd.state.motionObject)
        cmd.state.motionObject = nil
    end
end

function action_moveToConfig_tick(cmd)
    local result, state = sim.moveToConfig_step(cmd.state.motionObject)
    local retVal = {}
    if result >= 0 then
        retVal.feedback = {}
        retVal.feedback.pos = state.pos
        retVal.feedback.vel = state.vel
        retVal.feedback.accel = state.accel
        if result == 1 then
            retVal.transition = 'succeed'
            retVal.result = retVal.feedback
            action_moveToConfig_cleanup(cmd)
        end
        if result == 2 then
            retVal.transition = 'abort'
            retVal.result = retVal.feedback
            action_moveToConfig_cleanup(cmd)
        end
    else
        error('sim.moveToConfig_step returned error code ' .. result)
    end
    
    return retVal
end
