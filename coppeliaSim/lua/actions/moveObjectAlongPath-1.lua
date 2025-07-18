--[[

ACTION:
    moveObjectAlongPath

REQUIRED PARAMETERS:
    handle (int): handle of the object to move along the path
    path (table): path pose data (required if pathHandle not given)
    pathHandle (int): handle of the path object (required if path data not given)

OPTIONAL PARAMETERS:
    vel (float): velocity (default: 0.1)
    startPoint (float): path starting point (default: 0)
    timeout (float): maximum execution time in seconds after which the action will abort
    maxDistance (float): maximum distance to another object that will chase the handle (e.g. a robot); if distance is above this threshold, tracking will pause to allow the robot to catch up; if using this parameter, also trackingHandle is required
    trackingHandle (int): handle of the robot, for using with maxDistance

DESCRIPTION:
    Move an object (e.g. a dummy) along a path.
    Can use an additional object (trackingHandle) to monitor the distance from handle, and if greater than maxDistance, pause tracking until the distance goes again below threshold.

EXAMPLE:
    require 'actions/moveObjectAlongPath'

    actionServer:send('moveObjectAlongPath', {handle = sim.getObject '/Dummy', pathHandle = sim.getObject '/Path'})

--]]

sim = require('sim')

function action_moveObjectAlongPath_accept(cmd)
    assert(type(cmd.params.handle) == 'number' and sim.isHandle(cmd.params.handle), 'param "handle" required')
    if cmd.params.path == nil and cmd.params.pathHandle then
        cmd.params.path = sim.unpackDoubleTable(sim.readCustomBufferData(cmd.params.pathHandle, 'PATH'))
    end
    assert(type(cmd.params.path) == 'table' and type(cmd.params.path[1]) == 'number', 'param "path" required')
    cmd.params.pathHandle = cmd.params.pathHandle or sim.handle_world
    cmd.params.vel = cmd.params.vel or 0.1
    assert(type(cmd.params.vel) == 'number')
    if cmd.params.maxDistance then
        assert(type(cmd.params.maxDistance) == 'number')
        assert(type(cmd.params.trackingHandle) == 'number' and sim.isHandle(cmd.params.trackingHandle), 'param "trackingHandle" required when "maxDistance" is set')
    end
    return true
end

function action_moveObjectAlongPath_execute(cmd)
    local m = Matrix(-1, 7, cmd.params.path)
    if not cmd.state.pathPositions then
        cmd.state.pathPositions = m:slice(1, 1, m:rows(), 3):data()
    end
    if not cmd.state.pathQuaternions then
        cmd.state.pathQuaternions = m:slice(1, 4, m:rows(), 7):data()
    end
    if not cmd.state.pathLengths or not cmd.state.totalLength then
        cmd.state.pathLengths, cmd.state.totalLength = sim.getPathLengths(cmd.state.pathPositions, 3)
    end

    if cmd.params.startPoint then
        cmd.state.posAlongPath = math.min(cmd.state.totalLength, cmd.params.startPoint)
    else
        local p = sim.getObjectPosition(cmd.params.handle, cmd.params.pathHandle)
        cmd.state.posAlongPath = sim.getClosestPosOnPath(cmd.state.pathPositions, cmd.state.pathLengths, p)
    end

    return true
end

function action_moveObjectAlongPath_tick(cmd)
    local t = sim.getSimulationTime()
    if not cmd.state.t then
        cmd.state.t = t
        return
    end

    local d = 0
    if cmd.params.maxDistance then
        d = Vector(sim.getObjectPosition(cmd.params.handle, cmd.params.trackingHandle)):norm()
    end
    local v = cmd.params.vel
    if cmd.params.maxDistance then
        if d >= cmd.params.maxDistance then
            v = 0
        elseif d >= 0.8 * cmd.params.maxDistance then
            local dn = (d - 0.8 * cmd.params.maxDistance) / (0.2 * cmd.params.maxDistance)
            v = cmd.params.vel * (1 - dn)
        end
    end

    cmd.state.posAlongPath = math.min(cmd.state.totalLength, cmd.state.posAlongPath + v * (t - cmd.state.t))
    cmd.state.t = t
    local p = sim.getPathInterpolatedConfig(cmd.state.pathPositions, cmd.state.pathLengths, cmd.state.posAlongPath)
    local q = sim.getPathInterpolatedConfig(cmd.state.pathQuaternions, cmd.state.pathLengths, cmd.state.posAlongPath, nil, {2, 2, 2, 2})
    sim.setObjectPosition(cmd.params.handle, p, cmd.params.pathHandle)
    sim.setObjectQuaternion(cmd.params.handle, q, cmd.params.pathHandle)

    if math.abs(cmd.state.posAlongPath - cmd.state.totalLength) < 0.001 then
        return {transition = 'succeed'}
    end

    if cmd.params.timeout and sim.getSystemTime() >= (cmd.state.startTime + cmd.params.timeout) then
        return {transition = 'abort'}
    end
end

function action_moveObjectAlongPath_cancel(cmd)
    return true
end
