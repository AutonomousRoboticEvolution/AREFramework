#tracking parameters
import cv2
from cv2 import aruco
import os

show_fps = False
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

resolution_width = -1 # set negative to leave as the default for your camera
resolution_height = -1 # set negative to leave as the default for your camera

zmq_port_number= "5557"
videos_folder_path = os.path.expanduser('~')+"/tracking_videos"
if not os.path.exists(videos_folder_path): os.makedirs(videos_folder_path) # make sure folders exist to prevent errors later

recording_frame_rate = 15.0
recording_timeout_seconds = 60*11

with open('location.txt') as f:
    location = f.read().strip()

# default parameters, if not set by the location
show_frames = False
distortion_correction_file = None

#location specific parameters
if location == "bristol":
    # the pipe for getting images from the camera
    pipe =0

    show_frames = False

    #mask filter parameters
    brainMin = ((0,100,0), (0,0,0,))
    brainMax = ((14,255,255), (0,0,0))

    # overwrite default resolution:
    resolution_width = 1920 ; resolution_height = 1056

    pixel_scale = 380.55555555555554
    centre_reference = (943, 574)
    crop_rectangle = [525, 160, 830, 830]

elif location == "york":
    pipe = 0
    brainMin = ((0,130,52),)
    brainMax = ((179,194,86),)

    show_frames = True
    
    centre_reference = (347,240) # centre of arena in pixels
    pixel_scale = 270 # mm per meter

    crop_rectangle = [-1]

elif location == "york2":
    pipe = 0

    show_frames = True
    
    headMin = ((0,69,0),)
    headMax = ((22,255,255),)

    skeletonMin = ((34,22,0),)
    skeletonMax = ((63,255,255),)

    organMin = ((76,42,0),)
    organMax = ((123,255,255),)
    
    brainMin = [headMin,skeletonMin,organMin]
    brainMax = [headMax,skeletonMax,organMax]

elif location == "napier":
    pipe = "tcpclientsrc host=192.168.0.15 port=50000 ! gdpdepay ! rtph264depay ! avdec_h264 ! videoconvert ! tee ! appsink"

    show_frames = True

    brainMin = ((169,100,165),)
    brainMax = ((179,255,255),)

    centre_reference = (600,545)
    pixel_scale = 500 #nc

    crop_rectangle = [-1]

elif location == "amsterdam":
    pipe = "rtsp://admin:Robocam_0@10.15.1.198:554/cam/realmonitor?channel=1&subtype=0"
    distortion_correction_file = "./calibration/amsterdam_cam_198_barrel_distortion.npz"
    
    
    show_frames = True
    
    # mask filter parameters
    brainMin = ((0,47,87),)
    brainMax = ((11,255,255),)

    # centre of uncropped arena and ratio of pixels/metre
    centre_reference = (982,530)
    pixel_scale = 376.34408602150535
    crop_rectangle = [560,116,845,828]
    
    
print(f"location: {location}")
print(f"pipe: {pipe}")
if show_frames:
    print("Show frames is on")
else:
    print("Show frames is off")
