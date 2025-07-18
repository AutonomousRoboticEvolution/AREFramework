local simConvex = loadPlugin('simConvex');

function simConvex.hull(handles, growth)
    local vert = {}
    for _, h in ipairs(handles) do
        local t = sim.getObjectType(h)
        if t == sim.sceneobject_shape then
            local v = sim.getShapeMesh(h)
            local m = sim.getObjectMatrix(h)
            v = sim.multiplyVector(m, v)
            for _, x in ipairs(v) do table.insert(vert, x) end
        elseif t == sim.sceneobject_dummy then
            local p = sim.getObjectPosition(h)
            for _, x in ipairs(p) do table.insert(vert, x) end
        elseif t == sim.sceneobject_pointcloud then
            local v = sim.getPointCloudPoints(h)
            local m = sim.getObjectMatrix(h)
            v = sim.multiplyVector(m, v)
            for _, x in ipairs(v) do table.insert(vert, x) end
        elseif t == sim.sceneobject_octree then
            local v = sim.getOctreeVoxels(h)
            local vsh = 0.5 * sim.getObjectFloatParam(h, sim.octreefloatparam_voxelsize)
            local m = sim.getObjectMatrix(h)
            local vx = Vector{m[1], m[5], m[9]} * vsh
            local vy = Vector{m[2], m[6], m[10]} * vsh
            local vz = Vector{m[3], m[7], m[11]} * vsh
            for x = -1, 1, 2 do
                for y = -1, 1, 2 do
                    for z = -1, 1, 2 do
                        local d = vx * x + vy * y + vz * z
                        local m2 = table.clone(m)
                        m2[4] = m2[4] + d[1]
                        m2[8] = m2[8] + d[2]
                        m2[12] = m2[12] + d[3]
                        local v2 = sim.multiplyVector(m2, v)
                        for _, xx in ipairs(v2) do table.insert(vert, xx) end
                    end
                end
            end
        else
            sim.addLog(sim.verbosity_warnings, 'unsupported object type: ' .. t)
        end
    end
    if #vert == 0 then error('empty input') end
    local v, i = simConvex._ghull(vert, growth)
    local h = sim.createShape(0, 0.0, v, i)
    return h
end

function simConvex.qhull(vertices, growth)
    local vert, ind = simConvex._ghull(vertices, growth)
    return vert, ind
end

function simConvex._ghull(vertices, growth)
    growth = growth or 0.0
    local vert, ind = sim._qhull(vertices)
    if growth > 0.0 then
        local nvert = {}
        for j = 0, #ind / 3 - 1 do
            local indd = {ind[3 * j + 1], ind[3 * j + 2], ind[3 * j + 3]}
            local w = {Vector({vert[3 * indd[1] + 1], vert[3 * indd[1] + 2], vert[3 * indd[1] + 3]}),
                       Vector({vert[3 * indd[2] + 1], vert[3 * indd[2] + 2], vert[3 * indd[2] + 3]}),
                       Vector({vert[3 * indd[3] + 1], vert[3 * indd[3] + 2], vert[3 * indd[3] + 3]}) }
            local v12 = w[1] - w[2]
            local v13 = w[1] - w[3]
            local n = v12:cross(v13)
            n = n:normalized() * growth
            for k = -1.0, 1.0, 2.0 do
                for l = 1, 3 do
                    local vv = w[l] + n * k
                    nvert[#nvert + 1] = vv[1]
                    nvert[#nvert + 1] = vv[2]
                    nvert[#nvert + 1] = vv[3]
                end
            end
        end
        vert, ind = sim._qhull(nvert) -- sim.getQHull(nvert)
    end
    return vert, ind
end

return simConvex
