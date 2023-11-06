#include <stdio.h>
#include <math.h>
#define KERNEL_X 3
#define KERNEL_Y 3
#define WIDTH 256
#define HEIGHT 256

void readFile(const char *inputName, unsigned char gray_matrix[])
{
    FILE *fptr = fopen(inputName, "rb");
    fread(gray_matrix, sizeof(char), WIDTH * HEIGHT, fptr);
    fclose(fptr);
}

void conv(unsigned char *in, unsigned char *out, int dataSizeX, int dataSizeY,
          float *kernel, int kernelSizeX, int kernelSizeY)
{
    int i, j, m, n;
    unsigned char *inPtr, *inPtr2, *outPtr;
    float *kPtr;
    int kCenterX, kCenterY;
    int rowMin, rowMax; // to check boundary of input array
    int colMin, colMax; //
    float sum;          // temp accumulation buffer

    // find center position of kernel (half of kernel size)
    kCenterX = kernelSizeX >> 1;
    kCenterY = kernelSizeY >> 1;

    // init working  pointers
    inPtr = inPtr2 = &in[dataSizeX * kCenterY + kCenterX]; // note that  it is shifted (kCenterX, kCenterY),
    outPtr = out;
    kPtr = kernel;

    // start convolution
    for (i = 0; i < dataSizeY; ++i) // number of rows
    {
        // compute the range of convolution, the current row of kernel should be between these
        rowMax = i + kCenterY;
        rowMin = i - dataSizeY + kCenterY;

        for (j = 0; j < dataSizeX; ++j) // number of columns
        {
            // compute the range of convolution, the current column of kernel should be between these
            colMax = j + kCenterX;
            colMin = j - dataSizeX + kCenterX;

            sum = 0; // set to 0 before accumulate

            // flip the kernel and traverse all the kernel values
            // multiply each kernel value with underlying input data
            for (m = 0; m < kernelSizeY; ++m) // kernel rows
            {
                // check if the index is out of bound of input array
                if (m <= rowMax && m > rowMin)
                {
                    for (n = 0; n < kernelSizeX; ++n)
                    {
                        // check the boundary of array
                        if (n <= colMax && n > colMin)
                            sum += *(inPtr - n) * *kPtr;

                        ++kPtr; // next kernel
                    }
                }
                else
                    kPtr += kernelSizeX; // out of bound, move to next row of kernel

                inPtr -= dataSizeX; // move input data 1 raw up
            }

            // convert negative number to positive
            *outPtr = (unsigned char)((float)fabs(sum) + 0.5f);

            kPtr = kernel;    // reset kernel to (0,0)
            inPtr = ++inPtr2; // next input
            ++outPtr;         // next output
        }
    }
}

void Gray2Bin(unsigned char bin_matrix[], unsigned char gray_matrix[])
{
    unsigned char threshold = 0x7F;
    unsigned char temp = 0x00;
    unsigned char countBit = 0;
    unsigned int index = 0;
    for (int x = 0; x < HEIGHT; x++)
    {
        for (int y = 0; y < WIDTH; y++)
        {
            if (countBit == 8)
            {
                bin_matrix[index++] = temp;
                temp = 0x00;
                countBit = 0;
            }
            temp <<= 1;
            countBit++;
            if (gray_matrix[x * HEIGHT + y] > threshold)
            {
                temp |= 0x01;
            }
        }
    }
}

void writeFile(const char *outputName, unsigned char bin_matrix[])
{
    FILE *fptr = fopen(outputName, "wb");
    fwrite(bin_matrix, sizeof(char), (WIDTH * HEIGHT) / 8, fptr);
    fclose(fptr);
}
int main()
{
    float kernel[] = {-1, -2, -1, 0, 0, 0, 1, 2, 1};
    unsigned char gray_matrix[WIDTH * HEIGHT];
    unsigned char edge_detect_matrix[WIDTH * HEIGHT];
    unsigned char bin_matrix[(WIDTH * HEIGHT) / 8];
    const char *inputName = "input/raw_gray.dat";
    const char *outputName = "output/edge_detect.dat";
    readFile(inputName, gray_matrix);
    conv(gray_matrix, edge_detect_matrix, WIDTH, HEIGHT, kernel, KERNEL_X, KERNEL_Y);
    Gray2Bin(bin_matrix, edge_detect_matrix);
    writeFile(outputName, bin_matrix);
    return 0;
}
