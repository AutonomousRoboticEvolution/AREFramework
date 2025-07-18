local codeEditorInfos=[[
int[] version = simPython.getVersion()
string scriptStateHandle = simPython.create()
simPython.destroy(string scriptStateHandle)
map result = simPython.run(string scriptStateHandle, string code)
map result = simPython.call(string scriptStateHandle, string func, map args)
]]

registerCodeEditorInfos("simPython",codeEditorInfos)
