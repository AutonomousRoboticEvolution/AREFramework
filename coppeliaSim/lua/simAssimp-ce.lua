local codeEditorInfos=[[
int[] shapeHandles = simAssimp.importShapes(string filenames, int maxTextureSize=512, float scaling=0.0, int upVector=simassimp_upvect_auto, int options=0)
simAssimp.exportShapes(int[] shapeHandles, string filename, string formatId, float scaling=1.0, int upVector=simassimp_upvect_z, int options=0)
string formatDescription, string formatExtension = simAssimp.getImportFormat(int index)
string formatDescription, string formatExtension, string formatId = simAssimp.getExportFormat(int index)
map allVertices, map allIndices = simAssimp.importMeshes(string filenames, float scaling=0.0, int upVector=simassimp_upvect_auto, int options=0)
simAssimp.exportMeshes(map allVertices, map allIndices, string filename, string formatId, float scaling=1.0, int upVector=simassimp_upvect_z, int options=0)
int[] handles = simAssimp.importShapesDlg(string filename)
simAssimp.exportShapesDlg(string filename, int[] shapeHandles)
simAssimp.upVector.auto
simAssimp.upVector.z
simAssimp.upVector.y
]]

registerCodeEditorInfos("simAssimp",codeEditorInfos)
