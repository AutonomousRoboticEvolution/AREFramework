#tracking parameters
import cv2
from cv2 import aruco
import os

show_fps = False
verbose_messages = True

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

resolution_width = 1920 # set negative to leave as the default for your camera
resolution_height = 1080 # set negative to leave as the default for your camera

zmq_port_number= "5557"
videos_folder_path = os.path.expanduser('~')+"/tracking_videos"
if not os.path.exists(videos_folder_path): os.makedirs(videos_folder_path) # make sure folders exist to prevent errors later

recording_frame_rate = 15.0
recording_timeout_seconds = 60*11

with open('location.txt') as f:
    location = f.read().replace("\n","")

#location specific parameters
if location == "bristol":
    # the pipe for getting images from the camera
    # pipe = "http://192.168.2.248/img/video.mjpeg"
    pipe = "/dev/v4l/by-id/usb-046d_Logitech_Webcam_C930e_86CF445E-video-index0" # usb webcam

    show_frames = True

    #mask filter parameters
    # brainMin: (HSV1, HSV2, ..... HSVn)
    # brainMin: (HSV1, HSV2, ..... HSVn)
    # for each HSVn_min and HSVn_max is getting all pixel in the range between min and max
    # then joins all `n` HSV blobs into the same image
    brainMin = ((0,20,10) , (172,20,10)) #  blue: (13,38,18) green:(31,108,82)
    brainMax = ((8,255,255), (255,255,255)) # blue:(33,169,83) green:(53,203,158)

    #centre of uncropped arena and ratio of pixels/metre
    pixel_scale = 287.77777777777777
    centre_reference = (1052, 395)
    crop_rectangle = [390, 100, 1125, 834]

if location == "bristol_pi":
    # the pipe for getting images from the camera
    pipe =0

    show_frames = False

    #mask filter parameters
    brainMin = ((0,157,78), (0,0,0))
    brainMax = ((26,255,224), (0,0,0))

    # overwrite default resolution:
    resolution_width = 1920 ; resolution_height = 1056

    #centre of uncropped arena and ratio of pixels/metre
    centre_reference = (int(resolution_width/2),int(resolution_height/2)) # defined in the uncropped image
    pixel_scale = 175

    #crop parameters
    width = 640; height = 480
    crop_rectangle = [int( (resolution_width-width)/2),int((resolution_height-height)/2),width,height]
    #crop_rectangle = [-1]

elif location == "york":
    pipe = 0
    brainMin = (0,130,52)
    brainMax = (179,194,86)

    show_frames = True
    
    centre_reference = (347,240) # centre of arena in pixels
    pixel_scale = 270 # mm per meter

    crop_rectangle = [-1]

elif location == "york2":
    pipe = 0

    show_frames = True
    
    headMin = (0,69,0)
    headMax = (22,255,255)

    skeletonMin = (34,22,0)
    skeletonMax = (63,255,255)

    organMin = (76,42,0)
    organMax = (123,255,255)
    
    brainMin = [headMin,skeletonMin,organMin]
    brainMax = [headMax,skeletonMax,organMax]

elif location == "napier":
    pipe = "tcpclientsrc host=192.168.0.15 port=50000 ! gdpdepay ! rtph264depay ! avdec_h264 ! videoconvert ! tee ! appsink"

    show_frames = True

    brainMin = (169,100,165)
    brainMax = (179,255,255)

    centre_reference = (600,545)
    pixel_scale = 500 #nc

    crop_rectangle = [-1]
elif location == "amsterdam":
    pipe = "rtsp://admin:Robocam_0@10.15.1.198:554/cam/realmonitor?channel=1&subtype=0"

    show_frames = True

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
    
    
print("location: {}".format(location))
print("pipe: {}".format(pipe))
if show_frames:
    print ( "Show frames is on" )
else:
    print ( "Show frames is off" )