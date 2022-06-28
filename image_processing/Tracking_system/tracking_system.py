#import needed files
import time
import cv2
from cv2 import aruco
import zmq
import threading
import warnings

from tracking_parameters import *

class TrackingSystem:

    def __init__(self):
        # create the video capture object, which is used to get frames from the camera
        self.vid = cv2.VideoCapture(pipe)  # pipe is set in tracking_parameters.py

        # set resolution:
        if resolution_width > 0:
            self.vid.set(cv2.CAP_PROP_FRAME_WIDTH, resolution_width)
        if resolution_height > 0:
            self.vid.set(cv2.CAP_PROP_FRAME_HEIGHT, resolution_height)

        # start the video writer object, which is used to save the video to a file
        if crop_rectangle[0]<0:
            self.save_resolution = (int(self.vid.get(cv2.CAP_PROP_FRAME_WIDTH)), int(self.vid.get(cv2.CAP_PROP_FRAME_HEIGHT)))
        else:
            self.save_resolution = ( crop_rectangle[2],crop_rectangle[3] )
        self.output_video_writer = cv2.VideoWriter('temporary_output.avi', cv2.VideoWriter_fourcc(*'XVID'), recording_frame_rate, self.save_resolution )
        self.is_recording = False # this boolean keeps track of whether to be saving frames or not
        self.recording_start_time=0.0
        self.recording_frame_number=0
        self.video_writer_is_busy = False

        #set up zmq
        context = zmq.Context()
        self.socket = context.socket(zmq.REP)
        self.socket.bind("tcp://*:{}".format(zmq_port_number))

        # the robot and aruco_tags objects that are updated inside the threaded_updater:
        self.cropped_image = None
        self.robot = None
        self.aruco_tags = []
        self.frame_return_success= False # from openCV this will be `false` if no frames has been grabbed, e.g. failure to connect to camera
        self.last_frame_grab_time=time.time()


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
    def robot_location(self,image,hsv_mins,hsv_maxs):
        #converts image to hsv
        hsv = cv2.cvtColor(image,cv2.COLOR_BGR2HSV)
        
        masks = []
        for i in range(len(hsv_mins)):
            #create the black/white mask
            hsv_min, hsv_max = hsv_mins[i], hsv_maxs[i]
            masks += [cv2.inRange(hsv, hsv_min, hsv_max)]
        
        mask =  masks[0]
        if len(masks)>1:
            for m in masks[1:]:
                mask = cv2.add(mask, m)
        
        
        detector = cv2.SimpleBlobDetector_create(blob_detection_parameters)

        #fills holes in mask to improve blob detection
        fix_mask = True
        if fix_mask:
            fixed_mask = self.fillHoles(mask)
            keypoints = detector.detect(fixed_mask)
        else:
            keypoints = detector.detect(mask)

        #displays mask and blobs
        cv2.waitKey(1)
        if show_frames:
            kp_image = cv2.drawKeypoints(mask,keypoints,None,color=(0,0,255),flags= cv2.DRAW_MATCHES_FLAGS_DRAW_RICH_KEYPOINTS)

            cv2.imshow("cropped",self.cropped_image)
            cv2.imshow("Mask",mask)
            cv2.imshow("Blob",kp_image)

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


    def camera_updater(self):

        print("Tracking loop started")
        # loop for tracking system
        while (1):
            self.frame_return_success, uncropped_image = self.vid.read()

            timeNow = time.time()
            if show_fps: print("{} fps".format( round ( 1/(timeNow-self.last_frame_grab_time) , 2 ) ))
            self.last_frame_grab_time=timeNow

            if self.frame_return_success:
                self.cropped_image = self.crop_image(uncropped_image)

                # compute robot location
                self.robot = self.robot_location(self.cropped_image, brainMin, brainMax)

                # compute tag locations
                self.aruco_tags = self.aruco_locations(self.cropped_image)

            else:
                warnings.warn("OpenCV read() failed")

    def recording_updater(self):

        while(1):
            if self.is_recording:
                self.recording_frame_number += 1

                # wait for right time
                while( time.time() < self.recording_start_time + (self.recording_frame_number / recording_frame_rate)):
                    time.sleep(0.01)
                self.video_writer_is_busy=True
                self.output_video_writer.write(self.cropped_image)
                self.video_writer_is_busy=False

                # check for timeout
                if time.time() > self.recording_start_time + recording_timeout_seconds:
                    self.is_recording=False
                    print("Recording timed out")

            else:
                time.sleep(0.01)

    def zmq_updater(self):
        #main loop waiting for a zmq message
        print("Starting loop for zmq messages")
        while(1):
            # short sleep
            # time.sleep(1/1000)

            # wait for zmq request
            message = self.socket.recv().decode('utf-8')
            #if verbose_messages: print("Got message ",message)

            if str(message) == "Robot:position":
                # send the latest value of self.robot
                if verbose_messages: ("Sending robot position: {}".format(self.robot))
                if self.robot is not None and self.frame_return_success:
                    self.socket.send_string(f"Robot:{self.robot}")
                else:
                    self.socket.send_string("Robot:None")

            elif str(message) == "Tags:position":
                # send the latest value of self.aruco_tags
                if verbose_messages: ("Sending aruco tags position(s): {}".format(self.aruco_tags))
                if self.frame_return_success:
                    self.socket.send_string(f"Tags:{self.aruco_tags}")
                else:
                    self.socket.send_string("Tags:[]") # empty list

            elif str(message) == "Image:resolution":
                while(self.cropped_image is None): time.sleep(0.1)
                if verbose_messages: print("Sending image resolution of {}x{}x{}".format(self.cropped_image.shape[0], self.cropped_image.shape[1], self.cropped_image.shape[2]))
                self.socket.send_string("Image:{}".format(self.cropped_image.shape))

            elif str(message) == "Image:image":
                if verbose_messages: print("Got message ", message)
                self.socket.send(self.cropped_image.tobytes())

            elif str(message) == "Recording:start":
                if verbose_messages: print("starting recording")
                self.recording_frame_number=0
                self.recording_start_time=time.time()
                self.is_recording = True

                self.socket.send_string("OK")

            elif str(message) == "Recording:discard":
                self.is_recording = False
                while ( self.video_writer_is_busy ):time.sleep(0.01)
                if verbose_messages: print("discarding video")
                self.output_video_writer.release()
                os.remove("temporary_output.avi")
                self.output_video_writer.open('temporary_output.avi', cv2.VideoWriter_fourcc(*'XVID'), recording_frame_rate, self.save_resolution )

                self.socket.send_string("OK")

            elif str(message).startswith("Recording:save_"):
                self.is_recording = False
                while ( self.video_writer_is_busy ):time.sleep(0.01)
                self.output_video_writer.release()
                filename = str(message) [ len("Recording:save_"):] # filename is the part of the message after "save_"

                # create unique filename:
                i=1
                filePath = "{}/{} ({}).avi".format(videos_folder_path, filename, i)
                while os.path.exists(filePath):
                    i+=1
                    filePath = "{}/{} ({}).avi".format( videos_folder_path , filename,i)
                if verbose_messages: print("saving as file {}".format(filePath))
                os.rename("temporary_output.avi",filePath)
                self.output_video_writer.open('temporary_output.avi', cv2.VideoWriter_fourcc(*'XVID'), recording_frame_rate, self.save_resolution )

                self.socket.send_string("OK")

            else:
                warnings.warn("Got an unrecognised message: {}".format(message))


    def main(self):

        # start the threaded process that gets and interprets images:
        threading.Thread(target=self.zmq_updater).start()  # starting in thread allows update of the camera at a higher frequency than zmq messages are sent
        threading.Thread(target=self.recording_updater).start() # will save images into a video when requested
        self.camera_updater()

        print("Tracking stopped")


tracking_system = TrackingSystem()
tracking_system.main()

