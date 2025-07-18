local codeEditorInfos = [[
bool result = simMTB.connectInput(int inputMtbServerHandle, int inputBitNumber, int outputMtbServerHandle, int outputBitNumber, int connectionType)
bool result = simMTB.disconnectInput(int inputMtbServerHandle, int inputBitNumber)
int[4] inputValues = simMTB.getInput(int mtbServerHandle)
float[4] jointValues = simMTB.getJoints(int mtbServerHandle)
int[4] outputValues = simMTB.getOutput(int mtbServerHandle)
bool result = simMTB.setInput(int mtbServerHandle, int[4] inputValues)
int mtbServerHandle, string message = simMTB.startServer(string mtbServerExecutable, int portNumber, buffer program, float[4] jointPositions, float[2] velocities)
int result, string message = simMTB.step(int mtbServerHandle, float timeStep)
bool result = simMTB.stopServer(int mtbServerHandle)
]]

registerCodeEditorInfos("simMTB", codeEditorInfos)
