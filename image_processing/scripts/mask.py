# Machine vision lab - Practical Robotics
# test_camera.py: Grab the camera framebuffer in OpenCV and display using cv2.imshow


# Import the necessary packages
import picamera
import picamera.array
import time
import cv2

# Initialise the camera and create a reference to it
camera = picamera.PiCamera()
camera.resolution = (640, 480)
camera.framerate = 32
rawCapture = picamera.array.PiRGBArray(camera, size=camera.resolution)

# Allow the camera time to warm up
time.sleep(0.1)

# Capture frames from the camera
for frame in camera.capture_continuous(rawCapture, format="bgr", use_video_port=True):
    image = frame.array
    
    # Show the frame
    cv2.imshow("Frame", image)
    hsv = cv2.cvtColor(image,cv2.COLOR_BGR2HSV)
    blueMin = (100, 100, 100)
    blue = (120, 225, 127)
    blueMax = (140, 255, 255)
    mask = cv2.inRange(hsv, blueMin, blueMax)
    # Clear the stream in preparation for the next frame
    rawCapture.truncate(0)
    masked_image = cv2.bitwise_and(image, image, mask=mask)

    cv2.imshow("Mask", masked_image)

    # if the `q` key was pressed, break from the loop
    if cv2.waitKey(1) & 0xFF == ord("q"):
        break
