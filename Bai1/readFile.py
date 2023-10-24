from PIL import Image
import numpy as np
import os


# Argument input
dir_path = os.path.dirname(__file__)
image_name = "Lena.jpg"
output_name = "raw.dat"


# Read image
image_path = os.path.join(dir_path, image_name)
img = Image.open(fp=image_path, mode="r")


# handle image matrix
img_matrix = np.array(img)
red_matrix = img_matrix[:, :, 0]
green_matrix = img_matrix[:, :, 1]
blue_matrix = img_matrix[:, :, 2]

# write file
output_path = os.path.join(dir_path, "output", output_name)
with open(output_path, mode="wb") as f:
    red_matrix.tofile(f)
    green_matrix.tofile(f)
    blue_matrix.tofile(f)
