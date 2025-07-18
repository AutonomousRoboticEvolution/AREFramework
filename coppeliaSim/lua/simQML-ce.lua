local codeEditorInfos=[[
string handle = simQML.createEngine()
simQML.destroyEngine(string handle)
simQML.load(string engineHandle, string filename)
simQML.loadData(string engineHandle, string data, string basepath="")
simQML.setEventHandlerRaw(string engineHandle, string functionName)
simQML.sendEventRaw(string engineHandle, string eventName, string eventData)
string dataURL = simQML.imageDataURL(buffer data, int width, int height, string format="BMP", int data_format=simqml_image_data_format_rgb888)
int[] version = simQML.qtVersion()
simQML.sendEvent(string engine, string name, map data)
simQML.setEventHandler(string engine, string funcName)
simQML.image_data_format.gray8
simQML.image_data_format.rgb888
simQML.image_data_format.bgr888
simQML.image_data_format.rgbx8888
simQML.image_data_format.rgb32
simQML.image_data_format.argb32
]]

registerCodeEditorInfos("simQML",codeEditorInfos)
