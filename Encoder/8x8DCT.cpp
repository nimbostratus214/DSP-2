#pragma once
#define _USE_MATH_DEFINES
#include <math.h>
#include "8x8DCT.h"

#define PI 3.14159265359

void GenerateDCTmatrix(double* DCTKernel, int order)
{
	int i, j;
	int N;
	N = order; //8
	double alpha;
	double denominator;
	for (j = 0; j <= N - 1; j++)
	{
		DCTKernel[0, j] = sqrt(1 / (double)N);
	}
	alpha = sqrt(2 / (double)N);
	denominator = (double)2 * N;
	for (j = 0; j <= N - 1; j++)
		for (i = 1; i <= N - 1; i++)
		{
			DCTKernel[i * N + j] = alpha * cos(((2 * j + 1) * i * PI) / denominator);
		}
}


void DCT(const uchar input[], short output[], int N, double* DCTKernel)
{
	double* temp = new double[N * N];
	double* DCTCoefficients = new double[N * N];

	double sum;
	for (int i = 0; i <= N - 1; i++)
	{
		for (int j = 0; j <= N - 1; j++)
		{
			sum = 0;
			for (int k = 0; k <= N - 1; k++)
			{
				sum = sum + DCTKernel[i * N + k] * (input[k * N + j] - 128.0);
			}
			temp[i * N + j] = sum;
		}
	}

	for (int i = 0; i <= N - 1; i++)
	{
		for (int j = 0; j <= N - 1; j++)
		{
			sum = 0;
			for (int k = 0; k <= N - 1; k++)
			{
				sum = sum + temp[i * N + k] * DCTKernel[j * N + k];
			}
			DCTCoefficients[i * N + j] = sum;
		}
	}

	for (int i = 0; i < N * N; i++)
	{
		output[i] = floor(DCTCoefficients[i] + 0.5);
	}

	delete[] temp;
	delete[] DCTCoefficients;

	return;
}

void DCT2(const char input[], short output[], int N, double* DCTKernel)
{
	double* temp = new double[N * N];
	double* DCTCoefficients = new double[N * N];

	double sum;
	for (int i = 0; i <= N - 1; i++)
	{
		for (int j = 0; j <= N - 1; j++)
		{
			sum = 0;
			for (int k = 0; k <= N - 1; k++)
			{
				sum = sum + DCTKernel[i * N + k] * (input[k * N + j] - 128.0);
			}
			temp[i * N + j] = sum;
		}
	}

	for (int i = 0; i <= N - 1; i++)
	{
		for (int j = 0; j <= N - 1; j++)
		{
			sum = 0;
			for (int k = 0; k <= N - 1; k++)
			{
				sum = sum + temp[i * N + k] * DCTKernel[j * N + k];
			}
			DCTCoefficients[i * N + j] = sum;
		}
	}

	for (int i = 0; i < N * N; i++)
	{
		output[i] = floor(DCTCoefficients[i] + 0.5);
	}

	delete[] temp;
	delete[] DCTCoefficients;

	return;
}



void IDCT(const short input[], uchar output[], int N, double* DCTKernel)
{
	/* TO DO */
	double* temp = new double[N * N];
	double* DCTCoefficients = new double[N * N];

	double sum;
	for (int i = 0; i <= N - 1; i++)
	{
		for (int j = 0; j <= N - 1; j++)
		{
			sum = 0;
			for (int k = 0; k <= N - 1; k++)
			{
				sum = sum + DCTKernel[k * N + i] * (input[k * N + j]);
			}
			temp[i * N + j] = sum;
		}
	}

	for (int i = 0; i <= N - 1; i++)
	{
		for (int j = 0; j <= N - 1; j++)
		{
			sum = 0;
			for (int k = 0; k <= N - 1; k++)
			{
				sum = sum + temp[i * N + k] * DCTKernel[k * N + j];
			}
			DCTCoefficients[i * N + j] = sum;
		}
	}

	for (int i = 0; i < N * N; i++)
	{
		double x = floor(DCTCoefficients[i] + 0.5) + 128;
		output[i] = (x > 255) ? 255 : ((x < 0) ? 0 : x);
	}

	delete[] temp;
	delete[] DCTCoefficients;

	return;
}

void IDCT2(const short input[], char output[], int N, double* DCTKernel)
{
	/* TO DO */
	double* temp = new double[N * N];
	double* DCTCoefficients = new double[N * N];

	double sum;
	for (int i = 0; i <= N - 1; i++)
	{
		for (int j = 0; j <= N - 1; j++)
		{
			sum = 0;
			for (int k = 0; k <= N - 1; k++)
			{
				sum = sum + DCTKernel[k * N + i] * (input[k * N + j]);
			}
			temp[i * N + j] = sum;
		}
	}

	for (int i = 0; i <= N - 1; i++)
	{
		for (int j = 0; j <= N - 1; j++)
		{
			sum = 0;
			for (int k = 0; k <= N - 1; k++)
			{
				sum = sum + temp[i * N + k] * DCTKernel[k * N + j];
			}
			DCTCoefficients[i * N + j] = sum;
		}
	}

	for (int i = 0; i < N * N; i++)
	{
		double x = floor(DCTCoefficients[i] + 0.5) + 128;
		output[i] = (x > 255) ? 255 : ((x < 0) ? 0 : x);
	}

	delete[] temp;
	delete[] DCTCoefficients;

	return;
}


void extendBorders(uchar* input, int xSize, int ySize, int N, uchar** p_output, int* newXSize, int* newYSize)
{
	int deltaX = N - xSize % N;
	int deltaY = N - ySize % N;

	*newXSize = xSize + deltaX;
	*newYSize = ySize + deltaY;

	uchar* output = new uchar[(xSize + deltaX) * (ySize + deltaY)];

	for (int i = 0; i < ySize; i++)
	{
		memcpy(&output[i * (xSize + deltaX)], &input[i * (xSize)], xSize);
		if (deltaX != 0)
		{
			memset(&output[i * (xSize + deltaX) + xSize], output[i * (xSize + deltaX) + xSize - 1], deltaX);
		}
	}

	for (int i = 0; i < deltaY; i++)
	{
		memcpy(&output[(i + ySize) * (xSize + deltaX)], &output[(ySize) * (xSize + deltaX)], xSize + deltaX);
	}

	*p_output = output;
}

void extendBorders2(char* input, int xSize, int ySize, int N, char** p_output, int* newXSize, int* newYSize)
{
	int deltaX = N - xSize % N;
	int deltaY = N - ySize % N;

	*newXSize = xSize + deltaX;
	*newYSize = ySize + deltaY;

	char* output = new char[(xSize + deltaX) * (ySize + deltaY)];

	for (int i = 0; i < ySize; i++)
	{
		memcpy(&output[i * (xSize + deltaX)], &input[i * (xSize)], xSize);
		if (deltaX != 0)
		{
			memset(&output[i * (xSize + deltaX) + xSize], output[i * (xSize + deltaX) + xSize - 1], deltaX);
		}
	}

	for (int i = 0; i < deltaY; i++)
	{
		memcpy(&output[(i + ySize) * (xSize + deltaX)], &output[(ySize) * (xSize + deltaX)], xSize + deltaX);
	}

	*p_output = output;
}


void cropImage(uchar* input, int xSize, int ySize, uchar* output, int newXSize, int newYSize)
{
	for (int i = 0; i < newYSize; i++)
	{
		memcpy(&output[i * (newXSize)], &input[i * (xSize)], newXSize);
	}
}

void cropImage2(char* input, int xSize, int ySize, char* output, int newXSize, int newYSize)
{
	for (int i = 0; i < newYSize; i++)
	{
		memcpy(&output[i * (newXSize)], &input[i * (xSize)], newXSize);
	}
}





void doZigZag(short block[], int N, int B)
{
	int currNum = 0;
	int currDiagonalWidth = 1;
	int i;
	int row;
	int col;

	while (currDiagonalWidth < N)
	{
		for (i = 0; i < currDiagonalWidth; i++)
		{
			if (currDiagonalWidth % 2 == 1)
			{
				row = currDiagonalWidth - i - 1;
				col = i;
			}
			else
			{
				row = i;
				col = currDiagonalWidth - i - 1;
			}

			if (currNum > B)
				block[row * N + col] = 0;
			currNum++;
		}
		currDiagonalWidth++;
	}

	while (currDiagonalWidth > 0)
	{
		for (i = currDiagonalWidth; i > 0; i--)
		{
			if (currDiagonalWidth % 2 == 1)
			{
				row = N - currDiagonalWidth + i - 1;
				col = N - i;
			}
			else
			{
				row = N - i;
				col = N - currDiagonalWidth + i - 1;
			}
			if (currNum > B)
				block[row * N + col] = 0;
			currNum++;
		}
		currDiagonalWidth--;



	}

}

int rle(short input[], short output[], int N)
{
	//f-ja prima ulazni blok za kodovanje i generise izlazni (izlazni moze biti maksimalno 2 * N * N). 
	//parametar coefs ce da bude brojac koeficijenata koji su izgenerisani i upisani u output jer se to menja u zavisnosti od ponavljanja cifara
	//taj brojac cemo iskoristiti u BitStreamWriteru kako bi znali koje je velicine output kako bi sve koef. upisali u fajl
	// AAABBCCCAAA --- A 3 B 2 C 3 A 3--- 
	// input ima 64 koeficijenta
	//output moze imati 128 u najgorem slucaju 
	int k = 0;
	
	for (int i = 0; i < N*N; i++)
	{
		int count = 1;
		while (input[i] == input[i + 1])
		{
			count++; 
			i++;       //i se ispomera i krene od prvog koji je razlicit kasnije
		} 
		output[k] = input[i];
		output[k + 1] = count;
		k++;
		k++;
	}

	return k;
}
