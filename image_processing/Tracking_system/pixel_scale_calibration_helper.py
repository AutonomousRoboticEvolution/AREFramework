## Instructions:
## 1. print an aruco tags 1 and 2 and cut the paper so that there is a known distance from the corner of the paper to the centre of the tag
## 2. record this known distance as half_size_of_tag below
## 3. set the `tracking_parameters.py` file to crop_rectangle=[-1], pixel_scale=1 and centre_reference = (0,0)
## 4  start the tracking system and put the correct camera_pipe into the variable below
## 5. place the tags onto the floor of the arena so that they are in opposite corners (so the distance from centre of tag to each wall of the arena is half_size_of_tag)
## 6. run this program
## 7. the computed values for centre_reference, pixel_scale and crop_rectangle will be displayed, for you to copy into tracking_parameters.py

camera_pipe = "tcp://127.0.0.1:5557" # address of the tracking system (zeromq)
half_size_of_tag = 100 # distance from centre of tag to corner of the arena, in millimeters

import zmq

#  Prepare our context and sockets
context = zmq.Context()
socket = context.socket(zmq.REQ)
socket.connect(camera_pipe)


socket.send_string("Tags:position")
message = str( socket.recv() )[9:-3]
splt_string = message.split("], [")

for string in splt_string:
    if string.split(",")[0]=="1":
        x1 = float(string.split(",")[1])
        y1 = -float(string.split(",")[2])
    elif string.split(",")[0] == "2":
        x2 = float(string.split(",")[1])
        y2 = -float(string.split(",")[2])

print("Saw tag 1 at {}, {}".format(x1,y1))
print("Saw tag 2 at {}, {}".format(x2,y2))

diff_x = abs(x1-x2)
diff_y = abs(y1-y2)

distance_m = (2000 - 2 * half_size_of_tag)/1000
print("Pixels per meter (x): {}".format(diff_x / distance_m))
print("Pixels per meter (y): {}".format(diff_y / distance_m))
pixel_scale = (diff_y+diff_y) / (2*distance_m) # average of x and y values


border_pixels = round( pixel_scale *( 0.1 + half_size_of_tag/1000) ) # 10 cm extra to include some of the outer wall and help detection work right up to the edge of arena
crop_x = min(x1,x2)-border_pixels
crop_y = min(y1,y2)-border_pixels
size_crop_x = diff_x + 2*border_pixels
size_crop_y = diff_y + 2*border_pixels


print("\n\n")
print("pixel_scale = {}".format(pixel_scale))
print("centre_reference = ({},{})".format(
    int((x1+x2)/2),
    int((y1+y2)/2)
))
print("crop_rectangle = [{},{},{},{}]".format(
    int(crop_x),
    int(crop_y),
    int(size_crop_x),
    int(size_crop_y)
))
