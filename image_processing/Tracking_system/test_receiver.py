import cv2
import zmq
import time
import numpy

SHOW_FULL_RESOLUTION_IMAGE_AT_START = True
SHOW_IMAGES = True
PRINT_INFO = False
RECORD_VIDEO = False
video_length_seconds=5
LOOP_FOREVER = True

if LOOP_FOREVER and RECORD_VIDEO:
    raise RuntimeError("RECORD VIDEO and LOOP FOREVER are not compatible")

# aruco dictionary and parameters
from cv2 import aruco
aruco_dict = aruco.Dictionary_get(aruco.DICT_4X4_50)
aruco_parameters = aruco.DetectorParameters_create()

#  Prepare our context and sockets
context = zmq.Context()
socket = context.socket(zmq.REQ)
#socket.connect("tcp://192.168.2.100:5557")
socket.connect("tcp://192.168.2.247:5557")

if RECORD_VIDEO:
    socket.send_string("Recording:start")
    assert( socket.recv_string() == "OK")
    time.sleep(0.1)


# get resolution:
print("getting resolution")
socket.send_string("Image:resolution")
message = socket.recv_string()
message = message.replace("Image:","")
message = message.replace("(","")
message = message.replace(")","").split(",")
width = int(message[0])
height = int(message[1])
depth = int(message[2])
print(f"Resolution is {width} x {height} , {depth}")

if SHOW_FULL_RESOLUTION_IMAGE_AT_START:
    if PRINT_INFO: print ( "Requesting full image..." )
    socket.send_string ( "Image:full_image" )
    message = socket.recv ()
    full_image = numpy.frombuffer ( message, dtype=numpy.uint8 ) .reshape(264,480,3)
    cv2.imshow ( "full image", full_image )
    cv2.waitKey(1)


start_time = time.time()

while time.time() < start_time + video_length_seconds or LOOP_FOREVER:

    if SHOW_IMAGES:
        # get image:
        if PRINT_INFO: print("Requesting image...")
        socket.send_string("Image:image")
        message = socket.recv()
        if PRINT_INFO: print(f"Got message type: {type(message)}")
        image = numpy.frombuffer(message, dtype=numpy.uint8)
        if PRINT_INFO: print(image[0])
        if PRINT_INFO: print(f"image is type: {type(image)}")
        if PRINT_INFO: print(width*height*depth)
        image = image.reshape(width,height,depth)
        if PRINT_INFO: print(f"image shape is {image.shape}")

        (corners, ids, rejected) = aruco.detectMarkers(image, aruco_dict, parameters=aruco_parameters)
        aruco.drawDetectedMarkers(image,corners,ids)
        cv2.imshow("cropped section", image)

        # socket.send_string ( "Image:full_image" )
        # message = socket.recv ()
        # full_image = numpy.frombuffer ( message, dtype=numpy.uint8 ).reshape ( 264, 480, 3 )
        # cv2.imshow ( "full image", full_image )

        cv2.waitKey(1)

    else:
        socket.send_string("Robot:position")
        message = socket.recv()
        print(message)

    #time.sleep(0.1)

if RECORD_VIDEO:
    filename = str( int( time.time() ) )
    print(f"saving {filename}")
    socket.send_string(f"Recording:save_{filename}")
    assert( socket.recv_string() == "OK")
print("finished")
cv2.waitKey(0)
