local codeEditorInfos=[[
string serverHandle = simWS.start(int listenPort)
simWS.setOpenHandler(string serverOrClientHandle, string callbackFn)
simWS.setFailHandler(string serverOrClientHandle, string callbackFn)
simWS.setCloseHandler(string serverOrClientHandle, string callbackFn)
simWS.setMessageHandler(string serverOrClientHandle, string callbackFn)
simWS.setHTTPHandler(string serverHandle, string callbackFn)
simWS.send(string serverOrClientHandle, string connectionHandle, buffer data, int opcode=simws_opcode_text)
simWS.stop(string serverHandle)
string clientHandle = simWS.connect(string uri)
simWS.disconnect(string clientHandle)
simWS.opcode.continuation
simWS.opcode.text
simWS.opcode.binary
]]

registerCodeEditorInfos("simWS",codeEditorInfos)
