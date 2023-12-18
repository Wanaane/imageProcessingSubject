#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#define HEIGHT 480
#define WIDTH 640

void readRGBFile(const char *inputName,
                 uint8_t *red, uint8_t *green, uint8_t *blue,
                 uint32_t width, uint32_t height)
{
    FILE *fptr = fopen(inputName, "rb");
    fread(red, sizeof(uint8_t), width * height, fptr);
    fread(green, sizeof(uint8_t), width * height, fptr);
    fread(blue, sizeof(uint8_t), width * height, fptr);
    fclose(fptr);
}

void writeGrayImage(const char *outputName, uint8_t *gray,
                    uint32_t width, uint32_t height)
{
    FILE *fptr = fopen(outputName, "wb");
    fwrite(gray, sizeof(uint8_t), width * height, fptr);
    fclose(fptr);
}

void writeBinImage(const char *outputName, uint8_t *bin,
                   uint32_t width, uint32_t height)
{
    uint32_t size = width * height;
    uint8_t temp = 0, len = 0;

    FILE *fptr = fopen(outputName, "wb");
    for (uint32_t i = 0; i < size; i++)
    {
        if (len == 8)
        {
            fwrite(&temp, 1, 1, fptr);
            temp = 0;
            len = 0;
        }
        temp = temp << 1;
        temp = temp | (bin[i] & 0x1);
        len++;
    }
    fwrite(&temp, 1, 1, fptr);
    fclose(fptr);
}

void grayScale(uint8_t *gray, const char *inputName,
               uint32_t width, uint32_t height)
{
    uint32_t size = width * height;
    uint8_t *red = (uint8_t *)malloc(size * sizeof(uint8_t));
    uint8_t *green = (uint8_t *)malloc(size * sizeof(uint8_t));
    uint8_t *blue = (uint8_t *)malloc(size * sizeof(uint8_t));
    readRGBFile(inputName, red, green, blue, width, height);

    for (int i = 0; i < size; i++)
    {
        gray[i] = (uint8_t)(0.299 * red[i] + 0.587 * green[i] + 0.114 * blue[i]);
    }
    free(red);
    free(green);
    free(blue);
}

void subtraction(uint8_t *result, uint8_t *referenceImg, uint8_t *currentImg,
                 uint32_t width, uint32_t height)
{
    uint32_t size = width * height;
    for (int i = 0; i < size; i++)
    {
        result[i] = abs(referenceImg[i] - currentImg[i]);
    }
}

void thresholding(uint8_t *outImage, uint8_t *inImage, uint8_t threshold,
                  uint32_t width, uint32_t height)
{
    uint32_t size = width * height;
    for (int i = 0; i < size; i++)
    {
        outImage[i] = (inImage[i] > threshold) ? 1 : 0;
    }
}

void *medianFilter(uint8_t *inImage, uint8_t *outImage,
                   uint32_t width, uint32_t height, uint32_t kernelSize)
{
    uint32_t *kernel = (uint32_t *)calloc(kernelSize * kernelSize, sizeof(uint32_t));
    uint32_t centerKernel = kernelSize / 2;
    int32_t row, col;

    for (uint32_t i = 0; i < height; i++)
    {
        for (uint32_t j = 0; j < width; j++)
        {
            // Get value in kernel
            for (uint32_t m = 0; m < kernelSize; m++)
            {
                row = i + m - centerKernel;
                for (uint32_t n = 0; n < kernelSize; n++)
                {
                    col = j + n - centerKernel;
                    if (row >= 0 && row < height && col >= 0 && col < width)
                        kernel[m * kernelSize + n] = inImage[row * width + col];
                    else
                        kernel[m * kernelSize + n] = 0;
                }
            }

            // Sort kernel
            uint32_t temp;
            for (uint32_t m = 0; m < kernelSize * kernelSize - 1; m++)
            {
                for (uint32_t n = m + 1; n < kernelSize * kernelSize; n++)
                {
                    if (kernel[m] > kernel[n])
                    {
                        temp = kernel[m];
                        kernel[m] = kernel[n];
                        kernel[n] = temp;
                    }
                }
            }

            // Get median value
            outImage[i * width + j] = kernel[kernelSize * kernelSize / 2];
        }
    }

    free(kernel);
}

void segmentation(uint8_t *img, uint32_t width, uint32_t height)
{
    uint32_t *sumX = (uint32_t *)calloc(width, sizeof(uint32_t));
    uint32_t *sumY = (uint32_t *)calloc(height, sizeof(uint32_t));

    for (uint32_t i = 0; i < width; i++)
    {
        for (uint32_t j = 0; j < height; j++)
        {
            sumX[i] += img[j * width + i];
        }
    }

    for (uint32_t i = 0; i < height; i++)
    {
        for (uint32_t j = 0; j < width; j++)
        {
            sumY[i] += img[i * width + j];
        }
    }

    uint32_t maxX = sumX[0], maxY = sumX[0];
    for (uint32_t i = 1; i < width; i++)
    {
        if (sumX[maxX] < sumX[i])
            maxX = i;
    }
    for (uint32_t i = 1; i < height; i++)
    {
        if (sumY[maxY] < sumY[i])
            maxY = i;
    }

    printf("Position of object: (%d, %d)\n", maxX, maxY);

    // draw line on image
    // x-size = 50
    // y-size = 50

    for (uint32_t col = maxX - 100; col < maxX + 100; col++)
    {
        uint32_t y1 = maxY - 100;
        uint32_t y2 = maxY + 100;
        img[y1 * width + col] = 1;
        img[y2 * width + col] = 1;
    }

    for (uint32_t row = maxY - 100; row < maxY + 100; row++)
    {
        uint32_t x1 = maxX - 100;
        uint32_t x2 = maxX + 100;
        img[row * width + x1] = 1;
        img[row * width + x2] = 1;
    }

    free(sumX);
    free(sumY);
}

int main()
{
    uint32_t size = HEIGHT * WIDTH;
    // Read rgb image and convert to grayscale
    uint8_t *referenceImg = (uint8_t *)malloc(size * sizeof(uint8_t));
    uint8_t *currentImg = (uint8_t *)malloc(size * sizeof(uint8_t));
    grayScale(referenceImg, "./data/reference_image.dat", WIDTH, HEIGHT);
    grayScale(currentImg, "./data/current_image.dat", WIDTH, HEIGHT);

    // Subtraction
    uint8_t *subtractionImg = (uint8_t *)malloc(size * sizeof(uint8_t));
    subtraction(subtractionImg, referenceImg, currentImg, WIDTH, HEIGHT);
    // Write subtraction image
    writeGrayImage("./out/subtraction_image.dat", subtractionImg, WIDTH, HEIGHT);

    // Thresholding
    uint8_t *thresholdingImg = (uint8_t *)malloc(size * sizeof(uint8_t));
    uint8_t threshold = 75;
    thresholding(thresholdingImg, subtractionImg, threshold, WIDTH, HEIGHT);
    writeBinImage("./out/thresholding_image.dat", thresholdingImg, WIDTH, HEIGHT);
    // Median filter
    uint8_t *filteredImg = (uint8_t *)malloc(size * sizeof(uint8_t));
    medianFilter(thresholdingImg, filteredImg, WIDTH, HEIGHT, 3);

    // Write filtered binary image
    writeBinImage("./out/filtered_image.dat", filteredImg, WIDTH, HEIGHT);

    // Segmentation
    segmentation(filteredImg, WIDTH, HEIGHT);
    writeBinImage("./out/segmentation.dat", filteredImg, WIDTH, HEIGHT);

    // Free memory of pointer
    free(referenceImg);
    free(currentImg);
    free(subtractionImg);
    free(thresholdingImg);
    free(filteredImg);
    return 0;
}
