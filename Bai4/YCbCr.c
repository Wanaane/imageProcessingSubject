#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#define WIDTH 256
#define HEIGHT 256

typedef enum
{
    none, // no sampling format
    YUY2, // sampling format 4:2:2
    YV12, // sampling format 4:2:0
} SAMPLING_FORMAT;

typedef struct
{
    uint32_t width;
    uint32_t height;
    uint8_t *red;
    uint8_t *green;
    uint8_t *blue;
} RGB;

typedef struct
{
    SAMPLING_FORMAT typeSampling;
    uint32_t width;
    uint32_t height;
    uint8_t *Y;
    uint8_t *Cb;
    uint8_t *Cr;
} YCbCr;

void initRGB(RGB *rgbImage, uint32_t width, uint32_t height)
{
    /*
     *dynamic declaration of memory for RGB image object
     * @param rgbImage: RGB image object with width, height, red, green, blue hold data of RGB image
     */
    uint32_t size = width * height;
    rgbImage->width = width;
    rgbImage->height = height;
    rgbImage->red = (uint8_t *)malloc(size * sizeof(uint8_t));
    rgbImage->green = (uint8_t *)malloc(size * sizeof(uint8_t));
    rgbImage->blue = (uint8_t *)malloc(size * sizeof(uint8_t));
}

void initYCBCr(YCbCr *YCbCrImage, uint32_t width, uint32_t height, SAMPLING_FORMAT typeSampling)
{
    /*
     *dynamic declaration of memory for YCbCr image object
     * @param rgbImage: RGB image object with width, height, y, cb, cr hold data of YCbCr image
     */
    uint32_t size = width * height;
    uint32_t sizeCbCr = size;
    if (typeSampling == YUY2)
    {
        sizeCbCr /= 2;
    }
    else if (typeSampling == YV12)
    {
        sizeCbCr /= 4;
    }

    YCbCrImage->width = width;
    YCbCrImage->height = height;
    YCbCrImage->typeSampling = typeSampling;
    YCbCrImage->Y = (uint8_t *)malloc(size * sizeof(uint8_t));
    YCbCrImage->Cb = (uint8_t *)malloc(sizeCbCr * sizeof(uint8_t));
    YCbCrImage->Cr = (uint8_t *)malloc(sizeCbCr * sizeof(uint8_t));
}

void freeRGB(RGB *rgbImage)
{
    // free memory of red, green, blue array in RGB image object
    free(rgbImage->red);
    free(rgbImage->green);
    free(rgbImage->blue);
}

void freeYCbCr(YCbCr *rgbImage)
{
    // free memory of Y, Cb, Cr array in YCbCr image object
    free(rgbImage->Y);
    free(rgbImage->Cb);
    free(rgbImage->Cr);
}

void readRgbFile(const char *inputName, RGB *rgbImage)
{
    uint32_t size = rgbImage->width * rgbImage->height;
    FILE *fptr = fopen(inputName, "rb");
    fread(rgbImage->red, sizeof(uint8_t), size, fptr);
    fread(rgbImage->green, sizeof(uint8_t), size, fptr);
    fread(rgbImage->blue, sizeof(uint8_t), size, fptr);
    fclose(fptr);
}

void writeYCbCrFile(const char *outputPath, YCbCr *YCbCrImage)
{
    uint32_t size = YCbCrImage->width * YCbCrImage->height;
    uint32_t sizeCbCr = size;
    if (YCbCrImage->typeSampling == YUY2)
    {
        sizeCbCr /= 2;
    }
    else if (YCbCrImage->typeSampling == YV12)
    {
        sizeCbCr /= 4;
    }

    FILE *fptr = fopen(outputPath, "wb");
    fwrite(YCbCrImage->Y, sizeof(uint8_t), size, fptr);
    fwrite(YCbCrImage->Cb, sizeof(uint8_t), sizeCbCr, fptr);
    fwrite(YCbCrImage->Cr, sizeof(uint8_t), sizeCbCr, fptr);
    fclose(fptr);
}

void Rgb2YCbCr(RGB *rgbImage, YCbCr *YCbCrImage)
{
    /*
     * Convert RGB to YCbCr formula : standard ITU-R BT.601
     * BT.601 Documentation: https://www.itu.int/dms_pubrec/itu-r/rec/bt/R-REC-BT.601-7-201103-I!!PDF-E.pdf

     */

    uint32_t index;
    uint32_t width = rgbImage->width;
    uint32_t height = rgbImage->height;
    initYCBCr(YCbCrImage, width, height, none);
    uint8_t r, g, b;
    for (uint32_t i = 0; i < height; i++)
    {
        for (uint32_t j = 0; j < width; j++)
        {
            index = i * width + j;
            r = *(rgbImage->red + index);
            g = *(rgbImage->green + index);
            b = *(rgbImage->blue + index);

            *(YCbCrImage->Y + index) = 0.299 * r + 0.587 * g + 0.114 * b;
            *(YCbCrImage->Cb + index) = 128 - 0.168736 * r - 0.331264 * g + 0.5 * b;
            *(YCbCrImage->Cr + index) = 128 + 0.5 * r - 0.418688 * g - 0.081312 * b;
        }
    }
}

void sampling(YCbCr *inYCbCrImage, YCbCr *outYCbCrImage, SAMPLING_FORMAT typeSampling)
{
    uint32_t width = inYCbCrImage->width;
    uint32_t height = inYCbCrImage->height;
    uint32_t step = typeSampling == YUY2 ? 1 : 2;
    uint32_t inIndex;
    uint32_t outIndex = 0;
    initYCBCr(outYCbCrImage, width, height, typeSampling);
    outYCbCrImage->Y = inYCbCrImage->Y;

    for (uint32_t i = 0; i < height; i += step)
    {
        for (uint32_t j = 0; j < width; j += 2)
        {
            inIndex = i * height + j;
            *(outYCbCrImage->Cb + outIndex) = *(inYCbCrImage->Cb + inIndex);
            *(outYCbCrImage->Cr + outIndex) = *(inYCbCrImage->Cr + inIndex);
            outIndex++;
        }
    }
}

void recoverSampling(YCbCr *encodedImage, YCbCr *recoveredImage)
{
    SAMPLING_FORMAT typeSampling = encodedImage->typeSampling;
    uint32_t width = encodedImage->width;
    uint32_t height = encodedImage->height;
    uint32_t step = typeSampling == YUY2 ? 1 : 2;
    uint32_t inIndex;
    uint32_t outIndex = 0;
    initYCBCr(recoveredImage, width, height, none);
    recoveredImage->Y = encodedImage->Y;

    // Copy data Cb, Cr from encodedImage to recoveredImage into the original position
    for (uint32_t i = 0; i < height; i += step)
    {
        for (uint32_t j = 0; j < width; j += 2)
        {
            inIndex = i * height + j;
            *(recoveredImage->Cb + inIndex) = *(encodedImage->Cb + outIndex);
            *(recoveredImage->Cr + inIndex) = *(encodedImage->Cr + outIndex);
            outIndex++;
        }
    }

    // Interpolate data Cb, Cr
    // Interpolate by horizontal for 4:2:0 sampling
    uint32_t index;
    uint32_t preRowIndex;
    if (typeSampling == YV12)
    {
        for (uint32_t i = 1; i < height; i += 2)
        {
            for (uint32_t j = 0; j < width; j += 2)
            {
                index = i * height + j;
                preRowIndex = (i - 1) * height + j;
                *(recoveredImage->Cb + index) = *(recoveredImage->Cb + preRowIndex);
                *(recoveredImage->Cr + index) = *(recoveredImage->Cr + preRowIndex);
            }
        }
    }

    // Interpolate by vertical for 4:2:2 sampling and 4:2:0 sampling
    int preColIndex;
    for (uint32_t i = 0; i < height; i += 1)
    {
        for (uint32_t j = 1; j < width; j += 2)
        {
            index = i * height + j;
            preColIndex = index - 1;
            *(recoveredImage->Cb + index) = *(recoveredImage->Cb + preColIndex);
            *(recoveredImage->Cr + index) = *(recoveredImage->Cr + preColIndex);
        }
    }
}

double MSE(YCbCr *originalImage, YCbCr *recoveredImage)
{
    uint32_t width = originalImage->width;
    uint32_t height = originalImage->height;
    uint32_t size = width * height;
    uint32_t index;
    double sum = 0;
    for (uint32_t i = 0; i < height; i++)
    {
        for (uint32_t j = 0; j < width; j++)
        {
            index = i * width + j;
            sum += pow(*(originalImage->Cb + index) - *(recoveredImage->Cb + index), 2);
            sum += pow(*(originalImage->Cr + index) - *(recoveredImage->Cr + index), 2);
        }
    }
    return sum / size;
}

double PSNR(YCbCr *originalImage, YCbCr *recoveredImage)
{
    double mse = MSE(originalImage, recoveredImage);
    return 10 * log10(pow(255, 2) / mse);
}

int main()
{
    const char *inputNamePath = "data/lena.dat";
    const char *output444Path = "out/444.dat";
    const char *output422Path = "out/422.dat";
    const char *output420Path = "out/420.dat";
    const char *output442RecoveryPath = "recovery/422Recovery.dat";
    const char *output420RecoveryPath = "recovery/420Recovery.dat";

    /******************************************************************************************/
    // Create RGB image object and read data from file
    RGB *dataImage = (RGB *)malloc(sizeof(RGB));
    initRGB(dataImage, WIDTH, HEIGHT);
    readRgbFile(inputNamePath, dataImage);

    // Create YCbCr image object and convert digital RGB to digital YCbCr
    YCbCr *YCbCrImage = (YCbCr *)malloc(sizeof(YCbCr));
    Rgb2YCbCr(dataImage, YCbCrImage);
    // Write 444 to file
    writeYCbCrFile(output444Path, YCbCrImage);

    /******************************************************************************************/
    // Sampling
    //  YCrCb sampling with format YUY2 4:2:2
    YCbCr *YCbCr422 = (YCbCr *)malloc(sizeof(YCbCr));
    sampling(YCbCrImage, YCbCr422, YUY2);
    // Write 442 to file
    writeYCbCrFile(output422Path, YCbCr422);

    // YCrCb sampling with format YUY2 4:2:0
    YCbCr *YCbCr420 = (YCbCr *)malloc(sizeof(YCbCr));
    sampling(YCbCrImage, YCbCr420, YV12);
    // Write 420 to file
    writeYCbCrFile(output420Path, YCbCr420);

    /******************************************************************************************/
    // Recover sampling
    // For 4:2:2 sampling
    YCbCr *recoveredImageFrom422Sampling = (YCbCr *)malloc(sizeof(YCbCr));
    recoverSampling(YCbCr422, recoveredImageFrom422Sampling);
    writeYCbCrFile(output442RecoveryPath, recoveredImageFrom422Sampling);

    // For 4:2:0 sampling
    YCbCr *recoveredImageFrom420Sampling = (YCbCr *)malloc(sizeof(YCbCr));
    recoverSampling(YCbCr420, recoveredImageFrom420Sampling);
    writeYCbCrFile(output420RecoveryPath, recoveredImageFrom420Sampling);

    /******************************************************************************************/
    // Calculate MSE and PSNR
    printf("MSE between 4:4:4 and 4:2:2 is: %.4f\n", MSE(YCbCrImage, recoveredImageFrom422Sampling));
    printf("PSNR between 4:4:4 and 4:2:2 is: %.4f\n", PSNR(YCbCrImage, recoveredImageFrom422Sampling));

    printf("MSE between 4:4:4 and 4:2:0 is: %.4f\n", MSE(YCbCrImage, recoveredImageFrom420Sampling));
    printf("PSNR between 4:4:4 and 4:2:0 is: %.4fDb\n", PSNR(YCbCrImage, recoveredImageFrom420Sampling));

    /******************************************************************************************/
    // Free memory
    freeRGB(dataImage);
    freeYCbCr(YCbCrImage);
    freeYCbCr(YCbCr422);
    freeYCbCr(YCbCr420);

    free(dataImage);
    free(YCbCrImage);
    free(YCbCr422);
    free(YCbCr420);

    return 0;
}