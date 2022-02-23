#tracking parameters
import cv2
from cv2 import aruco
#blob detection parameters
blob_detection_parameters = cv2.SimpleBlobDetector_Params()

blob_detection_parameters.thresholdStep = 255
blob_detection_parameters.minRepeatability = 1
blob_detection_parameters.blobColor = 255
blob_detection_parameters.filterByConvexity = False
blob_detection_parameters.filterByArea = True
blob_detection_parameters.maxArea = 5000000
blob_detection_parameters.minArea = 100
blob_detection_parameters.filterByInertia = True
blob_detection_parameters.minInertiaRatio = 0.01

# aruco dictionary and parameters
aruco_dict = aruco.Dictionary_get(aruco.DICT_4X4_50)
aruco_parameters = aruco.DetectorParameters_create()

resolution_width = -1 # 1920 # set negative to leave as the default for your camera
resolution_height = -1 # 1080 # set negative to leave as the default for your camera

zmq_port_number= "5557"

with open('location.txt') as f:
    location = f.read().replace("\n","")
    print("location: {}".format(location))

#location specific parameters
if location == "bristol":
    # the pipe for getting images from the camera
    # pipe = "http://192.168.2.248/img/video.mjpeg"
    pipe =0

    #mask filter parameters
    brainMin = (0,157,78)
    brainMax = (26,255,224)

    #centre of uncropped arena and ratio of pixels/metre
    centre_reference = (386,264) # defined in the uncropped image
    pixel_scale = 175

    #crop parameters
    crop_rectangle = [200,75,380,380]

elif location == "york":
    pipe = 0

    brainMin = (171,120,75)
    brainMax = (179,255,255)
    
    centre_reference = (340,300) #not calibrated
    pixel_scale = 100 #nc

    crop_rectangle = [-1]
