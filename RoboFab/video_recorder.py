import os
import threading
import time
import cv2

from helperFunctions import debugPrint


class VideoRecorder:
    def __init__(self, camera_pipe="0"):
        debugPrint("Setting up RoboFab webcam to record assembly")
        self.recording_frame_rate=30
        # camera_pipe = "/dev/v4l/by-id/usb-046d_Logitech_Webcam_C930e_86CF445E-video-index0"
        if camera_pipe.isdigit():
            self.vid = cv2.VideoCapture(int(camera_pipe))
        else:
            self.vid = cv2.VideoCapture(camera_pipe)

        self.save_resolution = (int(self.vid.get(cv2.CAP_PROP_FRAME_WIDTH)), int(self.vid.get(cv2.CAP_PROP_FRAME_HEIGHT)))
        self.output_video_writer = cv2.VideoWriter('./temporary_output.avi', cv2.VideoWriter_fourcc(*'XVID'), self.recording_frame_rate, self.save_resolution)
        frame_return_success, image = self.vid.read()  # first frame can take some time to get, so get one frame now to avoid stutter at the start of the video

        self.is_recording = False  # flag that dictates when frames will be recorded
        self.is_running = True  # threaded loop will stop once this flag is set False
        self.video_writer_is_busy = False # set true by the threaded recorder when in use

        threading.Thread(target=self.threaded_recorder).start()
        debugPrint("Webcam setup done",messageVerbosity=1)

    def start_recording(self):
        self.is_recording=True

    def save_recording(self, filepath_input):
        self.is_recording = False # ask threaded_recorder to stop adding more frames
        while self.video_writer_is_busy: time.sleep(0.01) # wait for threaded loop to finish

        os.makedirs( os.path.dirname(filepath_input) , exist_ok=True)  # create the folder if it doesn't already exists

        filepath = "{}.avi".format(filepath_input)
        # create unique filename:
        i = 1
        while os.path.exists(filepath):
            i += 1
            filepath = "{} ({}).avi".format(filepath_input, i)

        # release the temporary file, rename it, then restart the output_video_writer
        self.output_video_writer.release()
        debugPrint("Saving RoboFab assembly video as {}".format(filepath))
        os.rename("temporary_output.avi", filepath)
        self.output_video_writer = cv2.VideoWriter('./temporary_output.avi', cv2.VideoWriter_fourcc(*'XVID'), self.recording_frame_rate, self.save_resolution)

    # stops the thread gracefully
    def disconnect(self):
        debugPrint("Disconnecting from RoboFab webcam",messageVerbosity=0)
        self.is_running = False
        self.is_recording = False
        while self.video_writer_is_busy: time.sleep(0.01) # wait for threaded loop to finish
        self.output_video_writer.release() # releases the writing object (i.e. temporary_output.avi)
        os.remove('./temporary_output.avi') # tidy up
        self.vid.release() # releases the webcam

    def threaded_recorder(self):

        while self.is_running:
            # wait for is_recording to be set to True:
            while not self.is_recording and self.is_running: time.sleep(0.01)

            if self.is_running: # else don't start a new recording but skip to the end
                # starting recording...
                debugPrint("Starting RoboFab assembly video recording",messageVerbosity=0)
                self.video_writer_is_busy= True
                start_time = time.time()
                i_frame=0
                while self.is_recording: # keep adding frames as long as is_recording stays True
                    i_frame+=1
                    # wait for right time to add next frame
                    while time.time() < start_time + i_frame/self.recording_frame_rate:
                        time.sleep(0.001)
                    # print("{}: {}".format(i_frame,time.time()-start_time))
                    frame_return_success, image = self.vid.read()
                    self.output_video_writer.write(image)
                self.video_writer_is_busy= False # "releases" the video writer object to allow saving video

if __name__=="__main__":
    print("testing")
    webcam = VideoRecorder()
    # webcam = VideoRecorder("/dev/v4l/by-id/usb-046d_Logitech_Webcam_C930e_86CF445E-video-index0")
    webcam.start_recording()
    time.sleep(5)
    webcam.save_recording("./test_file")
    webcam.disconnect()
    print("finish")
