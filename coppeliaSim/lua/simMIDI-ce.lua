local codeEditorInfos=[[
string inputPortHandle = simMIDI.openMidiIn(int inputPortIndex)
simMIDI.closeMidiIn(string inputPortHandle)
simMIDI.ignoreTypes(string inputPortHandle, bool sysex, bool timing, bool activeSensing)
int[] messageData = simMIDI.getMessage(string inputPortHandle)
string outputPortHandle = simMIDI.openMidiOut(int outputPortIndex)
simMIDI.closeMidiOut(string outputPortHandle)
simMIDI.sendMessage(string outputPortHandle, int[] messageData)
simMIDI.processIncomingMessages(string midiInPortHandle, int[] channels, map funcs)
]]

registerCodeEditorInfos("simMIDI",codeEditorInfos)
