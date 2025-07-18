local codeEditorInfos=[[
float[] m = simICP.match(int model_handle, int template_handle, float outlier_treshold=-1)
float[] m = simICP.matchToShape(int model_handle, int template_handle, float outlier_treshold=-1)
]]

registerCodeEditorInfos("simICP",codeEditorInfos)
