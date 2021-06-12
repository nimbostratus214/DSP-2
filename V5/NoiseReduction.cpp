/* AUTOR: Radenko Mihajlovic RA214/2018 */

#include "NoiseReduction.h"
#include "ImageFilter.h"

#include <vector>
#include <algorithm>


void performMovingAverage (uchar input[], int xSize, int ySize, int N)
{
	//TO DO
	double filter[25] = { 1.0/25 , 1.0/25 , 1.0 / 25 , 1.0 / 25 , 1.0 / 25,
						1.0 / 25 , 1.0 / 25 , 1.0 / 25 , 1.0 / 25 , 1.0 / 25 ,
						1.0 / 25 , 1.0 / 25 , 1.0 / 25 , 1.0 / 25 , 1.0 / 25 ,
						1.0 / 25 , 1.0 / 25 , 1.0 / 25 , 1.0 / 25 , 1.0 / 25 ,
						1.0 / 25 , 1.0 / 25 , 1.0 / 25 , 1.0 / 25 , 1.0 / 25
						};
	convolve2D(input, xSize, ySize, filter, N);
}

void calculateGaussKernel(double kernel[], int N, double sigma)
{
	double C = 0;
	for(int n = 0; n < N; n++)
    {
        for(int k = 0; k < N; k++)
        {
            kernel[n*N+k] = exp( -((n - N/2)*(n - N/2) + (k - N/2)*(k - N/2)) / (2 * sigma * sigma));
			C += kernel[n*N+k];
		}
	}

	C = 1.0 / C;

	for(int n = 0; n < N; n++)
    {
        for(int k = 0; k < N; k++)
        {
            kernel[n*N+k] *= C;
		}
	}
}

void performGaussFilter (uchar input[], int xSize, int ySize, int N, double sigma)
{
	//TO DO
	double kernel[25] = { 0 };

	calculateGaussKernel(kernel, N, sigma);

	convolve2D(input, xSize, ySize, kernel, N);
}

void performMedianFilter (uchar input[], int xSize, int ySize, int N)
{
	//TO DO

	uchar* extended = new uchar[(xSize + N - 1) * (ySize + N - 1)];
	extendBorders(input, xSize, ySize, extended, N / 2);

	for (int i = 0; i < xSize; i++) {
		for (int j = 0; j < ySize; j++) {
			int* med = new int[N * N];

			for (int k = 0; k < N; k++) {
				for (int m = 0; m < N; m++) {
					med[k + N * m] = extended[(i + k) + (j + m) * (xSize + N - 1)];
				}
			}

			for (int k = 0; k < N * N - 1; k++) {
				for (int m = k; m < N * N; m++) {
					if (med[k] > med[m]) {
						int t = med[k];
						med[k] = med[m];
						med[m] = t;
					}
				}
			}
			input[i + j * xSize] = med[N * N / 2];
			delete med;
		}
	}

	delete extended;
}
