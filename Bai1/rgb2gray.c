#include <stdio.h>

#define WIDTH 256
#define HEIGHT 256

void readFile(const char *inputName, unsigned char red_matrix[], unsigned char green_matrix[], unsigned char blue_matrix[])
{
    FILE *fptr = fopen(inputName, "rb");
    fread(red_matrix, sizeof(char), WIDTH * HEIGHT, fptr);
    fread(green_matrix, sizeof(char), WIDTH * HEIGHT, fptr);
    fread(blue_matrix, sizeof(char), WIDTH * HEIGHT, fptr);
    fclose(fptr);
}

void Rgb2Gray(unsigned char gray_matrix[], unsigned char red_matrix[], unsigned char green_matrix[], unsigned char blue_matrix[])
{
    // 0.299 ∙ Red + 0.587 ∙ Green + 0.114 ∙ Blue.
    int index;
    for (int x = 0; x < HEIGHT; x++)
    {
        for (int y = 0; y < WIDTH; y++)
        {
            index = x * HEIGHT + y;
            gray_matrix[index] = 0.299 * red_matrix[index] + 0.587 * green_matrix[index] + 0.114 * blue_matrix[index];
        }
    }
}

void writeFile(const char *outputName, unsigned char gray_matrix[])
{
    FILE *fptr = fopen(outputName, "wb");
    fwrite(gray_matrix, sizeof(char), WIDTH * HEIGHT, fptr);
    fclose(fptr);
}

int main()
{
    unsigned char red_matrix[WIDTH * HEIGHT];
    unsigned char green_matrix[WIDTH * HEIGHT];
    unsigned char blue_matrix[WIDTH * HEIGHT];
    unsigned char gray_matrix[WIDTH * HEIGHT];
    const char *inputName = "output/raw.dat";
    const char *outputName = "output/raw_gray.dat";

    readFile(inputName, red_matrix, green_matrix, blue_matrix);
    Rgb2Gray(gray_matrix, red_matrix, green_matrix, blue_matrix);
    writeFile(outputName, gray_matrix);
    return 0;
}