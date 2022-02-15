#import needed files
import time
import cv2
from cv2 import aruco
import zmq
import threading
import warnings

#must define location before importing parameters
location = "bristol"
from tracking_parameters import *

class TrackingSystem:

    def __init__(self):
        # set parameters
        self.show_frames = False

        #set up zmq
        context = zmq.Context()
        self.socket = context.socket(zmq.REP)
        self.socket.bind("tcp://*:{}".format(zmq_port_number))

        # the robot and aruco_tags objects that are updated inside the threaded_updater:
        self.robot = None
        self.aruco_tags = []
        self.frame_return_success= False # from openCV this will be `false` if no frames has been grabbed, e.g. failure to connect to camera

        # start the threaded process that gets and interprets images:
        threading.Thread(target=self.threaded_updater).start()  # starting in thread allows update of the camera at a higher frequency than zmq messages are sent

    @staticmethod
    # input pixels should be in the cropped image
    # input should be: [x, y]
    def convert_pixels_to_world_frame(pixels):

        x_reference = centre_reference[0]
        y_reference = centre_reference[1]
        if crop_rectangle[0]>=0:
            # adjust centre reference for cropping by subtracting coordinates of top left corner of crop box
            x_reference -= crop_rectangle[0]
            y_reference -= crop_rectangle[1]

        x =  ( pixels[0] - x_reference) / pixel_scale
        y = -( pixels[1] - y_reference) / pixel_scale

        return [x,y]

    def fillHoles(self,mask):
        contour,hier = cv2.findContours(mask,cv2.RETR_CCOMP,cv2.CHAIN_APPROX_SIMPLE)
        for cnt in contour:
            cv2.drawContours(mask,[cnt],0,255,-1)
        return mask

    # returns a list of two elements representing the robot location as [x,y] in meters
    def robot_location(self,image,hsv_min,hsv_max):
        #converts image to hsv
        hsv = cv2.cvtColor(image,cv2.COLOR_BGR2HSV)
        #create the black/white mask
        mask = cv2.inRange(hsv, hsv_min, hsv_max)
        detector = cv2.SimpleBlobDetector_create(blob_detection_parameters)

        #fills holes in mask to improve blob detection
        fix_mask = True
        if fix_mask:
            fixed_mask = self.fillHoles(mask)
            keypoints = detector.detect(fixed_mask)
        else:
            keypoints = detector.detect(mask)

        #finds biggest keypoint
        if len(keypoints) >0 :
            robotkp = keypoints[0]
            for kp in keypoints:
                if kp.size>robotkp.size:
                    robotkp = kp
            return self.convert_pixels_to_world_frame ( [robotkp.pt[0], robotkp.pt[1]] )
        return None


    # returns tag_list: a list of lists. Each sub-list is 3 elements, the tag number, x and y positions
    # [ [tag_number,x,y] , [tag_number,x,y] , ... ]
    def aruco_locations(self, image):
        #detects aruco aruco_tags and returns list of ids and coords of centre
        gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
        corners,ids,rejectedImgPoints = aruco.detectMarkers(gray, aruco_dict, parameters=aruco_parameters)

        # goes through list of tag ids and finds the centre point
        tag_list = []

        if ids is not None:
            for i in range(len(ids)):
                total_x, total_y = 0, 0
                # find average of corner positions to give the centre position
                for [x, y] in corners[i][0]:
                    total_x += x
                    total_y += y
                x_pos_pixels = int(total_x / 4)
                y_pos_pixels = int(total_y / 4)

                [x_pos, y_pos] = self.convert_pixels_to_world_frame( [ x_pos_pixels, y_pos_pixels])

                tag_list += [[ids[i][0], x_pos, y_pos]]
        # returns list
        return tag_list

    @staticmethod
    def crop_image(input_image):
        if crop_rectangle[0] >= 0:
            # crop_rectangle should be in the format: [x_min, y_min, x_size, y_size]
            return input_image[ crop_rectangle[1]:crop_rectangle[1]+crop_rectangle[3] , crop_rectangle[0]:crop_rectangle[0]+crop_rectangle[2] ]
        else: # if the first element of crop rectangle is set to any negative number, we don't do any cropping
           return  input_image


    def threaded_updater(self):
        print("Tracking started")
        if pipe:
            vid = cv2.VideoCapture(pipe) # pipe is set in tracking_parameters.py

            # main loop for tracking system
            while (1):
                self.frame_return_success, uncropped_image = vid.read()

                if self.frame_return_success:
                    image = self.crop_image(uncropped_image)
                    # gets robot location
                    self.robot = self.robot_location(image, brainMin, brainMax)

                    # gets tag locations and sends them
                    self.aruco_tags = self.aruco_locations(image)
                else:
                    warnings.warn("OpenCV read() failed")
        else:
            import picamera
            import picamera.array
            camera = picamera.PiCamera()
            camera.resolution = (640, 480)
            camera.framerate = 32
            rawCapture = picamera.array.PiRGBArray(camera, size=camera.resolution)
            for frame in camera.capture_continuous(rawCapture, format="bgr", use_video_port=True):
                image = frame.array
                self.frame_return_success, uncropped_image = vid.read()

                if self.frame_return_success:
                    image = self.crop_image(uncropped_image)
                    # gets robot location
                    self.robot = self.robot_location(image, brainMin, brainMax)

                    # gets tag locations and sends them
                    self.aruco_tags = self.aruco_locations(image)
                else:
                    warnings.warn("OpenCV read() failed")

            
    def main(self):

        #main loop waiting for a zmq message
        print("Starting main loop for zmq messages")
        while(1):
            # short sleep
            # time.sleep(1/1000)

            # wait for zmq request
            message = self.socket.recv().decode('utf-8')
            # print("Got message ",message)

            if str(message) == "Robot:position":
                # send the latest value of self.robot
                # print("Sending robot position: {}".format(self.robot))
                if self.robot is not None and self.frame_return_success:
                    self.socket.send_string(f"Robot:{self.robot}")
                else:
                    self.socket.send_string("Robot:None")

            elif str(message) == "Tags:position":
                # send the latest value of self.aruco_tags
                # print("Sending aruco tags position(s): {}".format(self.aruco_tags))
                if self.frame_return_success:
                    self.socket.send_string(f"Tags:{self.aruco_tags}")
                else:
                    self.socket.send_string("Tags:[]") # empty list

            else:
                warnings.warn("Got an unrecognised message: {}".format(message))

            ## for debugging only
            if self.show_frames:
                vid = cv2.VideoCapture(pipe)
                success, image = vid.read()
                if success:
                    cv2.imshow("Image", image )
                    cv2.imshow("Cropped", self.crop_image(image) )
                    cv2.waitKey(1)

        print("Tracking stopped")


tracking_system = TrackingSystem()
tracking_system.main()
