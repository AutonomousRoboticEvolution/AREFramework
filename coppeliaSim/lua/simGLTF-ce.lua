local codeEditorInfos=[[
simGLTF.clear()
bool result, string warnings, string errors = simGLTF.loadASCII(string filepath)
bool result, string warnings, string errors = simGLTF.loadBinary(string filepath)
bool result = simGLTF.saveASCII(string filepath)
bool result = simGLTF.saveBinary(string filepath)
string json = simGLTF.serialize()
int nodeIndex = simGLTF.exportShape(int shapeHandle, int parentHandle=-1, int parentNodeIndex=0)
int nodeIndex = simGLTF.exportObject(int objectHandle)
simGLTF.exportAllObjects()
simGLTF.exportSelectedObjects()
simGLTF.exportObjects(int[] objectHandles)
simGLTF.exportAnimation()
int count = simGLTF.animationFrameCount()
simGLTF.recordAnimation(bool enable)
simGLTF.setExportTextureFormat(int textureFormat)
int textureFormat, string formatName = simGLTF.getExportTextureFormat()
simGLTF.TextureFormat.bmp
simGLTF.TextureFormat.png
simGLTF.TextureFormat.jpg
]]

registerCodeEditorInfos("simGLTF",codeEditorInfos)
