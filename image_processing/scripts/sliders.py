#
# Machine vision lab - Practical Robotics
# test_camera.py: Grab the camera framebuffer in OpenCV and display using cv2.imshow


# Import the necessary packages

import time
import cv2

# Initialise the camera and create a reference to it
is_picamera = False
if is_picamera:
    import picamera
    import picamera.array
    camera = picamera.PiCamera()
    camera.resolution = (640, 480)
    camera.framerate = 32
    rawCapture = picamera.array.PiRGBArray(camera, size=camera.resolution)
else:
    #rawCapture = cv2.VideoCapture("v4l2src device=/dev/video2 ! videoconvert ! appsink")
    rawCapture = cv2.VideoCapture('tcpclientsrc host=192.168.2.248 port=50000 ! gdpdepay  ! rtph264depay ! avdec_h264 ! videoconvert ! tee ! appsink')

# Allow the camera time to warm up
time.sleep(0.1)

def nothing(x):
    pass

def slider_window():

    cv2.namedWindow('sliders')
    cv2.createTrackbar('Hmin','sliders',0,179,nothing)
    cv2.createTrackbar('Smin','sliders',0,255,nothing)
    cv2.createTrackbar('Vmin','sliders',0,255,nothing)
    cv2.createTrackbar('Hmax','sliders',0,179,nothing)
    cv2.createTrackbar('Smax','sliders',0,255,nothing)
    cv2.createTrackbar('Vmax','sliders',0,255,nothing)

slider_window()

def sliders_do(image):
    # draw on frame
    font = cv2.FONT_HERSHEY_SIMPLEX
    hsv = cv2.cvtColor(image,cv2.COLOR_BGR2HSV)
    
    # Show image
    # setup sliders
    
    cv2.imshow("sliders", image)
    # get current positions of three
    Hmin = cv2.getTrackbarPos('Hmin','sliders')
    Smin = cv2.getTrackbarPos('Smin','sliders')
    Vmin = cv2.getTrackbarPos('Vmin','sliders')
    Hmax = cv2.getTrackbarPos('Hmax','sliders')
    Smax = cv2.getTrackbarPos('Smax','sliders')
    Vmax = cv2.getTrackbarPos('Vmax','sliders')
    print(Hmax,Smax,Vmax)
    
    #cv2.imshow("Frame", image)
    hsv = cv2.cvtColor(image,cv2.COLOR_BGR2HSV)
    
    blueMin = (100, 100, 50)
    blue = (120, 225, 127)
    blueMax = (140, 255, 255)

    colorMax = (Hmax,Smax,Vmax)
    colorMin = (Hmin,Smin,Vmin)
    
    mask = cv2.inRange(hsv, colorMin, colorMax)

    masked_image = cv2.bitwise_and(image, image, mask=mask)

    cv2.imshow("Mask", masked_image)
    # Clear the stream in preparation for the next frame
    #rawCapture.truncate(0)
    

    # if the `q` key was pressed, break from the loop
    if cv2.waitKey(1) & 0xFF == ord("q"):
        return

# Capture frames from the camera
if is_picamera:
    for frame in camera.capture_continuous(rawCapture, format="bgr", use_video_port=True):
        image = frame.array
        sliders_do(image)
else:
    while(1):
        ret,image = rawCapture.read()
        sliders_do(image)


    
    

