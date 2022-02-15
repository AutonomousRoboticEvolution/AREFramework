#tracking parameters
import cv2
from cv2 import aruco
#blob detection parameters
params = cv2.SimpleBlobDetector_Params()

params.thresholdStep = 255
params.minRepeatability = 1
params.blobColor = 255
params.filterByConvexity = False
params.filterByArea = True
params.maxArea = 5000000
params.minArea = 100
params.filterByInertia = True
params.minInertiaRatio = 0.01

# aruco dictionary and parameters
aruco_dict = aruco.Dictionary_get(aruco.DICT_4X4_50)
aruco_parameters = aruco.DetectorParameters_create()

location = "bristol"

#location specific parameters
if location == "bristol":
    global brainMin,brainMax
    pipe = "http://192.168.2.248/img/video.mjpeg"
    #mask filter parameters
    brainMin = (0,157,78)
    brainMax = (26,255,224)

    #centre of uncropped arena and ratio of pixels/metre
    centre_reference = (390,265)
    pixel_scale = 175

    #crop parameters
    crop_rectangle = [205,75,380,380]
