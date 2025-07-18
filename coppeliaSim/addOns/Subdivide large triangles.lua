sim = require 'sim'

function sysCall_info()
    return {autoStart = false, menu = 'Geometry / Mesh\nSubdivide large triangles...'}
end

function sysCall_init()
    simUI = require 'simUI'
    if sim.getSimulationState() ~= sim.simulation_stopped then return {cmd = 'cleanup'} end

    sel = sim.getObjectSel()
    if #sel > 0 then
        for _, h in ipairs(sel) do
            if sim.getObjectType(h) ~= sim.sceneobject_shape then
                error 'Can only operate on shape objects'
            end
        end
    else
        error 'At least one shape object should be selected'
    end

    ui = simUI.create [[<ui on-close="abort" modal="true" title="Subdivide large triangles add-on">
        <label id="1" text="Maximum triangle edge length:" />
        <spinbox id="2" minimum="0" maximum="1000" step="0.001" value="0.01" suffix="m" />
        <button text="Subdivide" on-click="go" />
    </ui>]]
end

function sysCall_cleanup()
end

function sysCall_addOnScriptSuspend()
    return {cmd = 'cleanup'}
end

function sysCall_beforeSimulation()
    return {cmd = 'cleanup'}
end

function sysCall_beforeInstanceSwitch()
    return {cmd = 'cleanup'}
end

function sysCall_nonSimulation()
    if leaveNow then return {cmd = 'cleanup'} end
end

function abort()
    leaveNow = true
end

function go()
    simIGL = require 'simIGL'
    local threshold = simUI.getSpinboxValue(ui, 2)
    local result = {}
    for _, h in ipairs(sel) do
        local culling = sim.getObjectInt32Param(h, sim.shapeintparam_culling)
        local edges = sim.getObjectInt32Param(h, sim.shapeintparam_edge_visibility)
        local _, colad = sim.getShapeColor(h, nil, sim.colorcomponent_ambient_diffuse)
        local _, colsp = sim.getShapeColor(h, nil, sim.colorcomponent_specular)
        local _, colem = sim.getShapeColor(h, nil, sim.colorcomponent_emission)
        local mesh = simIGL.getMesh(h)
        mesh = simIGL.adaptiveUpsample(mesh, threshold)
        h = sim.createShape(1 + 2 * edges, math.pi / 8, mesh.vertices, mesh.indices)
        sim.setShapeColor(h, nil, sim.colorcomponent_ambient_diffuse, colad)
        sim.setShapeColor(h, nil, sim.colorcomponent_specular, colsp)
        sim.setShapeColor(h, nil, sim.colorcomponent_emission, colem)
        sim.setObjectInt32Param(h, sim.shapeintparam_culling, culling)
        table.insert(result, h)
    end
    sim.removeObjects(sel)
    sim.setObjectSel(result)
    leaveNow = true
end
