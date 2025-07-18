local codeEditorInfos=[[
string probeHandle = simEvents.addProbe(string callback, any condition)
simEvents.removeProbe(string probeHandle)
string probeHandle = simEvents.addChildrenMonitor(string callback, int parentHandle)
]]

registerCodeEditorInfos("simEvents",codeEditorInfos)
