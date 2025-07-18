--[[ USAGE EXAMPLE
function sysCall_init()
    sim = require('sim')
    require'actionServer-1'
    require'actions/trackObject-1'

    actionServer = ActionServer()
    
    -- Following is required:
    -------------------------
    local params = {}
    params.ik = {tip = sim.getObject('./UR5_tip'), target = sim.getObject('./UR5_target')}
    -------------------------

    refer to sim.moveToPose documentation for additional, optional parameters

    id = actionServer:send({cmd = 'trackObject', params = params})

end

function sysCall_actuation()
    actionServer:tick()
end
--]]

sim = require('sim')
simIK = require('simIK')

function action_trackObject_accept(cmd)
    cmd.params = cmd.params or {}
    cmd.params = table.deepcopy(cmd.params)
    return true
end

function action_trackObject_execute(cmd)
    cmd.params.ik = cmd.params.ik or {}
    
    if type(cmd.params.ik) ~= 'table' then
        error("invalid 'params.ik' field")
    end
    if cmd.params.ik.tip == nil or type(cmd.params.ik.tip) ~= 'number' then
        error("missing or invalid 'params.ik.tip' field")
    end
    if cmd.params.ik.target == nil or type(cmd.params.ik.target) ~= 'number' then
        error("missing or invalid 'params.ik.target' field")
    end

    cmd.params.ik.altTarget = cmd.params.ik.target
    cmd.params.ik.target = sim.createDummy(0.001)
    sim.setObjectInt32Param(cmd.params.ik.target, sim.objintparam_visibility_layer, 0)
    sim.setObjectPose(cmd.params.ik.target, sim.getObjectPose(cmd.params.ik.altTarget))
    cmd.params.targetPose =  sim.getObjectPose(cmd.params.ik.altTarget)

    cmd.state.motionObject = sim.moveToPose_init(cmd.params)
    return true
end

function action_trackObject_cancel(cmd)
    action_trackObject_cleanup(cmd)
    return true
end

function action_trackObject_cleanup(cmd)
    if cmd.state.motionObject then
        sim.removeObjects({cmd.state.motionObject.ik.targtet})
        sim.moveToPose_cleanup(cmd.state.motionObject)
        cmd.state.motionObject = nil
    end
end

function action_trackObject_tick(cmd)
    local retVal = {}
    local result, state = sim.moveToPose_step(cmd.state.motionObject)
    if result >= 0 then
        retVal.feedback = {}
        retVal.feedback.pose = state.pose
        retVal.feedback.vel = state.vel
        retVal.feedback.accel = state.accel
        
        retVal.result = retVal.feedback
        sim.moveToPose_cleanup(cmd.state.motionObject)
        cmd.params.vel = state.vel
        cmd.params.accel = state.accel
        cmd.params.targetPose =  sim.getObjectPose(cmd.params.ik.altTarget)
        cmd.state.motionObject = sim.moveToPose_init(cmd.params)
        
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
