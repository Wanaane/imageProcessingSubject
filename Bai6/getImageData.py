from PIL import Image
import numpy as np


def writeImage(image, path):
    image = np.array(image)
    red = image[:, :, 0]
    green = image[:, :, 1]
    blue = image[:, :, 2]

    # write file
    with open(path, mode="wb") as f:
        red.tofile(f)
        green.tofile(f)
        blue.tofile(f)


reference_image_path = "./image/reference_image.jpg"
current_image_path = "./image/current_image.jpg"

# read image
reference_image = Image.open(fp=reference_image_path, mode="r")
current_image = Image.open(fp=current_image_path, mode="r")

# write image
writeImage(reference_image, "./data/reference_image.dat")
writeImage(current_image, "./data/current_image.dat")
