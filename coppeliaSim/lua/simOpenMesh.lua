local simOpenMesh = loadPlugin('simOpenMesh');

function simOpenMesh.getDecimatedShape(shapeHandle, prop)
    -- for backward compatibility
    local vertices, indices = sim.getShapeMesh(shapeHandle)
    local m = sim.getObjectMatrix(shapeHandle, -1)
    for i = 1, #vertices / 3, 1 do
        local v = {vertices[3 * (i - 1) + 1], vertices[3 * (i - 1) + 2], vertices[3 * (i - 1) + 3]}
        v = sim.multiplyVector(m, v)
        vertices[3 * (i - 1) + 1] = v[1]
        vertices[3 * (i - 1) + 2] = v[2]
        vertices[3 * (i - 1) + 3] = v[3]
    end
    local currentTriangleCount = #indices // 3
    local afterTriangleCount = math.max(1, currentTriangleCount * prop)
    local nvertices, nindices = simOpenMesh.getDecimated(vertices, indices, 0, afterTriangleCount)
    local newShape = sim.createShape(0, 0, nvertices, nindices)
    local _, colad = sim.getShapeColor(shapeHandle, nil, sim.colorcomponent_ambient_diffuse)
    sim.setShapeColor(newShape, nil, sim.colorcomponent_ambient_diffuse, colad)
    local _, colsp = sim.getShapeColor(shapeHandle, nil, sim.colorcomponent_specular)
    sim.setShapeColor(newShape, nil, sim.colorcomponent_specular, colsp)
    local _, colem = sim.getShapeColor(shapeHandle, nil, sim.colorcomponent_emission)
    sim.setShapeColor(newShape, nil, sim.colorcomponent_emission, colem)
    sim.reorientShapeBoundingBox(newShape, shapeHandle)
    local nm = sim.getObjectAlias(shapeHandle)
    local p = sim.getObjectParent(shapeHandle)
    sim.setObjectParent(newShape, p, true)
    local children = sim.getObjectsInTree(shapeHandle, sim.handle_all, 1 + 2)
    for i = 1, #children, 1 do sim.setObjectParent(children[i], newShape, true) end
    sim.removeObjects({shapeHandle})
    sim.setObjectAlias(newShape, nm)
    return newShape
end

return simOpenMesh
