#!/usr/bin/env python3

# Usage:
# python3 <name of python file> <number of columns> [ list of filenames of images ]
# Note: First image's dimensions will be used to determine grid size. Assumes all images have the same dimensions.

import sys
import os
import math
from PIL import Image

def create_list_images(folder):
    list_images = []    
    for file in os.listdir(folder):
        if(file.split("_")[0] == "robot" and file.split("_")[2] == "2.png"):
            list_images.append(folder + "/" + file)
    return list_images

def create_grid_images(folder, numcols, list_images):
    numimages = len(list_images) # Ignore name of program and column input
    numrows = math.ceil(numimages / numcols) # Number of rows

    sampleimage = Image.open(list_images[0]) # Open first image, just to get dimensions
    width, height = sampleimage.size # PIL uses (x, y)

    outimg = Image.new("RGBA", (numcols * width, numrows * height), (0, 0, 0, 0)) # Initialize to transparent

    # Write to output image. This approach copies pixels from the source image
    for i in range(numimages):
        currimage = Image.open(list_images[i])
        for j in range(width):
            for k in range(height):
                currimgpixel = currimage.getpixel((j, k))
                outimg.putpixel(((i % numcols * width) + j, (i // numcols * height) + k), currimgpixel)
    outimg.save(folder + "/images_grid.png")

if __name__ == '__main__':
    list_images = create_list_images(sys.argv[1])
    create_grid_images(sys.argv[1],5,list_images)
