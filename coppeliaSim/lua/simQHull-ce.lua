local codeEditorInfos = [[
double[] vertices, int[] indices = simQHull.compute(double[] vertices, bool generateIndices)
int resultShapeHandle = simQHull.computeShape(int[] handles)
]]

registerCodeEditorInfos("simQHull", codeEditorInfos)
