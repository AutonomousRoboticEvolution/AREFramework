-- For backward compatibility
simConvex = require('simConvex')
local simQHull = {}

function simQHull.computeShape(handles)
    return simConvex.hull(handles)
end

function simQHull.compute(vertices, genIndices)
    return simConvex._qhull(vertices, genIndices)
end

return simQHull
