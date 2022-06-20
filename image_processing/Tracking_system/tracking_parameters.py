#tracking parameters
import cv2
from cv2 import aruco
import os

show_frames = False
show_fps = True
verbose_messages = False

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
videos_folder_path = os.path.expanduser('~')+"/tracking_videos"
if not os.path.exists(videos_folder_path): os.makedirs(videos_folder_path) # make sure folders exist to prevent errors later

recording_frame_rate = 15.0

with open('location.txt') as f:
    location = f.read().replace("\n","")
    print("location: {}".format(location))

#location specific parameters
if location == "bristol":
    # the pipe for getting images from the camera
    pipe = "http://192.168.2.248/img/video.mjpeg"

    #mask filter parameters
    brainMin = (0,157,78)
    brainMax = (26,255,224)

    #centre of uncropped arena and ratio of pixels/metre
    centre_reference = (386,264) # defined in the uncropped image
    pixel_scale = 175


    #crop parameters
    crop_rectangle = [200,75,380,380]

if location == "bristol_pi":
    # the pipe for getting images from the camera
    pipe =0

    #mask filter parameters
    brainMin = (0,157,78)
    brainMax = (26,255,224)

    # overwrite default resolution:
    resolution_width = 1920 ; resolution_height = 1056

    #centre of uncropped arena and ratio of pixels/metre
    centre_reference = (int(resolution_width/2),int(resolution_height/2)) # defined in the uncropped image
    pixel_scale = 175

    #crop parameters
    width = 640; height = 480
    #crop_rectangle = [int( (resolution_width-width)/2),int((resolution_height-height)/2),width,height]
    crop_rectangle = [-1]

elif location == "york":
    pipe = 0
    brainMin = (0,120,52)
    brainMax = (16,206,139)
    
    centre_reference = (347,240) # centre of arena in pixels
    pixel_scale = 270 # mm per meter

    crop_rectangle = [-1]
elif location == "napier":
    pipe = "tcpclientsrc host=192.168.0.15 port=50000 ! gdpdepay ! rtph264depay ! avdec_h264 ! videoconvert ! tee ! appsink"

    brainMin = (169,100,165)
    brainMax = (179,255,255)

    centre_reference = (600,545)
    pixel_scale = 500 #nc

    crop_rectangle = [-1]
elif location == "amsterdam":
    pipe = "rtsp://admin:Robocam_0@10.15.1.198:554/cam/realmonitor?channel=1&subtype=0"
    #mask filter parameters
    brainMin = (0,157,154)
    brainMax = (10,221,208)

    #centre of uncropped arena and ratio of pixels/metre
    centre_reference = (386,264) # defined in the uncropped image
    pixel_scale = 175
    
    crop_rectangle = [-1]


elif location == "amsterdam":
    pipe = "rtsp://admin:Robocam_0@10.15.1.198:554/cam/realmonitor?channel=1&subtype=0"
    #mask filter parameters
    brainMin = (0,47,87)
    brainMax = (11,255,255)

    #centre of uncropped arena and ratio of pixels/metre
    centre_reference = (970,430) # defined in the uncropped image
    pixel_scale = 540

    crop_rectangle = [-1]
    
    
