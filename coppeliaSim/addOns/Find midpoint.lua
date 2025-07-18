sim = require 'sim'

function sysCall_info()
    return {autoStart = false, menu = 'Geometry / Mesh\nFind midpoint'}
end

function sysCall_init()
    simUI = require 'simUI'
    simIGL = require 'simIGL'
    if sim.getSimulationState() ~= sim.simulation_stopped then return {cmd = 'cleanup'} end
    sim.addLog(
        sim.verbosity_scriptinfos,
        "This tool finds midpoint between two objects (start the addon with the two objects selected) or between two points (first clicked vertex/dummy and second clicked vertex/dummy). Hold shift to create two evenly spaced midpoints. Use sim.setNamedInt32Param('findMidpoint.n',3) to change the number of midpoints created when shift is held to e.g. 3."
    )

    -- start with two objects selected to compute the midpoint between those two
    local sel = sim.getObjectSel()
    if #sel == 2 then
        local firstPoint = Vector(sim.getObjectPosition(sel[1]))
        local secondPoint = Vector(sim.getObjectPosition(sel[2]))
        local objs = makeMidpoints(firstPoint, secondPoint)
        sim.setObjectSel(objs)
        return {cmd = 'cleanup'}
    elseif #sel == 0 then
        interactive = true
        sim.broadcastMsg {
            id = 'pointSampler.enable',
            data = {
                key = 'findMidpoint',
                vertex = true,
                dummy = true,
                snapToClosest = true,
                hover = true,
            },
        }
        pts = sim.addDrawingObject(sim.drawing_spherepts | sim.drawing_itemsizes, 0.01, 0, -1, 100, {1, 0, 0})
    else
        error 'Incorrect number of objects selected'
    end
end

function sysCall_cleanup()
    if interactive then
        sim.removeDrawingObject(pts)
        sim.broadcastMsg {id = 'pointSampler.disable', data = {key = 'findMidpoint'}}
    end
end

function sysCall_addOnScriptSuspend()
    return {cmd = 'cleanup'}
end

function sysCall_msg(event)
    if not event.data or not event.data.key or event.data.key ~= 'findMidpoint' then return end
    local point = nil
    if event.id == 'pointSampler.click' or event.id == 'pointSampler.hover' then
        if event.data.dummy then
            point = sim.getObjectPosition(event.data.dummy)
        else
            point = event.data.vertexCoords
        end
    end
    if not point then return end
    if event.id == 'pointSampler.click' then
        if not firstPoint then
            firstPoint = Vector(point)
            sim.broadcastMsg {
                id = 'pointSampler.setFlags',
                data = {key = 'findMidpoint', segmentSource = point},
            }
        else
            secondPoint = Vector(point)
            makeMidpoints(firstPoint, secondPoint)
            return {cmd = 'cleanup'}
        end
    elseif event.id == 'pointSampler.hover' and firstPoint then
        sim.addDrawingObjectItem(pts, nil)
        local p = Vector(point)
        local n = simUI.getKeyboardModifiers().shift and
                      math.max(1, sim.getNamedInt32Param('findMidpoint.n') or 2) or 1
        local sz = math.min(0.01, (p - firstPoint):norm() / n / 4)
        for i, m in ipairs(getMidpoints(firstPoint, p)) do
            sim.addDrawingObjectItem(pts, {m[4], m[8], m[12], sz})
        end
    end
end

function sysCall_beforeSimulation()
    return {cmd = 'cleanup'}
end

function sysCall_beforeInstanceSwitch()
    return {cmd = 'cleanup'}
end

function getMidpoints(a, b)
    local d = b - a
    local n = simUI.getKeyboardModifiers().shift and
                  math.max(1, sim.getNamedInt32Param('findMidpoint.n') or 2) or 1
    local midPoints = {}
    for i = 1, n do
        local midPoint = a + d * i / (n + 1)
        table.insert(midPoints, simIGL.pointNormalToMatrix(midPoint, d))
    end
    return midPoints
end

function makeMidpoints(a, b)
    local objs = {}
    for i, m in ipairs(getMidpoints(a, b)) do
        dummy = sim.createDummy(0.01)
        sim.setObjectAlias(dummy, 'Midpoint')
        sim.setObjectMatrix(dummy, m)
        table.insert(objs, dummy)
    end
    sim.announceSceneContentChange()
    return objs
end
