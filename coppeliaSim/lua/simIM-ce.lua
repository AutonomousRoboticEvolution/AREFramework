local codeEditorInfos=[[
string handle = simIM.create(int width, int height, int format=simim_fmt_8UC3, int initialValue=0)
string handle = simIM.createFromData(int width, int height, buffer data, int format=simim_fmt_8UC3)
simIM.destroy(string handle)
string handle = simIM.read(string filename)
simIM.write(string handle, string filename)
string output = simIM.encode(string handle, string format)
string handle = simIM.convert(string handle, int format, float scale=1.0, bool inPlace=false)
int format = simIM.getFormat(string handle)
string handle = simIM.rgb2gray(string handle, bool inPlace=false)
string handle = simIM.gray2rgb(string handle, bool inPlace=false)
string handle = simIM.rgb2hsv(string handle, bool inPlace=false)
string handle = simIM.hsv2rgb(string handle, bool inPlace=false)
string handle = simIM.rgb2hls(string handle, bool inPlace=false)
string handle = simIM.hls2rgb(string handle, bool inPlace=false)
string[] handles = simIM.split(string handle)
string handle = simIM.merge(string[] handles)
simIM.mixChannels(string[] inputHandles, string[] outputHandles, int[] fromTo)
float[] value = simIM.get(string handle, int[2] coord)
simIM.set(string handle, int[2] coord, float[] value)
string handle = simIM.resize(string handle, int width, int height, int interpolation=simim_interp_linear, bool inPlace=false)
int[2] size = simIM.size(string handle)
simIM.copy(string srcHandle, int[2] srcOffset, string dstHandle, int[2] dstOffset, int[2] size)
bool valid, int[2] p1, int[2] p2 = simIM.clipLine(string handle, int[2] p1, int[2] p2)
simIM.line(string handle, int[2] p1, int[2] p2, int[3] color, int thickness=1, int type=8, int shift=0)
simIM.arrowedLine(string handle, int[2] p1, int[2] p2, int[3] color, int thickness=1, int type=8, int shift=0, float tipLength=0.1)
simIM.polylines(string handle, int[] points, int[] numPoints, bool isClosed, int[3] color, int thickness=1, int type=8, int shift=0)
simIM.rectangle(string handle, int[2] p1, int[2] p2, int[3] color, int thickness=1, int type=8, int shift=0)
simIM.circle(string handle, int[2] center, int radius, int[3] color, int thickness=1, int type=8, int shift=0)
simIM.ellipse(string handle, int[2] center, int[2] radius, float angle=0.0, float startAngle=0.0, float endAngle=360.0, int[3] color={255,255,255}, int thickness=1, int type=8, int shift=0)
simIM.fillPoly(string handle, int[] points, int[] numPoints, int[2] offset, int[3] color, int type=8, int shift=0)
simIM.fillConvexPoly(string handle, int[] points, int[3] color, int type=8, int shift=0)
simIM.text(string handle, string str, int[2] pos, int fontFace=simim_fontface_simplex, bool italic=false, float fontScale=1.0, int[3] color={255,255,255}, int thickness=1, int type=8, bool bottomLeftOrigin=false)
int width, int height, int baseline = simIM.textSize(string str, int fontFace=simim_fontface_simplex, bool italic=false, float fontScale=1.0, int thickness=1)
string handle = simIM.abs(string handle, bool inPlace=false)
string handle = simIM.absdiff(string handle1, string handle2, bool inPlace=false)
string handle = simIM.absdiffK(string handle, float[] k, bool inPlace=false)
string handle = simIM.add(string handle1, string handle2, bool inPlace=false)
string handle = simIM.addK(string handle, float[] k, bool inPlace=false)
string handle = simIM.subtract(string handle1, string handle2, bool inPlace=false)
string handle = simIM.subtractK(string handle, float[] k, bool inPlace=false)
string handle = simIM.multiply(string handle1, string handle2, bool inPlace=false)
string handle = simIM.divide(string handle1, string handle2, bool inPlace=false)
string handle = simIM.divideK(float[] k, string handle, bool inPlace=false)
string handle = simIM.compare(string handle1, string handle2, int op, bool inPlace=false)
string handle = simIM.compareK(string handle, float[] k, int op, bool inPlace=false)
string handle = simIM.reduce(string handle, int dim, int op, bool inPlace=false)
string handle = simIM.repeat(string handle, int nx, int ny, bool inPlace=false)
string handle = simIM.flip(string handle, int op=0, bool inPlace=false)
string handle = simIM.log(string handle, bool inPlace=false)
string handle = simIM.exp(string handle, bool inPlace=false)
string handle = simIM.pow(string handle, float power, bool inPlace=false)
string handle = simIM.sqrt(string handle, bool inPlace=false)
string handle = simIM.addWeighted(string handle1, string handle2, float alpha, float beta, float gamma, bool inPlace=false)
string handle = simIM.scaleAdd(string handle1, string handle2, float alpha, bool inPlace=false)
string handle = simIM.magnitude(string handle1, string handle2)
string handle = simIM.phase(string handle1, string handle2, bool angleInDegrees=false)
string handle1, string handle2 = simIM.polar2cart(string handle1, string handle2, bool angleInDegrees=false)
string handle1, string handle2 = simIM.cart2polar(string handle1, string handle2, bool angleInDegrees=false)
string handle = simIM.bitwiseAnd(string handle1, string handle2, bool inPlace=false)
string handle = simIM.bitwiseAndK(string handle, float[] k, bool inPlace=false)
string handle = simIM.bitwiseOr(string handle1, string handle2, bool inPlace=false)
string handle = simIM.bitwiseOrK(string handle, float[] k, bool inPlace=false)
string handle = simIM.bitwiseXor(string handle1, string handle2, bool inPlace=false)
string handle = simIM.bitwiseXorK(string handle, float[] k, bool inPlace=false)
string handle = simIM.bitwiseNot(string handle, bool inPlace=false)
string handle = simIM.distanceTransform(string handle, int distanceType=simim_dist_L2, int maskSize=simim_masksize_precise, bool inPlace=false)
simIM.writeToVisionSensor(string handle, int sensorHandle)
string handle = simIM.readFromVisionSensor(int sensorHandle, string handle="")
simIM.openVideoCapture(int deviceIndex)
simIM.closeVideoCapture(int deviceIndex)
string handle = simIM.readFromVideoCapture(int deviceIndex, string handle="")
simIM.writeToTexture(string handle, int textureId)
string handle = simIM.getMarkerDictionary(int type)
string handle = simIM.drawMarker(string dictionaryHandle, int markerId, int size, string handle="", int borderSize=1)
float[] corners, int[] markerIds, float[] rejectedCandidates = simIM.detectMarkers(string handle, string dictionaryHandle)
string output = simIM.dataURL(string imgHandle, string format=BMP)
int size = simIM.getMarkerBitSize(int dictType)
simIM.format._8UC1
simIM.format._8UC3
simIM.format._8UC4
simIM.format._32FC1
simIM.format._32FC3
simIM.format._32FC4
simIM.interp.nearest
simIM.interp.linear
simIM.interp.area
simIM.interp.cubic
simIM.interp.lanczos4
simIM.fontFace.simplex
simIM.fontFace.plain
simIM.fontFace.duplex
simIM.fontFace.complex
simIM.fontFace.triplex
simIM.fontFace.complex_small
simIM.fontFace.script_simplex
simIM.fontFace.script_complex
simIM.cmpOp.eq
simIM.cmpOp.gt
simIM.cmpOp.ge
simIM.cmpOp.lt
simIM.cmpOp.le
simIM.cmpOp.ne
simIM.reduceOp.sum
simIM.reduceOp.avg
simIM.reduceOp.max
simIM.reduceOp.min
simIM.flipOp.x
simIM.flipOp.y
simIM.flipOp.both
simIM.dist.L1
simIM.dist.L2
simIM.dist.C
simIM.maskSize._3x3
simIM.maskSize._5x5
simIM.maskSize._precise
simIM.dict_type._4X4_50
simIM.dict_type._4X4_100
simIM.dict_type._4X4_250
simIM.dict_type._4X4_1000
simIM.dict_type._5X5_50
simIM.dict_type._5X5_100
simIM.dict_type._5X5_250
simIM.dict_type._5X5_1000
simIM.dict_type._6X6_50
simIM.dict_type._6X6_100
simIM.dict_type._6X6_250
simIM.dict_type._6X6_1000
simIM.dict_type._7X7_50
simIM.dict_type._7X7_100
simIM.dict_type._7X7_250
simIM.dict_type._7X7_1000
simIM.dict_type._ARUCO_ORIGINAL
simIM.dict_type._APRILTAG_16h5
simIM.dict_type._APRILTAG_25h9
simIM.dict_type._APRILTAG_36h10
simIM.dict_type._APRILTAG_36h11
]]

registerCodeEditorInfos("simIM",codeEditorInfos)
