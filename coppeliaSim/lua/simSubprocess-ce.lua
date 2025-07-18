local codeEditorInfos=[[
int exitCode, buffer output = simSubprocess.exec(string programPath, string[] args, buffer input="", map opts={})
string handle = simSubprocess.execAsync(string programPath, string[] args, map opts={})
bool running = simSubprocess.isRunning(string handle)
int exitCode = simSubprocess.wait(string handle, float timeout=5)
int exitCode = simSubprocess.kill(string handle)
int pid = simSubprocess.getpid(string handle)
]]

registerCodeEditorInfos("simSubprocess",codeEditorInfos)
