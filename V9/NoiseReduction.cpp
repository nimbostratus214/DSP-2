#include "NoiseReduction.h"
#include "ImageFilter.h"

#include <vector>
#include <algorithm>


void performMovingAverage (uchar input[], int xSize, int ySize, int N)
{
	//TO DO
	double* coeffs = new double[N*N];
	for (int i = 0; i < N*N; i++)
	{
		coeffs[i] = 1.0 / (N*N);
	}
	convolve2D(input, xSize, ySize, coeffs, N);
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
	double* coeffs = new double[N*N];
	calculateGaussKernel(coeffs, N, sigma);

	convolve2D(input, xSize, ySize, coeffs, N);
}

void performMedianFilter (uchar input[], int xSize, int ySize, int N)
{
	//TO DO
	std::vector<uchar> temp(N*N);
	uchar* input_copy = new uchar[xSize*ySize];
	for (int i = 0; i < xSize*ySize; i++)
	{
		input_copy[i] = input[i];
	}


	for (int i = 0; i < xSize; i++)
	{
		for (int j = 0; j < ySize; j++)
		{
			int k = 0;
			for (int m = i - N / 2; m <= i + N / 2; m++)
			{
				for (int n = j - N / 2; n <= j + N / 2; n++)
				{
					if (m >= 0 && m < xSize && n >= 0 && n < ySize)
					{
						temp[k] = input_copy[m + n*xSize];
					}
					else
					{
						temp[k] = 0;
					}
					k++;
				}

				std::sort(temp.begin(), temp.end());
				input[i + j*xSize] = temp[N*N / 2];
			}

		}
	}
	delete[] input_copy;

}
