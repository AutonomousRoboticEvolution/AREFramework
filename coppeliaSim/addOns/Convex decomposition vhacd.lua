function sysCall_info()
    return {autoStart = false, menu = 'Geometry / Mesh\nConvex decomposition\nV-HACD...'}
end

function sysCall_nonSimulation()
    if leaveNow then
        simUI.destroy(ui)
        ui = nil
        if params then
            local generated = {}
            local convexSel = params.convexSel
            params.convexSel = nil
            if #params.sel > 0 then
                local cnt = 1
                for i = 1, #params.sel do
                    local h = params.sel[i]
                    local vert, ind = sim.getShapeMesh(h)
                    sim.addLog(sim.verbosity_scriptinfos, string.format('Generating V-HACD convex decomposed equivalent shape (%i/%i)... (input shape has %i triangular faces)', cnt, #params.sel, #ind / 3))
                    local nh = getConvexDecomposed(h, params, params.adoptColor)
                    generated[#generated + 1] = nh
                    local vert, ind = sim.getShapeMesh(nh)
                    sim.addLog(sim.verbosity_scriptinfos, string.format('Done. (output shape has %i triangular faces)', #ind / 3))
                    cnt = cnt + 1
                end
                sim.announceSceneContentChange()
            end
            for i = 1, #convexSel do
                -- Some shapes are already convex. Simply duplicate them:
                generated[#generated + 1] = sim.copyPasteObjects({convexSel[i]}, 2|4|8|16|32)[1]
            end
            sim.setObjectSel(generated)
            local convex = true
            for i = 1, #generated do
                if (sim.getShapeGeomInfo(generated[i]) & 4) == 0 then
                    convex = false
                end
            end
            if not convex then
                sim.addLog(sim.verbosity_scripterrors, 'One or more of the generated shapes is not convex.') 
            end
        else
            if not abort then
                simUI.msgBox(simUI.msgbox_type.info, simUI.msgbox_buttons.ok, "Convex Decomposition", 'The resulting selection is effectively empty, indicating it does not contain any non-convex shapes that meet the specified inclusion criteria..')
                sim.setObjectSel({})
            end
        end
        return {cmd = 'cleanup'} 
    end
end
    
function sysCall_init()
    sim = require('sim')
    simUI = require('simUI')
    simConvex = require('simConvex')
    
    local sel = sim.getObjectSel()
    if #sel == 0 or sim.getSimulationState() ~= sim.simulation_stopped then
        simUI.msgBox(simUI.msgbox_type.info, simUI.msgbox_buttons.ok, "Convex Decomposition", 'Make sure that at least one object is selected, and that simulation is not running.')
    else
        ui = simUI.create(
          [[<ui title="V-HACD Convex Decomposition" closeable="true" on-close="onClose" modal="true">
            <group flat="true" content-margins="0,0,0,0" layout="form">
                <label text="resolution:" />
                <spinbox id="${resolution}" minimum="10000" maximum="64000000" value="100000" step="10000" on-change="updateUi" />
                <label text="concavity:" />
                <spinbox id="${concavity}" minimum="0.0" maximum="1.0" value="0.1" step="0.05" on-change="updateUi" />
                <label text="plane_downsampling:" />
                <spinbox id="${plane_downsampling}" minimum="1" maximum="16" value="4" step="1" on-change="updateUi" />
                <label text="hull_downsampling:" />
                <spinbox id="${hull_downsampling}" minimum="1" maximum="16" value="4" step="1" on-change="updateUi" />
                <label text="alpha:" />
                <spinbox id="${alpha}" minimum="0.0" maximum="1.0" value="0.05" step="0.05" on-change="updateUi" />
                <label text="beta:" />
                <spinbox id="${beta}" minimum="0.0" maximum="1.0" value="0.05" step="0.05" on-change="updateUi" />
                <label text="max_vertices:" />
                <spinbox id="${max_vertices}" minimum="4" maximum="1024" value="64" step="1" on-change="updateUi" />
                <label text="min_volume:" />
                <spinbox id="${min_volume}" minimum="0.0" maximum="0.1" value="0.0001" step="0.01" on-change="updateUi" />
            </group>
            <checkbox id="${pca}" text="pca" checked="false" on-change="updateUi" />
            <checkbox id="${voxels}" text="voxels" checked="true" on-change="updateUi" />
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
    local convexSel = {}
    for obj, v in pairs(selMap) do
        if sim.getObjectType(obj) == sim.sceneobject_shape then
            if not excludeHiddenShapes or (sim.getObjectInt32Param(obj, sim.objintparam_visible) > 0) then
                local t = sim.getShapeGeomInfo(obj)
                if (t & 4) == 0 then
                    -- not convex
                    sel[#sel + 1] = obj
                else
                    convexSel[#convexSel + 1] = obj
                end
            end
        end
    end
    
    leaveNow = true
    if #sel + #convexSel > 0 then
        params = {adoptColor = adoptColors, sel = sel, convexSel = convexSel}

        params.resolution = math.floor(tonumber(simUI.getSpinboxValue(ui, resolution)))
        params.concavity = tonumber(simUI.getSpinboxValue(ui, concavity))
        params.plane_downsampling = math.floor(tonumber(simUI.getSpinboxValue(ui, plane_downsampling)))
        params.hull_downsampling = math.floor(tonumber(simUI.getSpinboxValue(ui, hull_downsampling)))
        params.alpha = tonumber(simUI.getSpinboxValue(ui, alpha))
        params.beta = tonumber(simUI.getSpinboxValue(ui, beta))
        params.max_vertices = math.floor(tonumber(simUI.getSpinboxValue(ui, max_vertices)))
        params.min_volume = tonumber(simUI.getSpinboxValue(ui, min_volume))
        params.pca = simUI.getCheckboxValue(ui, pca) > 0
        params.voxels = simUI.getCheckboxValue(ui, voxels) > 0
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

function getConvexDecomposed(shapeHandle, params, adoptColor)
    local allShapes = sim.copyPasteObjects({shapeHandle}, 2|4|8|16|32)
    allShapes = extractSimpleShapes(allShapes)
    local newShapes = {}
    for i = 1, #allShapes do
        local shape = allShapes[i]
        local parts = simConvex.vhacd(shape, params)
        for j = 1, #parts do
            local nshape = parts[j]
            sim.relocateShapeFrame(nshape, {0, 0, 0, 0, 0, 0, 0})
            if adoptColor then
                sim.setObjectColor(nshape, 0, sim.colorcomponent_ambient_diffuse, sim.getObjectColor(shape, 0, sim.colorcomponent_ambient_diffuse))
                local angle = sim.getObjectFloatParam(shape, sim.shapefloatparam_shading_angle)
                sim.setObjectFloatParam(nshape, sim.shapefloatparam_shading_angle, angle)
            end
            newShapes[#newShapes + 1] = nshape
        end
    end
    sim.removeObjects(allShapes)
    local newShape
    if #newShapes > 1 then
        for i = 1, #newShapes do
        end
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
    sim.setObjectAlias(newShape, sim.getObjectAlias(shapeHandle) .. '_convexDecomposed')
    
    return newShape
end
