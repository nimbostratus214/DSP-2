#include "ImageInterpolation.h"
#include <math.h>

#ifndef M_PI
#define M_PI 3.1415926535897932
#endif

void sampleAndHold(const uchar input[], int xSize, int ySize, uchar output[], int newXSize, int newYSize)
{
	/* TO DO */
    double scaleX = ((double)xSize) / newXSize;
    double scaleY = ((double)ySize) / newYSize;
    for (int i = 0; i < newXSize; i++)
    {
        for (int j = 0; j < newYSize; j++)
        {
            int newI = floor((i - 1) * scaleX + 1);
            int newJ = floor((j - 1) * scaleY + 1);
            if (newI >= xSize) {
                newI = xSize - 1;
            }
            if (newJ >= ySize) {
                newJ = ySize - 1;
            }
            output[3 * i + j * newXSize * 3] = input[3 * newI + newJ * xSize * 3];
            output[3 * i + 1 + j * newXSize * 3] = input[3 * newI + 1 + newJ * xSize * 3];
            output[3 * i + 2 + j * newXSize * 3] = input[3 * newI + 2 + newJ * xSize * 3];
        }
    }
}

void bilinearInterpolate(const uchar input[], int xSize, int ySize, uchar output[], int newXSize, int newYSize)
{
	/* TO DO */

    double scaleX = ((double)xSize) / newXSize;
    double scaleY = ((double)ySize) / newYSize;
    for (int i = 0; i < newXSize; i++)
    {
        for (int j = 0; j < newYSize; j++)
        {
            int newI = (i)*scaleX;
            int newJ = (j)*scaleY;
            double a = j * scaleY - floor(j * scaleY);
            double b = i * scaleX - floor(i * scaleX);

            output[3 * i + j * newXSize * 3] =
                (1 - a) * (1 - b) * input[3 * newI + newJ * xSize * 3] +
                (1 - a) * b * input[3 * (newI + 1) + newJ * xSize * 3] +
                a * (1 - b) * input[3 * newI + (newJ + 1) * xSize * 3] +
                a * b * input[3 * (newI + 1) + (newJ + 1) * xSize * 3];
            output[3 * i + 1 + j * newXSize * 3] =
                (1 - a) * (1 - b) * input[3 * newI + 1 + newJ * xSize * 3] +
                (1 - a) * b * input[3 * (newI + 1) + 1 + newJ * xSize * 3] +
                a * (1 - b) * input[3 * newI + 1 + (newJ + 1) * xSize * 3] +
                a * b * input[3 * (newI + 1) + 1 + (newJ + 1) * xSize * 3];
            output[3 * i + 2 + j * newXSize * 3] =
                (1 - a) * (1 - b) * input[3 * newI + 2 + newJ * xSize * 3] +
                (1 - a) * b * input[3 * (newI + 1) + 2 + newJ * xSize * 3] +
                a * (1 - b) * input[3 * newI + 2 + (newJ + 1) * xSize * 3] +
                a * b * input[3 * (newI + 1) + 2 + (newJ + 1) * xSize * 3];
        }
    }
}

void imageRotate(const uchar input[], int xSize, int ySize, uchar output[], int m, int n, double angle)
{
	/* TO DO */
    for (int i = 0; i < xSize; i++)
    {
        for (int j = 0; j < ySize; j++)
        {
            double di = i - m;
            double dj = j - n;
            double r = sqrt(pow(di, 2) + pow(dj, 2));
            double theta = angle * M_PI / 2; //* M_PI * r;
            int newI = cos(theta) * di - sin(theta) * dj + m;
            int newJ = sin(theta) * di + cos(theta) * dj + n;

            if (newI >= 0 && newI < xSize && newJ >= 0 && newJ < ySize)
            {
                output[3 * i + j * xSize * 3] = input[3 * newI + newJ * xSize * 3];
                output[3 * i + 1 + j * xSize * 3] = input[3 * newI + 1 + newJ * xSize * 3];
                output[3 * i + 2 + j * xSize * 3] = input[3 * newI + 2 + newJ * xSize * 3];
            }
            else
            {
                output[3 * i + j * xSize * 3] = 0;
                output[3 * i + 1 + j * xSize * 3] = 0;
                output[3 * i + 2 + j * xSize * 3] = 0;
            }
        }
    }

}

void imageRotateBilinear(const uchar input[], int xSize, int ySize, uchar output[], int m, int n, double angle)
{
	/* TO DO */

    for (int i = 0; i < xSize; i++)
    {
        for (int j = 0; j < ySize; j++)
        {
            double di = i - m;
            double dj = j - n;
            double r = sqrt(pow(di, 2) + pow(dj, 2));
            double theta = angle * M_PI /2; /// angle * M_PI * r;

            double disI = cos(theta) * di - sin(theta) * dj + m;
            double disJ = sin(theta) * di + cos(theta) * dj + n;

            int newI = floor(disI);
            int newJ = floor(disJ);

            double a = disJ - newJ;
            double b = disI - newI;

            if (newI >= 0 && newI + 1 < xSize && newJ >= 0 && newJ + 1 < ySize)
            {
                output[3 * i + j * xSize * 3] =
                    (1 - a) * (1 - b) * input[3 * newI + newJ * xSize * 3] +
                    (1 - a) * b * input[3 * (newI + 1) + newJ * xSize * 3] +
                    a * (1 - b) * input[3 * newI + (newJ + 1) * xSize * 3] +
                    a * b * input[3 * (newI + 1) + (newJ + 1) * xSize * 3];
                output[3 * i + 1 + j * xSize * 3] =
                    (1 - a) * (1 - b) * input[3 * newI + 1 + newJ * xSize * 3] +
                    (1 - a) * b * input[3 * (newI + 1) + 1 + newJ * xSize * 3] +
                    a * (1 - b) * input[3 * newI + 1 + (newJ + 1) * xSize * 3] +
                    a * b * input[3 * (newI + 1) + 1 + (newJ + 1) * xSize * 3];
                output[3 * i + 2 + j * xSize * 3] =
                    (1 - a) * (1 - b) * input[3 * newI + 2 + newJ * xSize * 3] +
                    (1 - a) * b * input[3 * (newI + 1) + 2 + newJ * xSize * 3] +
                    a * (1 - b) * input[3 * newI + 2 + (newJ + 1) * xSize * 3] +
                    a * b * input[3 * (newI + 1) + 2 + (newJ + 1) * xSize * 3];
            }
            else
            {
                output[3 * i + j * xSize * 3] = 0;
                output[3 * i + 1 + j * xSize * 3] = 0;
                output[3 * i + 2 + j * xSize * 3] = 0;
            }
        }
    }


}


