from PIL import Image
import numpy as np
import matplotlib.pyplot as plt

with open("./out/subtraction_image.dat", "rb") as f:
    bin_data = f.read()
img = Image.frombytes("L", (640, 480), bin_data)
img.save("./image/subtraction_image.png")


with open("./out/thresholding_image.dat", "rb") as f:
    bin_data = f.read()
img = Image.frombytes("1", (640, 480), bin_data)
img.save("./image/thresholding_image.png")


with open("./out/filtered_image.dat", "rb") as f:
    bin_data = f.read()
img = Image.frombytes("1", (640, 480), bin_data)
img.save("./image/filtered_image.png")

with open("./out/segmentation.dat", "rb") as f:
    bin_data = f.read()
img = Image.frombytes("1", (640, 480), bin_data)
img.save("./image/segmentation.png")
