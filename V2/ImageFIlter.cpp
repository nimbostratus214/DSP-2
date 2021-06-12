/*Autor: Radenko Mihajlovic RA 214/2018 ///  Datum: 11.03.2021.*/

#include "ImageFilter.h"


void convolve2D (uchar image[], int xSize, int ySize, double filterCoeff[], int N)
{
	//TO DO

	int delta = (N - 1) / 2;
	uchar* Y_ext = new uchar[(xSize + 2 * delta) * (ySize + 2 * delta)];
	extendBorders(image, xSize, ySize, Y_ext, delta);

	for (int h = 0; h < xSize; h++) {
		for (int v = 0; v < ySize; v++) {

			double res = 0;
			for (int k = 0; k < N; k++) {
				for (int n = 0; n < N; n++) {
					res += filterCoeff[k + n * N] * Y_ext[h + k + (v + n) * (xSize + 2 * delta)];
				}
			}
			image[h + v * xSize] = abs(res);
		}
	}
	delete Y_ext;
}

void extendBorders(uchar input[], int xSize, int ySize, uchar output[], int delta)
{
	int newXSize = xSize + 2 * delta;
	int newYSize = ySize + 2 * delta;

	for (int i = 0; i < xSize; i++)
	{
		for (int j = 0; j < ySize; j++)
		{
			output[(j + delta) * newXSize + i + delta] = input[j * xSize + i];
		}
	}
	
	for (int i = 0; i < delta; i++)
	{
		memcpy(&output[i * newXSize + delta], &input[0], xSize);
		memcpy(&output[(ySize + delta + i)*newXSize + delta], &input[(ySize - 1) * xSize], xSize);
	}
	
	for (int i = 0; i < newYSize; i++)
	{
		memset(&output[i * newXSize], output[i * newXSize + delta], delta);
		memset(&output[i * newXSize + delta + xSize], output[i * newXSize + xSize + delta - 1], delta);
	}
}
	
void performNFFilter (uchar input[], int xSize, int ySize)
{
	//TO DO
	double filterCoefs[9] = { 1.0 / 9, 1.0 / 9 , 1.0 / 9, 1.0 / 9, 1.0 / 9 , 1.0 / 9, 1.0 / 9, 1.0 / 9 , 1.0 / 9 };
	convolve2D(input, xSize, ySize, filterCoefs, 3);
}

void performVFFilter (uchar input[], int xSize, int ySize)
{
	//TO DO
	double filterCoefs[9] = { 0, -1.0 / 4 , 0, -1.0 / 4, 1 , -1.0 / 4, 0, -1.0 / 4 , 0 };
	convolve2D(input, xSize, ySize, filterCoefs, 3);
}

void performSuccessiveNFFilter (uchar input[], int xSize, int ySize, int stages)
{
	//TO DO
	double filterCoefs[9] = { 1.0 / 9, 1.0 / 9 , 1.0 / 9, 1.0 / 9, 1.0 / 9 , 1.0 / 9, 1.0 / 9, 1.0 / 9 , 1.0 / 9 };
	for (int i = 0; i < stages; i++)
	{
		convolve2D(input, xSize, ySize, filterCoefs, 3);
	}
}
