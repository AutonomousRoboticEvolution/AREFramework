--[[ USAGE EXAMPLE
function sysCall_init()
    sim = require('sim')
    require'actionServer-1'
    require'actions/moveToPose-1'

    actionServer = ActionServer()
    
    -- Following is required:
    -------------------------
    local params = {}
    -- One of the following 2 is optional (i.e. just object movement or full arm movement via IK)
    --params.object = sim.getObject('./UR5_tip')
    params.ik = {tip = sim.getObject('./UR5_tip'), target = sim.getObject('./UR5_target')}
    
    params.targetPose = {0.0, 0.2, 0.50096988332966, 1.9832943768143e-05, -0.70724195339448, -7.2268179229349e-06, 0.70697158281871}
    -------------------------
    
    refer to sim.moveToPose documentation for additional, optional parameters

    id = actionServer:send({cmd = 'moveToPose', params = params})
end
--]]

sim = require('sim')
simIK = require('simIK')

function action_moveToPose_accept(cmd)
    cmd.params = cmd.params or {}
    cmd.params = table.deepcopy(cmd.params)
    return true
end

function action_moveToPose_execute(cmd)
    cmd.state.motionObject = sim.moveToPose_init(cmd.params)
    return true
end

function action_moveToPose_cancel(cmd)
    action_moveToPose_cleanup(cmd)
    return true
end

function action_moveToPose_cleanup(cmd)
    if cmd.state.motionObject then
        sim.moveToPose_cleanup(cmd.state.motionObject)
        cmd.state.motionObject = nil
    end
end

function action_moveToPose_tick(cmd)
    local retVal = {}
    local result, state = sim.moveToPose_step(cmd.state.motionObject)
    if result >= 0 then
        retVal.feedback = {}
        retVal.feedback.pose = state.pose
        retVal.feedback.vel = state.vel
        retVal.feedback.accel = state.accel
        
        if result == 1 then
            retVal.transition = 'succeed'
            retVal.result = retVal.feedback
            action_moveToPose_cleanup(cmd)
        end
        if result == 2 then
            retVal.transition = 'abort'
            retVal.result = retVal.feedback
            action_moveToPose_cleanup(cmd)
        end
    else
        error('sim.moveToPose_step returned error code ' .. result)
    end

    return retVal
end
