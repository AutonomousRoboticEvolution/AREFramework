#import needed files
import time
import cv2
from cv2 import aruco
import zmq

#must define location before importing parameters
location = "bristol"
from tracking_parameters import *

show_frames = True

#set up zmq
context = zmq.Context()
socket = context.socket(zmq.PUB)
socket.bind("tcp://*:5556")

def fillHoles(mask):
    contour,hier = cv2.findContours(mask,cv2.RETR_CCOMP,cv2.CHAIN_APPROX_SIMPLE)
    for cnt in contour:
        cv2.drawContours(mask,[cnt],0,255,-1)
    return mask

def robot_location(image,hsv_min,hsv_max):
    #converts image to hsv
    hsv = cv2.cvtColor(image,cv2.COLOR_BGR2HSV)
    #create the black/white mask
    mask = cv2.inRange(hsv, hsv_min, hsv_max)
    detector = cv2.SimpleBlobDetector_create(params)
    
    #fills holes in mask to improve blob detection
    fix_mask = True
    if fix_mask:
        fixed_mask = fillHoles(mask)
        keypoints = detector.detect(fixed_mask)

    if show_frames:
        #adds circles to mask to show blobs
        kp_image = cv2.drawKeypoints(mask,keypoints,None,color=(0,0,255),flags= cv2.DRAW_MATCHES_FLAGS_DRAW_RICH_KEYPOINTS)

        cv2.imshow("Blob image",image)
        cv2.imshow("Holy Mask",mask)
        cv2.imshow("Fixed Mask",fixed_mask)
        cv2.imshow("Blob",kp_image)

    #finds biggest keypoint
    if keypoints != []:
        robotkp = keypoints[0]
        for kp in keypoints:
            if kp.size>robotkp.size:
                robotkp = kp
        return robotkp.pt
    return []

def aruco_proccessing(ids,corners):
    #goes through list of tag ids and finds the centre point
    tag_list =[]

    if ids is not None:
        for i in range(len(ids)):
            total_x,total_y = 0,0
            for [x,y] in corners[i][0]:
                total_x += x
                total_y += y
            x_pos = int(total_x/4)
            y_pos = int(total_y/4)

            tag_list += [[ids[i][0],x_pos,y_pos]]
    # returns list
    return tag_list

def aruco_locations(image):
    #detects aruco tags and returns list of ids and coords of centre
    gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
    corners,ids,rejectedImgPoints = aruco.detectMarkers(gray, aruco_dict, parameters=aruco_parameters)

    # Show the frame
    if show_frames:
        frame_markers = aruco.drawDetectedMarkers(image, corners, ids)
        cv2.imshow("Frame", frame_markers)
            
    return aruco_proccessing(ids,corners)

def tracking_system(pipe):
    print("Tracking started")
    vid = cv2.VideoCapture(pipe)

    #main loop for tracking systm
    for i in range(1000):
        ret, image = vid.read()
        
        #image = frame.array
        if ret:
            #cv2.imshow("raw", image)
            #crop image
            #image = image[crop_rectangle[0]:crop_rectangle[2], crop_rectangle[1]:crop_rectangle[3]]
            
            #gets robot location and sends it
            robot=robot_location(image,brainMin,brainMax)
            if robot:
                socket.send_string(f"Robot:{robot}")
            else:
                socket.send_string("Robot: No robot")

            #gets tag locations and sends them
            tags=aruco_locations(image)

            if tags:
                socket.send_string(f"Tags:{tags}")
            else:
                socket.send_string("Tags: No tags")

            #prints data if needed, if empty then empty list printed
            if show_frames:
                print("Robot @:",robot)
                print("Tags @:",tags)
            

        if cv2.waitKey(1) & 0xFF == ord("q"):
            break

    print("Tracking stoped")


tracking_system(pipe)
