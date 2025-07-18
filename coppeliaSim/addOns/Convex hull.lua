function sysCall_info()
    return {autoStart = false, menu = 'Geometry / Mesh\nConvex hull...'}
end

function sysCall_init()
    sim = require('sim')
    simUI = require('simUI')
    simConvex = require('simConvex')

    local sel = sim.getObjectSel()
    if #sel == 0 or sim.getSimulationState() ~= sim.simulation_stopped then
        simUI.msgBox(simUI.msgbox_type.info, simUI.msgbox_buttons.ok, "Convex Hull Generator", 'Make sure that at least one object is selected, and that simulation is not running.')
        return {cmd = 'cleanup'}
    end
    createUi()
end

function sysCall_nonSimulation()
    if leaveNow then
        simUI.destroy(ui)
        if params then
            compute()
        end
        return {cmd = 'cleanup'}
    end
end

function compute()
    local generated = {}

    local firstShape = nil -- shape appearance source
    for i, obj in ipairs(params.sel) do
        if sim.getObjectType(obj) == sim.sceneobject_shape then
            firstShape = obj
            break
        end
    end

    if params.mode == 'perObject' or params.mode == 'perComponent' then
        for i, obj in ipairs(params.sel) do
            local t = sim.getObjectType(obj)
            if t == sim.sceneobject_shape or t == sim.sceneobject_pointcloud or t == sim.sceneobject_octree then
                if t == sim.sceneobject_shape and params.mode == 'perComponent' then
                    local tt = sim.getShapeGeomInfo(obj)
                    if (tt & 1) > 0 then
                        local shapes = extractSimpleShapes(sim.copyPasteObjects({obj}, 2|4|8|16|32))
                        for j, shape in ipairs(shapes) do
                            local h = simConvex.hull({shape}, params.growth)
                            table.insert(generated, h)
                        end
                        sim.removeObjects(shapes)
                    else
                        local h = simConvex.hull({obj}, params.growth)
                        table.insert(generated, h)
                    end
                else
                    local h = simConvex.hull({obj}, params.growth)
                    table.insert(generated, h)
                end
            end
        end
    else
        -- perSelection
        if #params.sel > 0 then
            local h = simConvex.hull(params.sel, params.growth)
            table.insert(generated, h)
        end
    end
    local convex = true
    local faces = 0
    for i, h in ipairs(generated) do
        if (sim.getShapeGeomInfo(h) & 4) == 0 then
            convex = false
        end
        -- Pose, BB:
        sim.relocateShapeFrame(h, {0, 0, 0, 0, 0, 0, 0})
        sim.alignShapeBB(h, {0, 0, 0, 0, 0, 0, 0})

        -- Centered inertia, kep defaults otherwise:
        local im, com = sim.getShapeInertia(h)
        sim.setShapeInertia(h, im, {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0})

        -- Various:
        sim.setObjectAlias(h, 'convexHull')
        if params.preserveAppearance and firstShape then
            sim.setShapeAppearance(h, sim.getShapeAppearance(firstShape))
        else
            sim.setObjectFloatParam(h, sim.shapefloatparam_shading_angle, 45.0 * math.pi / 180.0)
        end
        local vert, ind = sim.getShapeMesh(h)
        faces = faces + #ind / 3
    end
    if not convex then
        sim.addLog(sim.verbosity_scripterrors, 'One or more of the generated shapes is not convex.')
    end
    sim.setObjectSel(generated)

    if #generated > 0 then
        sim.announceSceneContentChange()
        sim.addLog(sim.verbosity_scriptinfos, string.format('Generated %i convex hull(s) (with a total of %i triangular faces)', #generated, faces))
    else
        simUI.msgBox(simUI.msgbox_type.info, simUI.msgbox_buttons.ok, "Convex Hull Generator", 'The resulting selection is effectively empty...')
        sim.setObjectSel({})
    end
end

function createUi()
    ui = simUI.create([[<ui title="Convex Hull Generator" closeable="true" on-close="onClose" modal="true">
        <radiobutton text="Generate one single hull" id="${perSelection}" checked="true" />
        <radiobutton text="Generate one hull per object" id="${perObject}" checked="false" />
        <radiobutton text="Generate one hull per object component" id="${perComponent}" checked="false"/>
        <checkbox id="${modelObjects}" text="Include model objects" checked="false" />
        <checkbox id="${hiddenObjects}" text="Exclude hidden objects" checked="false" />
        <group flat="true" content-margins="0,0,0,0" layout="form">
            <label text="Growth:" />
            <spinbox id="${growth}" minimum="0.0" maximum="100.0" value="0.0" step="0.02" />
        </group>
        <checkbox id="${preserveAppearance}" text="Preserve original shape appearance" checked="false" />
        <button text="Generate" on-click="initGenerate" />
    </ui>]])
end

function onClose()
    leaveNow = true
end

function initGenerate()
    leaveNow = true
    params = {sel = {}}

    local includeModelObjects = simUI.getCheckboxValue(ui, modelObjects) > 0
    local excludeHiddenObjects = simUI.getCheckboxValue(ui, hiddenObjects) > 0
    local selMap = {}
    for i, h in ipairs(sim.getObjectSel()) do
        if sim.getModelProperty(h) == sim.modelproperty_not_model or not includeModelObjects then
            selMap[h] = true
        else
            for j, hj in ipairs(sim.getObjectsInTree(h, sim.sceneobject_shape)) do
                selMap[hj] = true
            end
        end
    end
    for obj, v in pairs(selMap) do
        if not excludeHiddenObjects or sim.getObjectInt32Param(obj, sim.objintparam_visible) > 0 then
            table.insert(params.sel, obj)
        end
    end

    if simUI.getRadiobuttonValue(ui, perSelection) == 1 then
        params.mode = 'perSelection'
    elseif simUI.getRadiobuttonValue(ui, perObject) == 1 then
        params.mode = 'perObject'
    elseif simUI.getRadiobuttonValue(ui, perComponent) == 1 then
        params.mode = 'perComponent'
    end

    params.growth = simUI.getSpinboxValue(ui, growth)

    params.preserveAppearance = simUI.getCheckboxValue(ui, preserveAppearance) > 0
end

function extractSimpleShapes(shapes)
    local retVal = {}
    for i, shape in ipairs(shapes) do
        local t = sim.getShapeGeomInfo(shape)
        if (t & 1) > 0 then
            local nshapes = sim.ungroupShape(shape)
            retVal = table.add(retVal, extractSimpleShapes(nshapes))
        else
            table.insert(retVal, shape)
        end
    end
    return retVal
end
