function sysCall_info()
    return {autoStart = false, menu = 'Geometry / Mesh\nMesh decimation...'}
end

function sysCall_nonSimulation()
    if leaveNow then
        simUI.destroy(ui)
        ui = nil
        if params then
            local generated = {}
            if #params.sel > 0 then
                local cnt = 1
                for i = 1, #params.sel do
                    local h = params.sel[i]
                    local vert, ind = sim.getShapeMesh(h)
                    sim.addLog(sim.verbosity_scriptinfos, string.format('Generating a decimated equivalent shape (%i/%i)... (input shape has %i triangular faces)', cnt, #params.sel, #ind / 3)) 
                    local nh = getDecimated(h, params, params.adoptColor)
                    generated[#generated + 1] = nh
                    local vert, ind = sim.getShapeMesh(nh)
                    sim.addLog(sim.verbosity_scriptinfos, string.format('Done. (output shape has %i triangular faces)', #ind / 3))
                    cnt = cnt + 1
                end
                sim.announceSceneContentChange()
            end
            sim.setObjectSel(generated)
        else
            if not abort then
                simUI.msgBox(simUI.msgbox_type.info, simUI.msgbox_buttons.ok, "Mesh Decimation", 'The resulting selection is effectively empty, indicating it does not contain any non-convex shapes that meet the specified inclusion criteria..')
                sim.setObjectSel({})
            end
        end
        return {cmd = 'cleanup'} 
    end
end
    
function sysCall_init()
    sim = require('sim')
    simUI = require('simUI')
    simOpenMesh = require('simOpenMesh')

    local sel = sim.getObjectSel()
    if #sel == 0 or sim.getSimulationState() ~= sim.simulation_stopped then
        simUI.msgBox(simUI.msgbox_type.info, simUI.msgbox_buttons.ok, "Mesh Decimation", 'Make sure that at least one object is selected, and that simulation is not running.')
    else
        ui = simUI.create(
          [[<ui title="Mesh Decimation" closeable="true" on-close="onClose" modal="true">
            <group flat="true" content-margins="0,0,0,0" layout="form">
                <label text="percentage:" />
                <spinbox id="${percentage}" minimum="5" maximum="95" value="80" step="5" on-change="updateUi" />
            </group>
            <checkbox id="${model_shapes}" text="include model shapes" checked="false" on-change="updateUi" />
            <checkbox id="${hidden_shapes}" text="exclude hidden shapes" checked="false" on-change="updateUi" />
            <checkbox id="${adopt_colors}" text="adopt colors" checked="true" on-change="updateUi" />
            <button id="${gen}" text="Generate" on-click="initGenerate" />
        </ui>]]
             )
    end
end

function onClose()
    leaveNow = true
    abort = true
end

function updateUi()
end

function initGenerate()
    local includeModelShapes = simUI.getCheckboxValue(ui, model_shapes) > 0
    local excludeHiddenShapes = simUI.getCheckboxValue(ui, hidden_shapes) > 0
    local adoptColors = simUI.getCheckboxValue(ui, adopt_colors) > 0
    local s = sim.getObjectSel()
    local selMap = {}
    for i = 1, #s do
        local h = s[i]
        if sim.getModelProperty(h) == sim.modelproperty_not_model or not includeModelShapes then
            selMap[h] = true
        else
            local tree = sim.getObjectsInTree(h, sim.sceneobject_shape)
            for j = 1, #tree do
                selMap[tree[j]] = true
            end
        end
    end
    local sel = {}
    for obj, v in pairs(selMap) do
        if sim.getObjectType(obj) == sim.sceneobject_shape then
            if not excludeHiddenShapes or (sim.getObjectInt32Param(obj, sim.objintparam_visible) > 0) then
                sel[#sel + 1] = obj
            end
        end
    end
    
    leaveNow = true
    if #sel > 0 then
        params = {adoptColor = adoptColors, sel = sel}
        params.percentage = tonumber(simUI.getSpinboxValue(ui, percentage)) / 100.0
    end
end

function extractSimpleShapes(shapes)
    local retVal = {}
    for i = 1, #shapes do
        local shape = shapes[i]
        local t = sim.getShapeGeomInfo(shape)
        if t & 1 > 0 then
            local nshapes = sim.ungroupShape(shape)
            retVal = table.add(retVal, extractSimpleShapes(nshapes))
        else
            retVal[#retVal + 1] = shape
        end
    end
    return retVal
end

function getDecimated(shapeHandle, params, adoptColor)
    local allShapes = sim.copyPasteObjects({shapeHandle}, 2|4|8|16|32)
    allShapes = extractSimpleShapes(allShapes)
    local newShapes = {}
    for i = 1, #allShapes do
        local shape = allShapes[i]
        local nshape = simOpenMesh.decimate(shape, params)
        sim.relocateShapeFrame(nshape, {0, 0, 0, 0, 0, 0, 0})
        if adoptColor then
            sim.setObjectColor(nshape, 0, sim.colorcomponent_ambient_diffuse, sim.getObjectColor(shape, 0, sim.colorcomponent_ambient_diffuse))
            local angle = sim.getObjectFloatParam(shape, sim.shapefloatparam_shading_angle)
            sim.setObjectFloatParam(nshape, sim.shapefloatparam_shading_angle, angle)
        end
        newShapes[#newShapes + 1] = nshape
    end
    sim.removeObjects(allShapes)
    local newShape
    if #newShapes > 1 then
        newShape = sim.groupShapes(newShapes)
    else
        newShape = newShapes[1]
    end

    -- Pose, BB:
    local pose = sim.getObjectPose(shapeHandle)
    sim.relocateShapeFrame(newShape, pose)
    sim.alignShapeBB(newShape, {0, 0, 0, 0, 0, 0, 0})

    -- Dynamic aspects:
    sim.setObjectInt32Param(newShape, sim.shapeintparam_respondable, sim.getObjectInt32Param(shapeHandle, sim.shapeintparam_respondable))
    sim.setObjectInt32Param(newShape, sim.shapeintparam_respondable_mask, sim.getObjectInt32Param(shapeHandle, sim.shapeintparam_respondable_mask))
    sim.setObjectInt32Param(newShape, sim.shapeintparam_static, sim.getObjectInt32Param(shapeHandle, sim.shapeintparam_static))
    sim.setShapeMass(newShape, sim.getShapeMass(shapeHandle))
    local inertiaMatrix, com = sim.getShapeInertia(shapeHandle)
    sim.setShapeInertia(newShape, inertiaMatrix, com)
    
    -- Various:
    sim.setObjectAlias(newShape, sim.getObjectAlias(shapeHandle) .. '_decimated')
    
    return newShape
end
