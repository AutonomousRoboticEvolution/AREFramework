local simIGL = loadPlugin 'simIGL';

-- @fun getMesh get mesh data of a given shape in the format used by simIGL functions
-- @arg int h the handle of the shape
-- @arg {type='table',default={}} options options
-- @ret table mesh mesh object
function simIGL.getMesh(h, options)
    local v, i, n = sim.getShapeMesh(h)
    local m = sim.getObjectMatrix(h)
    v = sim.multiplyVector(m, v)
    return {vertices = v, indices = i}
end

-- @fun meshBooleanShape convenience wrapper for simIGL.meshBoolean to operate on shapes directly
-- @arg table.int handles the handle of the input shapes
-- @arg int op the operation (see simIGL.boolean_op)
-- @ret int handleResult the handle of the resulting shape
function simIGL.meshBooleanShape(handles, op)
    if #handles < 2 then error('not enough input shapes') end
    local meshes = map(simIGL.getMesh, handles)
    local result = simIGL.meshBoolean(meshes[1], meshes[2], op)
    for i = 3, #meshes do result = simIGL.meshBoolean(result, meshes[i], op) end
    if #result.vertices == 0 then return end
    result = sim.createShape(1, 0, result.vertices, result.indices)
    sim.setShapeAppearance(result, sim.getShapeAppearance(handles[1]))
    sim.alignShapeBB(result, {0, 0, 0, 0, 0, 0, 1})
    sim.relocateShapeFrame(result, {0, 0, 0, 0, 0, 0, 0})
    return result
end

-- @fun convexHullShape convenience wrapper for simIGL.convexHull to operate on shapes directly
-- @arg table.int handles the handle of the input shapes
-- @ret int handleResult the handle of the resulting shape
function simIGL.convexHullShape(handles)
    local vert = {}
    local app = {
        edges = false,
        culling = false,
        shadingAngle = math.pi / 8,
        color = {
            ambientDiffuse = {0.85, 0.85, 0.85},
            specular = {0, 0, 0},
            emission = {0, 0, 0},
            transparency = {0},
        },
    }
    for i, h in ipairs(handles) do
        local toadd = {}
        local t = sim.getObjectType(h)
        if t == sim.sceneobject_shape then
            app = sim.getShapeAppearance(h)
            local m = simIGL.getMesh(h)
            toadd = m.vertices
        elseif t == sim.sceneobject_dummy then
            toadd = sim.getObjectPosition(h, sim.handle_world)
        else
            error('unsupported object type')
        end
        if #vert > 0 then
            for _, x in ipairs(toadd) do table.insert(vert, x) end
        else
            vert = toadd
        end
    end
    if #vert == 0 then error('empty input') end
    local m = simIGL.convexHull(vert)
    local h = sim.createShape(1, 0, m.vertices, m.indices)
    sim.setShapeAppearance(h, app)
    return h
end

-- @fun pointNormalToMatrix return the transform matrix (table of 12 values) from point and normal
-- @arg table point (3D vector)
-- @arg table normal (3D vector)
-- @ret table matrix the resulting transform matrix (table of 12 values)
function simIGL.pointNormalToMatrix(point, normal)
    local m = sim.buildIdentityMatrix()
    m[4] = point[1]
    m[8] = point[2]
    m[12] = point[3]
    if normal[1] < 0.99 then
        local z = Vector3(normal):normalized()
        local x = Vector3({1, 0, 0})
        local y = z:cross(x):normalized()
        local x = y:cross(z)
        m[1] = x[1]; m[5] = x[2]; m[9] = x[3];
        m[2] = y[1]; m[6] = y[2]; m[10] = y[3];
        m[3] = z[1]; m[7] = z[2]; m[11] = z[3];
    else
        m[1] = 0; m[5] = 1; m[9] = 0;
        m[2] = 0; m[6] = 0; m[10] = 1;
        m[3] = 1; m[7] = 0; m[11] = 0;
    end
    return m
end

-- @fun rayTest perform a ray test, from given origin towards every of the points, and return a new array of points.
-- @arg table origin (3D vector)
-- @arg table points one or more points to test
-- @arg {type='int',nullable=true,default='NIL'} proximitySensorHandle handle of a ray type proximity sensor, or nil in which case it will be created
-- @ret table resultPoints the resulting points of the ray test
function simIGL.rayTest(origin, points, proximitySensorHandle)
    local removeSensor = false
    if proximitySensorHandle == nil then
        local zoffset = 0
        local zrange = 10
        proximitySensorHandle = sim.createProximitySensor(
            sim.proximitysensor_ray, 16, 0
            +1 -- the sensor will be explicitly handled
            +4 -- the detection volume is not shown
            , {0, 0, 0, 0, 0, 0, 0, 0}
            , {zoffset, zrange, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
        )
        removeSensor = true
    end
    local originVec = Vector(origin)
    local result = {}
    for i, point in ipairs(table.batched(points, 3)) do
        local m = simIGL.pointNormalToMatrix(origin, (Vector(point) - originVec):normalized())
        sim.setObjectMatrix(proximitySensorHandle, m)
        local r, d, pt, h, n = sim.handleProximitySensor(proximitySensorHandle)
        pt = sim.multiplyVector(m, pt)
        for _, x in ipairs(pt) do table.insert(result, x) end
    end
    if removeSensor then
        sim.removeObjects {proximitySensorHandle}
    end
    return result
end

-- @fun drawMesh draw a mesh using drawing objects
-- @arg table mesh the mesh data, as returned by simIGL.getMesh
-- @arg table opts various options (color: table3, lineWidth: int, offset: table3, dwo: int or nil, it will be created)
-- @ret table dwo the drawing object that has been used to draw
function simIGL.drawMesh(mesh, opts)
    opts = opts or {}
    opts.color = opts.color or {0, 0, 1}
    opts.lineWidth = opts.lineWidth or 2
    opts.offset = opts.offset or {0, 0, 0}

    local V = Matrix(-1, 3, mesh.vertices)
    local F = Matrix(-1, 3, mesh.indices)
    local offset = Vector(opts.offset):t()

    if opts.dwo == nil then
        opts.dwo = sim.addDrawingObject(sim.drawing_lines, opts.lineWidth, 0, -1, #mesh.indices, opts.color)
    end
    if opts.dwo ~= false then
        for i, tri in ipairs(F) do
            for _, ij in ipairs{{1, 2}, {1, 3}, {2, 3}} do
                local vi = V[tri[ij[1]] + 1] + offset
                local vj = V[tri[ij[2]] + 1] + offset
                sim.addDrawingObjectItem(opts.dwo, Matrix:horzcat(vi, vj):data())
            end
        end
    end

    return opts.dwo
end

(require 'simIGL-typecheck')(simIGL)

return simIGL
