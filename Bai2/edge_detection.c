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

void conv(unsigned char in[], unsigned char out[], int width, int height,
          float kernel[], int kernelSizeX, int kernelSizeY)
{
    int i, j, m, n, mm, nn;
    int kCenterX, kCenterY;
    int rowIndex, colIndex;
    float sum;
    int index = 0;
    // find center position of kernel (half of kernel size)
    kCenterX = kernelSizeX / 2;
    kCenterY = kernelSizeY / 2;

    for (i = 0; i < height; ++i) // rows
    {
        for (j = 0; j < width; ++j) // columns
        {
            sum = 0;                          // init to 0 before sum
            for (m = 0; m < kernelSizeY; ++m) // kernel rows
            {
                mm = kernelSizeY - 1 - m; // row index of flipped kernel

                for (n = 0; n < kernelSizeX; ++n) // kernel columns
                {
                    nn = kernelSizeX - 1 - n; // column index of flipped kernel

                    // index of input signal, used for checking boundary
                    rowIndex = i + m - kCenterY;
                    colIndex = j + n - kCenterX;

                    // ignore input samples which are out of bound
                    if (rowIndex >= 0 && rowIndex < height && colIndex >= 0 && colIndex < width)
                        sum += in[rowIndex * width + colIndex] * kernel[mm * kernelSizeX + nn];
                }
            }
            out[index++] = (unsigned char)fabs(sum);
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
            if (gray_matrix[x * WIDTH + y] > threshold)
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
