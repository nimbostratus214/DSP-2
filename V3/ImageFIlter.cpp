/*AUTOR: Radenko Mihajlovic RA214/18  Datum: 20.03.2021.*/

#include "ImageFilter.h"
#include <cmath>
#include <fstream>
#include <math.h>

static const double PI = 3.14159265358979323846;
static const double PI_8 = PI / 8;


void convolve2D(uchar image[], int xSize, int ySize, double* filterCoeff, int N)
{
	int newXSize = xSize + (N - 1);
	int newYSize = ySize + (N - 1);

	uchar* extendedImage = new uchar[newXSize * newYSize];

	extendBorders(image, xSize, ySize, extendedImage, (N - 1) / 2);

	for (int i = 0; i < xSize; i++)
	{
		for (int j = 0; j < ySize; j++)
		{
			double accum = 0;
			for (int m = 0; m < N; m++)
			{
				for (int n = 0; n < N; n++)
				{

					accum += extendedImage[(j + n) * newXSize + i + m] * filterCoeff[(N - n) * N - m - 1];
				}
			}
			if (accum > 255.0)
				image[j * xSize + i] = 255;
			else if (accum < 0.0)
				image[j * xSize + i] = 0;
			else
				image[j * xSize + i] = floor(accum + 0.5);

		}
	}

	delete[] extendedImage;
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
		memcpy(&output[(ySize + delta + i) * newXSize + delta], &input[(ySize - 1) * xSize], xSize);
	}

	for (int i = 0; i < newYSize; i++)
	{
		memset(&output[i * newXSize], output[i * newXSize + delta], delta);
		memset(&output[i * newXSize + delta + xSize], output[i * newXSize + xSize + delta - 1], delta);
	}

}

void performSuccessiveNFFilter(uchar input[], int xSize, int ySize, int stages)
{
	double* coeff = new double[9];
	for (int i = 0; i < 9; i++)
	{
		coeff[i] = 1.0 / 9.0;
	}

	for (int i = 0; i < stages; i++)
	{
		convolve2D(input, xSize, ySize, coeff, 3);
	}
	delete[] coeff;
}

void performSobelEdgeDetection(uchar input[], int xSize, int ySize, double threshold)
{
	int N = 3;
	int delta = (N - 1) / 2;

	//Declare Sobel coeffs for horizontal and vertical edges
	double hCoeff[] = { 0.25, 0.5, 0.25, 0, 0, 0,-0.25, -0.5, -0.25 };
	double vCoeff[] = { 0.25, 0, -0.25, 0.5, 0, -0.5, 0.25, 0, -0.25 };

	int newXSize = xSize + 2;
	int newYSize = ySize + 2;

	// Extend image
	// TO DO
	uchar* extendedImage = new uchar[newXSize*newYSize];
	extendBorders(input, xSize, ySize, extendedImage, delta);

	// Perform edge detection
	for (int i = 0; i < xSize; i++)
	{
		for (int j = 0; j < ySize; j++)
		{
			// Apply Sobel horizontal and vertical filter
			double hAccum = 0, vAccum = 0;
			
			// TO DO

			for (int n = 0; n < N; n++) {
				for (int m = 0; m < N; m++) {
					hAccum += extendedImage[(j + n) * newXSize + i + m] * hCoeff[(N - n) * N - m - 1];
					vAccum += extendedImage[(j + n) * newXSize + i + m] * vCoeff[(N - n) * N - m - 1];
				}
			}

			// Calculate global gradient and apply threshold
			
			// TO DO
			double gradient = sqrt(hAccum * hAccum + vAccum * vAccum);
			if (gradient < threshold) {
				input[j*xSize + i] = 0;
			}
			else {
				input[j*xSize + i] = 255;
			}

		}
	}

	// Free dynamicali allocated memory
	// TO DO
	delete[] extendedImage;

}

void performNFplusSobelEdgeDetection(uchar input[], int xSize, int ySize, int stages, double threshold)
{
	// TO DO
	performSuccessiveNFFilter(input, xSize, ySize, stages);
	performSobelEdgeDetection(input, xSize, ySize, threshold);
}

static void nonMaxSupression(double edgeMagnitude[], uchar edgeDirection[], int xSize, int ySize, double out[])
{
	for (int i = 0; i < xSize; i++)
	{
		for (int j = 0; j < ySize; j++)
		{
			double mag1 = 0;
			double mag2 = 0;
			switch (edgeDirection[j * xSize + i])
			{
			case 0:
				if (j > 0)
					mag1 = edgeMagnitude[(j - 1) * xSize + i];
				if (j < ySize - 1)
					mag2 = edgeMagnitude[(j + 1) * xSize + i];
				break;
			case 45:
				if (j > 0 && i > 0)
					mag1 = edgeMagnitude[(j - 1) * xSize + i - 1];
				if (j < ySize - 1 && i < xSize - 1)
					mag2 = edgeMagnitude[(j + 1) * xSize + i + 1];
				break;
			case 90:
				if (i > 0)
					mag1 = edgeMagnitude[(j)*xSize + i - 1];
				if (i < xSize - 1)
					mag2 = edgeMagnitude[(j)*xSize + i + 1];
				break;
			case 135:
				if (j > 0 && i < xSize - 1)
					mag1 = edgeMagnitude[(j - 1) * xSize + i + 1];
				if (j < ySize - 1 && i > 0)
					mag2 = edgeMagnitude[(j + 1) * xSize + i - 1];
				break;
			default:
				break;
			}

			double mag0 = edgeMagnitude[j * xSize + i];

			if (mag0 >= mag1 && mag0 >= mag2)
				out[j * xSize + i] = mag0;
			else
				out[j * xSize + i] = 0;
		}
	}

}

void performCannyEdgeDetection(uchar input[], int xSize, int ySize, double threshold1, double threshold2)
{
	int N = 3;
	int delta = (N - 1) / 2;
	double hCoeff[] = { 0.25, 0.5, 0.25, 0, 0, 0,-0.25, -0.5, -0.25 }; 
	double vCoeff[] = { 0.25, 0, -0.25, 0.5, 0, -0.5, 0.25, 0, -0.25 };

	int newXSize = xSize + 2 * delta;
	int newYSize = ySize + 2 * delta;

	uchar* extendedImage = new uchar[newXSize * newYSize];

	double* edgeMagnitude = new double[xSize * ySize];
	uchar* edgeDirection = new uchar[xSize * ySize];
	double* nonMaxSup = new double[xSize * ySize];


	// Extend borders
	// TO DO
	extendBorders(input, xSize, ySize, extendedImage, delta);
	

	for (int i = 0; i < xSize; i++)
	{
		for (int j = 0; j < ySize; j++)
		{
			// Apply Sobel horizontal and vertical filter
			double hAccum = 0, vAccum = 0;

			// TO DO
			for (int n = 0; n < N; n++) {
				for (int m = 0; m < N; m++) {
					hAccum += extendedImage[(j + n) * newXSize + i + m] * hCoeff[(N - n) * N - m - 1];
					vAccum += extendedImage[(j + n) * newXSize + i + m] * vCoeff[(N - n) * N - m - 1];
				}
			}

			// Calculate global gradient

			// TO DO
			edgeMagnitude[j * xSize + i] = sqrt(hAccum * hAccum + vAccum * vAccum);

			// Calculate edge angle

			// TO DO
			double edgeAngle = atan2(vAccum, hAccum);
			if (edgeAngle < 0)
				edgeAngle += PI;

			if ((edgeAngle < PI_8) || (edgeAngle > 7 * PI_8))
				edgeDirection[j * xSize + i] = 0;
			else if (edgeAngle < 3*PI_8)
				edgeDirection[j * xSize + i] = 45;
			else if (edgeAngle < 5*PI_8)
				edgeDirection[j * xSize + i] = 90;
			else 
				edgeDirection[j * xSize + i] = 135;
		}
	}

	nonMaxSupression(edgeMagnitude, edgeDirection, xSize, ySize, nonMaxSup);

	/* Apply threshold */
	for (int i = 0; i < xSize; i++)
	{
		for (int j = 0; j < ySize; j++)
		{

			if (nonMaxSup[j * xSize + i] < threshold1)  //nije ivica
			{
				// TO DO
				input[j * xSize + i] = 0;
			}
			else if (nonMaxSup[j * xSize + i] > threshold2)  //jeste tvrda ivica
			{
				// TO DO
				input[j * xSize + i] = 255;
			}
			else               //jeste ivica ako makar jedna od dve susedne tacke koje se nalaze u pravcu ivice jeste cvrsta ivica 
			{
				input[j * xSize + i] = 0;

				switch (edgeDirection[j * xSize + i])
				{
				case 0:
					// TO DO

					if ((i > 0) && (i < xSize - 1)) {
						if ((nonMaxSup[j * xSize + i + 1] > threshold2) || (nonMaxSup[j * xSize + i - 1] > threshold2)) {
							input[j * xSize + i] = 255; //jeste ivica
						}
						else {
							input[j * xSize + i] = 0;  //nije ivica
						}
					}
					break;
				case 45:
					//TO DO
					 
					/*if (((j > 0) && (i < xSize - 1)) || ((i > 0) && (j < ySize - 1))) {   //Ovo nije dobro, jer moze da se desi "gadjanje" u nedozvoljenu memoriju
						if ((nonMaxSup[(j-1) * xSize + i+1] > threshold2) || (nonMaxSup[(j+1) * xSize + i-1] > threshold2)) {
							input[j * xSize + i] = 255; //jeste ivica
						}
						else {
							input[j * xSize + i] = 0;  //nije ivica
						}  
					} */
					if (j > 0 && i < xSize - 1) {
						if (nonMaxSup[(j - 1) * xSize + i + 1] > threshold2) {
							input[j * xSize + i] = 255;
						}
						else {
							input[j * xSize + i] = 0;
						}
					}
					if (i > 0 && j < ySize - 1) {
						if (nonMaxSup[(j + 1) * xSize + i - 1] > threshold2) {
							input[j * xSize + i] = 255;
						}
						else {
							input[j * xSize + i] = 0;
						}
					}


					break;
				case 90:
					// TO DO

					if ((j > 0) && (j < ySize - 1)) {
						if ((nonMaxSup[(j-1) * xSize + i] > threshold2) || (nonMaxSup[(j+1) * xSize + i] > threshold2)) {
							input[j * xSize + i] = 255; //jeste ivica
						}
						else {
							input[j * xSize + i] = 0;  //nije ivica
						}
					}

					break;
				case 135:
					// TO DO
					/*
					if (((i > 0) && (j > 0)) || ((i < xSize - 1) && (j < ySize - 1))) {			//takodje moze da "gadja" u nedozvoljenu memoriju
						if ((nonMaxSup[(j - 1) * xSize + i - 1] > threshold2) || (nonMaxSup[(j + 1) * xSize + i + 1] > threshold2)) {
							input[j * xSize + i] = 255; //jeste ivica
						}
						else {
							input[j * xSize + i] = 0;  //nije ivica
						}
					}
					*/
					if (i > 0 && j > 0) {
						if (nonMaxSup[(j - 1) * xSize + i - 1] > threshold2) {
							input[j * xSize + i] = 255;
						}
						else {
							input[j * xSize + i] = 0;
						}
					}

					if ((i < xSize - 1) && (j < ySize - 1)) {
						if (nonMaxSup[(j + 1) * xSize + i + 1] > threshold2) {
							input[j * xSize + i] = 255;
						}
						else {
							input[j * xSize + i] = 0;
						}
					}

					break;

				default:
					break;
				}
			}
		}
	}
	
	delete[] extendedImage;
	delete[] edgeDirection;
	delete[] edgeMagnitude;
	delete[] nonMaxSup;
}

void performNFplusCannyEdgeDetection(uchar input[], int xSize, int ySize, int stages, double threshold1, double threshold2)
{
	// TO DO
	performSuccessiveNFFilter(input, xSize, ySize, stages);
	performCannyEdgeDetection(input, xSize, ySize, threshold1, threshold2);
}
