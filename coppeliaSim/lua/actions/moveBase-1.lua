--[[

ACTION:
    moveBase

REQUIRED PARAMETERS:
    baseHandle
    leftWheelJointHandle
    rightWheelJointHandle
    boundsMax
    boundsMin
    targetConfig
    targetPose

OPTIONAL PARAMETERS:
    leftWheelVelocityFactor
    rightWheelVelocityFactor
    targetDistanceLimit
    maxPlanningTime
    robotCollectionHandle
    environmentCollectionHandle
    planningAlgorithm
    getConfig
    setConfig
    kVelocity
    holonomic
    timeout
    errorPlot
    drawPath

DESCRIPTION:
    Control a gripper using the standard gripper interface.
    The standard gripper interface consists of implementing the three functions:
        - open()
        - close()
        - getStatus() -> 'opened' || 'closed'

EXAMPLE:
    require 'actions/moveBase'

    local h = sim.getObject '/path/to/gripper'
    actionServer:send('moveBase', {handle = h})

--]]

sim = require('sim')
simOMPL = require('simOMPL')

require 'matrix'


function action_moveBase_accept(cmd)
    for _, f in ipairs{'baseHandle', 'leftWheelJointHandle', 'rightWheelJointHandle'} do
        assert(type(cmd[f]) == 'number' and sim.isHandle(cmd[f]), 'param ' .. f .. ' required')
    end
    cmd.params.leftWheelVelocityFactor = cmd.params.leftWheelVelocityFactor or 1
    cmd.params.rightWheelVelocityFactor = cmd.params.rightWheelVelocityFactor or 1
    cmd.params.kVelocity = cmd.params.kVelocity or 15
    cmd.params.targetDistanceLimit = cmd.params.targetDistanceLimit or 0.2
    cmd.params.maxPlanningTime = cmd.params.maxPlanningTime or 2.0
    if cmd.params.robotCollectionHandle == nil then
        cmd.params.robotCollectionHandle = sim.createCollection()
        sim.addItemToCollection(cmd.params.robotCollectionHandle, sim.handle_tree, cmd.params.baseHandle, 0)
    end
    cmd.params.environmentCollectionHandle = cmd.params.environmentCollectionHandle or sim.handle_all
    cmd.params.planningAlgorithm = cmd.params.planningAlgorithm or simOMPL.Algorithm.RRT
    cmd.params.getConfig = cmd.params.getConfig or function(handle)
        handle = handle or cmd.params.baseHandle
        local p = sim.getObjectPosition(handle)
        local o = sim.getObjectOrientation(handle)
        return {p[1], p[2], o[3]}
    end
    cmd.params.setConfig = cmd.params.setConfig or function(cfg, handle)
        handle = handle or cmd.params.baseHandle
        sim.setObjectPosition(handle, {cfg[1], cfg[2], 0.13879})
        sim.setObjectOrientation(handle, {0, 0, cfg[3]})
    end
    assert(type(cmd.params.targetPose or cmd.params.targetConfig) == 'table', 'either targetPose or targetConfig should be given')
    return true
end

function action_moveBase_execute(cmd)
    cmd.state.task = simOMPL.createTask('moveBase-' .. cmd.id)
    if cmd.params.holonomic then
        error 'holonomic not implemented yet!'
    else
        cmd.params.boundsMin = cmd.params.boundsMin or {-4.5, -4.5}
        cmd.params.boundsMax = cmd.params.boundsMax or {4.5, 4.5}
        local ss = simOMPL.createStateSpace('', simOMPL.StateSpaceType.dubins,
            cmd.params.baseHandle, cmd.params.boundsMin, cmd.params.boundsMax, 1)
        simOMPL.setDubinsParams(ss, 0.2, false)
        simOMPL.setStateSpace(cmd.state.task, {ss})
    end
    simOMPL.setCollisionPairs(cmd.state.task, {cmd.params.robotCollectionHandle, cmd.params.environmentCollectionHandle})
    simOMPL.setAlgorithm(cmd.state.task, cmd.params.planningAlgorithm)
    simOMPL.setStartState(cmd.state.task, cmd.params.getConfig())
    if cmd.params.targetConfig == nil then
        local target = sim.createDummy(0)
        sim.setObjectPose(target, cmd.params.targetPose)
        cmd.params.targetConfig = cmd.params.getConfig(target)
        sim.removeObjects{target}
    end
    simOMPL.setGoalState(cmd.state.task, cmd.params.targetConfig)
    simOMPL.setup(cmd.state.task)
    if simOMPL.solve(cmd.state.task, cmd.params.maxPlanningTime) then
        simOMPL.simplifyPath(cmd.state.task, cmd.params.maxPlanningTime)
        local n = 250
        simOMPL.interpolatePath(cmd.state.task, n)
        cmd.state.path = simOMPL.getPath(cmd.state.task)
        cmd.state.path = Matrix(-1, simOMPL.getStateSpaceDimension(cmd.state.task), cmd.state.path)
        cmd.state.pathTrackingDummy = sim.createDummy(0.1)
        cmd.state.pathIndex = 1
        --printf('solved: %s (%s)', solved, simOMPL.hasApproximateSolution(cmd.state.task) and 'approximate' or 'exact')
        --printf('path: %d states', #path)
        if cmd.params.drawPath then
            cmd.state.dwo = sim.addDrawingObject(sim.drawing_linestrip, 5, 0, -1, #cmd.state.path, {1,0,0})
            for i = 1, #cmd.state.path do
                cmd.params.setConfig(cmd.state.path:row(i):data(), cmd.state.pathTrackingDummy)
                sim.addDrawingObjectItem(cmd.state.dwo, sim.getObjectPosition(cmd.state.pathTrackingDummy))
            end
        else
            sim.setObjectInt32Param(cmd.state.pathTrackingDummy, sim.objintparam_visibility_layer, 0)
        end
        cmd.params.setConfig(cmd.state.path:row(cmd.state.pathIndex):data(), cmd.state.pathTrackingDummy)
    end
    cmd.state.startTime = sim.getSystemTime()

    if type(cmd.params.errorPlot) == 'number' and sim.isHandle(cmd.params.errorPlot) then
        sim.resetGraph(cmd.params.errorPlot)
        cmd.state.errorPlotPos = sim.addGraphStream(cmd.params.errorPlot, 'pos', 'm', 0, {1, 0, 0})
        cmd.state.errorPlotOrient = sim.addGraphStream(cmd.params.errorPlot, 'orient', 'rad', 0, {0, 1, 0})
    end

    return true
end

function action_moveBase_tick(cmd)
    local errPos = sim.getObjectPosition(cmd.state.pathTrackingDummy, cmd.params.baseHandle)
    local errOrient = (sim.getObjectOrientation(cmd.state.pathTrackingDummy, cmd.params.baseHandle))[3]
    local errX, errY = errPos[1], errPos[2]
    local errPosNorm = math.sqrt(errX * errX + errY * errY)
    local vLeft, vRight = 0, 0
    if errPosNorm > 0.2 * cmd.params.targetDistanceLimit then
        vLeft = cmd.params.kVelocity * cmd.params.leftWheelVelocityFactor * (errX - errY)
        vRight = cmd.params.kVelocity * cmd.params.rightWheelVelocityFactor * (errX + errY)
    else
        vLeft = cmd.params.kVelocity * cmd.params.leftWheelVelocityFactor * -errOrient
        vRight = cmd.params.kVelocity * cmd.params.rightWheelVelocityFactor * errOrient
    end
    sim.setJointTargetVelocity(cmd.params.leftWheelJointHandle, vLeft)
    sim.setJointTargetVelocity(cmd.params.rightWheelJointHandle, vRight)

    if type(cmd.params.errorPlot) == 'number' and sim.isHandle(cmd.params.errorPlot) then
        sim.setGraphStreamValue(cmd.params.errorPlot, cmd.state.errorPlotPos, errPosNorm)
        sim.setGraphStreamValue(cmd.params.errorPlot, cmd.state.errorPlotOrient, errOrient)
    end

    if errPosNorm < cmd.params.targetDistanceLimit and cmd.state.pathIndex < cmd.state.path:rows() then
        cmd.state.pathIndex = cmd.state.pathIndex + 1
        cmd.params.setConfig(cmd.state.path:row(cmd.state.pathIndex):data(), cmd.state.pathTrackingDummy)
    end

    if cmd.params.timeout and sim.getSystemTime() >= (cmd.state.startTime + cmd.params.timeout) then
        action_moveBase_cleanup(cmd)
        return {transition = 'abort'}
    end
end

function action_moveBase_cancel(cmd)
    action_moveBase_cleanup(cmd)
    return true
end

function action_moveBase_cleanup(cmd)
    if cmd.state.task then
        simOMPL.destroyTask(cmd.state.task)
        cmd.state.task = nil
    end
    if cmd.state.dwo then
        sim.removeDrawingObject(cmd.state.dwo)
        cmd.state.dwo = nil
    end
end

