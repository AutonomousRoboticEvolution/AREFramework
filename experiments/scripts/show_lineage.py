#! /usr/bin/python3

import me_data_fcts as medf
import sys
from PIL import Image
import matplotlib.pyplot as plt


parents = medf.load_parenting(sys.argv[1])
lineages = medf.compute_lineage_dict(parents)
path = sys.argv[1].split('/')
exp_folder = "/"
for folder in path[:-1]:
    exp_folder += folder + "/" 
robot_id = int(sys.argv[2])

images = []
image_name = "robot_" + str(int(robot_id)) + "_3.png"
img = Image.open(exp_folder + "/images/" + image_name)
images.append(img)
for l in lineages[robot_id]:
    image_name = "robot_" + str(int(l)) + "_3.png"
    img = Image.open(exp_folder + "/images/" + image_name)
    images.append(img)
print(len(images))
columns = 5
rows = round(len(images)/5)
height = round(len(images)/5) * 50
plt.figure(figsize=(50, height))
for (i, image), l in zip(enumerate(images), lineages[robot_id]):
    plt.subplot(rows, columns, i + 1)
    plt.title("robot_" + str(int(l)),fontsize=30)
    plt.imshow(image)
plt.show()
