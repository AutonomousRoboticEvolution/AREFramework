
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

def find_limit(grid):
    min_hsv = [179,255,255]
    max_hsv = [0,0,0]
    for x in range(270,370):
	for y in range(190,290):
            pixel = grid[y,x]
	    for i in range(3):
		max_hsv[i] = max(max_hsv[i],pixel[i])
                min_hsv[i] = min(min_hsv[i],pixel[i])
    return max_hsv,min_hsv

# Capture frames from the camera
for frame in camera.capture_continuous(rawCapture, format="bgr", use_video_port=True):
    image = frame.array
    
    # draw on frame
    image = cv2.rectangle(image,(270,190),(370,290),(0,255,0),3)
    font = cv2.FONT_HERSHEY_SIMPLEX
    hsv = cv2.cvtColor(image,cv2.COLOR_BGR2HSV)
    square = hsv[270:370][190:290]
    max_hsv,min_hsv = find_limit(hsv)
    data="Middle:"+str(hsv[240,320])
    data2="Max:"+str(max_hsv)
    data3="Min:"+str(min_hsv)
    cv2.putText(image,data,(10,150), font,1,(255,255,255),2,cv2.LINE_AA)
    cv2.putText(image,data2,(10,50), font, 1, (255,255,255),2,cv2.LINE_AA)
    cv2.putText(image,data3,(10,100), font, 1,(255,255,255),2,cv2.LINE_AA)
    # Show image
    cv2.imshow("Frame", image)
    hsv = cv2.cvtColor(image,cv2.COLOR_BGR2HSV)
    
    # Clear the stream in preparation for the next frame
    rawCapture.truncate(0)
    

    # if the `q` key was pressed, break from the loop
    if cv2.waitKey(1) & 0xFF == ord("q"):
        break

