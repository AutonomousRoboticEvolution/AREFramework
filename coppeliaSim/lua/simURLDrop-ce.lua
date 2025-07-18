local codeEditorInfos=[[
simURLDrop.openURL(string url)
string dataOrFilename = simURLDrop.getURL(string url, int mode=simurldrop_download_mode_buffer)
simURLDrop.download_mode.file
simURLDrop.download_mode.buffer
]]

registerCodeEditorInfos("simURLDrop",codeEditorInfos)
