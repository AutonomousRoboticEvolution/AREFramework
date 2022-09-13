import cv2
import zmq
import time
import numpy

SHOW_IMAGES = True
PRINT_INFO = False

#  Prepare our context and sockets
context = zmq.Context()
socket = context.socket(zmq.REQ)
socket.connect("tcp://192.168.2.247:5557")

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
print("Resolution is {} x {} , {}".format(width,height, depth))

start_time = time.time()

while time.time() < start_time + 5:

    if SHOW_IMAGES:
        # get image:
        if PRINT_INFO: print("Requesting image...")
        socket.send_string("Image:image")
        message = socket.recv()
        if PRINT_INFO: print("Got message type: {}".format(type(message)))
        image = numpy.frombuffer(message, dtype=numpy.uint8)
        if PRINT_INFO: print(image[0])
        if PRINT_INFO: print("image is type: {}".format(type(image)))
        if PRINT_INFO: print(width*height*depth)
        image = image.reshape(width,height,depth)
        if PRINT_INFO: print("image shape is {}".format(image.shape))

        cv2.imshow("receiver", image)
        cv2.waitKey(1)

    else:
        socket.send_string("Robot:position")
        message = socket.recv()
        print(message)

    time.sleep(0.1)

filename = str( int( time.time() ) )
print("saving {}".format(filename))
socket.send_string("Recording:save_{}".format(filename))
assert( socket.recv_string() == "OK")
print("finished")
