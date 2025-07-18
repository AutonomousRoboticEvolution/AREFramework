local codeEditorInfos = [[
int[] convexShapeHandles = simConvex.hacd(int shapeHandle, map params = nil)
int[] convexShapeHandles = simConvex.vhacd(int shapeHandle, map params = nil)
int convexShapeHandle = simConvex.hull(int[] objectHandles, float growth = 0.0)
float[] vertices, int[] indices = simConvex.qhull(float[] points, float growth = 0.0)
]]

registerCodeEditorInfos("simConvex", codeEditorInfos)
