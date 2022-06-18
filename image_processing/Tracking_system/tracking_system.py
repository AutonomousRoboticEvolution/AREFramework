#import needed files
import time
import cv2
from cv2 import aruco
import zmq
import threading
import warnings

#must define location before importing parameters

from tracking_parameters import *

class TrackingSystem:

    def __init__(self):
        # set parameters
        self.show_frames = True

        # recording variables
        self.is_recording = False
        self.i_frame = 0

        #set up zmq
        context = zmq.Context()
        self.socket = context.socket(zmq.REP)
        self.socket.bind("tcp://*:{}".format(zmq_port_number))

        # the robot and aruco_tags objects that are updated inside the threaded_updater:
        self.cropped_image = None
        self.robot = None
        self.aruco_tags = []
        self.frame_return_success= False # from openCV this will be `false` if no frames has been grabbed, e.g. failure to connect to camera


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
        fix_mask = False
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

        #displays tags
 #       if self.show_frames:
 #           frame_markers = aruco.drawDetectedMarkers(image, corners, ids)
 #           cv2.imshow("Frame", frame_markers)

        # returns list
        return tag_list

    @staticmethod
    def crop_image(input_image):
        if crop_rectangle[0] >= 0:
            # crop_rectangle should be in the format: [x_min, y_min, x_size, y_size]
            return input_image[ crop_rectangle[1]:crop_rectangle[1]+crop_rectangle[3] , crop_rectangle[0]:crop_rectangle[0]+crop_rectangle[2] ]
        else: # if the first element of crop rectangle is set to any negative number, we don't do any cropping
           return  input_image


    def camera_updater(self):
        vid = cv2.VideoCapture(pipe) # pipe is set in tracking_parameters.py

        # set resolution:
        if resolution_width>0: vid.set(cv2.CAP_PROP_FRAME_WIDTH, resolution_width)
        if resolution_height>0: vid.set(cv2.CAP_PROP_FRAME_HEIGHT, resolution_height)

        print("Tracking loop started")
        # loop for tracking system
        while (1):
            self.frame_return_success, temp_image = vid.read()

            if self.frame_return_success:
                self.cropped_image = self.crop_image(temp_image)
                # gets robot location
                self.robot = self.robot_location(self.cropped_image, brainMin, brainMax)

                # gets tag locations and sends them
                self.aruco_tags = self.aruco_locations(self.cropped_image)

                if self.is_recording:
                    cv2.imwrite("{}/image%04d.png".format(temporary_image_folder_path) % self.i_frame , self.cropped_image)
                    self.i_frame += 1
                    if self.i_frame>=max_frame: self.is_recording=False

                if self.show_frames:
                    cv2.imshow("Cropped", self.cropped_image )
                    cv2.imshow("Mask", cv2.inRange(cv2.cvtColor(self.cropped_image,cv2.COLOR_BGR2HSV), brainMin, brainMax) )
                cv2.waitKey(1)
            else:
                warnings.warn("OpenCV read() failed")

    def show_im(self):
        vid = cv2.VideoCapture(pipe)
        while (1):
            self.frame_return_success, image = vid.read()

            if self.frame_return_success:
                cv2.imshow("Uncropped", image )
                cv2.waitKey(1)
    def zmq_updater(self):
        #main loop waiting for a zmq message
        print("Starting loop for zmq messages")
        while(1):
            # short sleep
            # time.sleep(1/1000)

            # wait for zmq request
            message = self.socket.recv().decode('utf-8')
            print("Got message ",message)

            if str(message) == "Robot:position":
                # send the latest value of self.robot
                if self.show_frames:
                    print("Sending robot position: {}".format(self.robot))
                if self.robot is not None and self.frame_return_success:
                    self.socket.send_string(f"Robot:{self.robot}")
                else:
                    self.socket.send_string("Robot:None")

            elif str(message) == "Tags:position":
                # send the latest value of self.aruco_tags
                if self.show_frames:
                    print("Sending aruco tags position(s): {}".format(self.aruco_tags))
                if self.frame_return_success:
                    self.socket.send_string(f"Tags:{self.aruco_tags}")
                else:
                    self.socket.send_string("Tags:[]") # empty list

            elif str(message) == "Image:resolution":
                print("Sending image resolution of {}x{}x{}".format(self.cropped_image.shape[0], self.cropped_image.shape[1], self.cropped_image.shape[2]))
                self.socket.send_string("Image:{}".format(self.cropped_image.shape))

            elif str(message) == "Image:image":
                # print(self.uncropped_image)
                self.socket.send(self.cropped_image.tobytes())

            elif str(message) == "Recording:start":
                print("starting recording")
                self.is_recording = False
                self.discardRecordingImages()
                self.is_recording = True
                self.socket.send_string("OK")

            elif str(message) == "Recording:discard":
                self.is_recording = False
                print("deleting images")
                self.discardRecordingImages()
                self.socket.send_string("OK")

            elif str(message).startswith("Recording:save_"):
                self.is_recording = False
                filename = str(message) [ len("Recording:save_"):] # filename is the part of the message after "save_"
                print("saving file {}".format(filename))
                self.saveVideo(filename)
                self.socket.send_string("OK")

            else:
                warnings.warn("Got an unrecognised message: {}".format(message))

    def saveVideo(self,filename):
        ffmpeg_command = "ffmpeg -y -r 30 -i {}/image%04d.png -vcodec libx264 -crf 25  -pix_fmt yuv420p {}/{}.mp4".format(temporary_image_folder_path,videos_folder_path,filename)
        os.system(ffmpeg_command)
        self.discardRecordingImages()

    def discardRecordingImages(self):
        # os.remove("{}/*".format(temporary_image_folder_path))
        os.system("rm {}/*".format(temporary_image_folder_path))
        self.i_frame = 0

    def main(self):

        # start the threaded process that gets and interprets images:
        threading.Thread(target=self.zmq_updater).start()  # starting in thread allows update of the camera at a higher frequency than zmq messages are sent
        self.camera_updater()

        print("Tracking stopped")


tracking_system = TrackingSystem()
tracking_system.main()

