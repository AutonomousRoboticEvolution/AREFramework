#!/usr/bin/env python3
import cv2
import sys

pipe = sys.argv[1]

video = cv2.VideoCapture(pipe)
if not video:
    print("unable to open pipeline")
    exit(1)
while True:
    ret,image = video.read()
    if not ret:
        print("cv read failed")
        break
    cv2.imshow("image",image)
    cv2.waitKey(1)

