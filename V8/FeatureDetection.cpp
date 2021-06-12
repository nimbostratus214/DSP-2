#include "FeatureDetection.h"
#include "SIFTLib.h"
#include "ColorSpaces.h"
#include "FeatureDraw.h"

#include <list>

using namespace std;

/*******************************************************************************************************************************/
/* Find SIFT keypoints and mark each one with red color*/
/*******************************************************************************************************************************/
void SIFTDetect(uchar input[], int xSize, int ySize)
{
	SiftKeypointList kptList;

	uchar* Y_buff = new uchar[xSize*ySize];
	char* U_buff = new char[xSize*ySize / 4];
	char* V_buff = new char[xSize*ySize / 4];
	/* Convert input image to YUV420 image */
	RGBtoYUV420(input, xSize, ySize, Y_buff, U_buff, V_buff);


	/* Perform SIFT transformation  */
	/* TO DO*/

	SiftKeypointList kp = calculateSIFT(Y_buff, xSize, ySize);

	/* Mark all keypoints in input image */
	/* TO DO*/
	markSIFTKeypointsRGB(input, xSize, ySize, kp, 255, 0, 0);

	delete[] Y_buff;
	delete[] U_buff;
	delete[] V_buff;
}

/*******************************************************************************************************************************/
/* Helper function. Splits image feature set in half (used for "two image" input)
/*******************************************************************************************************************************/
void splitFeatures(const SiftKeypointList& in, int width, SiftKeypointList& leftImageKP, SiftKeypointList& rightImageKP)
{
	for (SiftKeypoint kp : in)
	{
		if (kp.c < width / 2)
			leftImageKP.push_back(kp);
		else
			rightImageKP.push_back(kp);
	}
}

/*******************************************************************************************************************************/
/* Calculate Euclidean between two descriptor vectors
/*******************************************************************************************************************************/
double l2Distance(SiftKeypoint kp1, SiftKeypoint kp2)
{
	double suma = 0;
	for (int i = 0; i < DEGREE_OF_DESCRIPTORS; i++)
	{
		suma += (kp2.descriptors[i] - kp1.descriptors[i]) * (kp2.descriptors[i] - kp1.descriptors[i]);
	}
	

	suma = sqrt(suma);
	/* TO DO */
	return suma;
}

/*******************************************************************************************************************************/
/* Go through the first set of keypoints, and for each keypoint try to find the match in the second set
/* One feature matches the other if Euclidean distance between them is lower than threshold.
/*******************************************************************************************************************************/
void matchFeatures(SiftKeypointList leftImageKP, SiftKeypointList rightImageKP, list<pair<QPoint, QPoint>>& matchPairs, double threshold)
{
	for (SiftKeypoint kp1 : leftImageKP)
	{
		for (SiftKeypoint kp2 : rightImageKP)
		{
			if (l2Distance(kp1, kp2) < threshold)
			{
				matchPairs.push_back({ QPoint(kp1.r, kp1.c), QPoint(kp2.r, kp2.c) });
			}
		}
	}
	
}

/*******************************************************************************************************************************/
/* Find SIFT keypoints, split the image in half, match the features from one image with those from another
/* one and connect them with green lines
/*******************************************************************************************************************************/
void SIFTDetectPlusMatch(uchar input[], int xSize, int ySize, double threshold)
{
	SiftKeypointList kptList, kptListLeft, kptListRight;

	uchar* Y_buff = new uchar[xSize*ySize];
	char* U_buff = new char[xSize*ySize / 4];
	char* V_buff = new char[xSize*ySize / 4];

	/* Convert input image to YUV420 image */
	RGBtoYUV420(input, xSize, ySize, Y_buff, U_buff, V_buff);

	/* Perform SIFT transformation  */
	kptList = calculateSIFT(Y_buff, xSize, ySize);

	/* Split the features of left and right images in separate lists */
	splitFeatures(kptList, xSize, kptListLeft, kptListRight);

	/* Match the features from two images */
	list<pair<QPoint, QPoint>> matchedDots;
	matchFeatures(kptListLeft, kptListRight, matchedDots, threshold);

	/* Draw a line for each mached feature pair */
	/* TO DO */
	drawMatchedFeaturesLinesRGB(input, xSize, ySize, matchedDots, 255, 0, 255);

	

	delete[] Y_buff;
	delete[] U_buff;
	delete[] V_buff;

}

/*******************************************************************************************************************************/
/* Harris algorithm for corner detection
/*******************************************************************************************************************************/
void HarisCornerDetection(uchar input[], int xSize, int ySize, double threshold, double alpha)
{
	double* sobelVertical = new double[xSize*ySize];
	double* sobelHorizontal = new double[xSize*ySize];
	uchar* Y_buff = new uchar[xSize*ySize];
	char* U_buff = new char[xSize*ySize / 4];
	char* V_buff = new char[xSize*ySize / 4];

	RGBtoYUV420(input, xSize, ySize, Y_buff, U_buff, V_buff);

	
	int N = 3;
	int delta = (N - 1) / 2;

	double hCoeff[] = { 0.25, 0.5, 0.25, 0, 0, 0,-0.25, -0.5, -0.25 };
	double vCoeff[] = { 0.25, 0, -0.25, 0.5, 0, -0.5, 0.25, 0, -0.25 };

	int newXSize = xSize + 2;
	int newYSize = ySize + 2;

	uchar* R_matrix = new uchar[xSize * ySize];

	uchar* extendedImage = new uchar[newXSize * newYSize];
	//EXTEND BORDERS
	int newXSize1 = xSize + 2 * delta;
	int newYSize1 = ySize + 2 * delta;

	for (int i = 0; i < xSize; i++)
	{
		for (int j = 0; j < ySize; j++)
		{
			extendedImage[(j + delta) * newXSize1 + i + delta] = Y_buff[j * xSize + i]; //nije input, detekcija se radi nad Y_buff
		}
	}

	for (int i = 0; i < delta; i++)
	{
		memcpy(&extendedImage[i * newXSize1 + delta], &Y_buff[0], xSize);
		memcpy(&extendedImage[(ySize + delta + i) * newXSize1 + delta], &Y_buff[(ySize - 1) * xSize], xSize);
	}

	for (int i = 0; i < newYSize; i++)
	{
		memset(&extendedImage[i * newXSize1], extendedImage[i * newXSize1 + delta], delta);
		memset(&extendedImage[i * newXSize1 + delta + xSize], extendedImage[i * newXSize1 + xSize + delta - 1], delta);
	}
	//KRAJ EXTENDA
	double* horizontalniGrad = new double[xSize * ySize];
	double* vertikalniGrad = new double[xSize * ySize];

	for (int i = 0; i < xSize; i++)
	{
		for (int j = 0; j < ySize; j++)
		{
			double Gh = 0, Gv = 0;

			for (int m = 0; m < N; m++)
			{
				for (int n = 0; n < N; n++)
				{
					Gh += extendedImage[(j + n) * newXSize + i + m] * hCoeff[(N - n) * N - m - 1];
					Gv += extendedImage[(j + n) * newXSize + i + m] * vCoeff[(N - n) * N - m - 1];
				}
			}
			
			horizontalniGrad[j * xSize + i] = Gh;
			vertikalniGrad[j * xSize + i] = Gv;
			
		}
	}

	for (int i = 0; i < xSize; i++) {
		for (int j = 0; j < ySize; j++) {
			//posmatramo blok NxN oko piksela i racunamo tezinsku matricu M

			double Ix = 0, Iy = 0, IxIy = 0;

			for (int m = 0; m < N; m++)
			{
				for (int n = 0; n < N; n++)
				{
					Ix += horizontalniGrad[(j + n) * ySize + i + m] * horizontalniGrad[(j + n) * ySize + i + m];
					Iy += vertikalniGrad[(j + n) * ySize + i + m] * vertikalniGrad[(j + n) * ySize + i + m];
					IxIy += vertikalniGrad[(j + n) * ySize + i + m] * horizontalniGrad[(j + n) * ySize + i + m];

				}
			}

			double det = 0;
			det = Ix * Iy - IxIy * IxIy;

			double R = 0;
			double alfa = alpha;//0.04;
			double trace = Ix + Iy;

			R = det - alfa * trace * trace;

			if (R > 255)
			{
				R = 255;
			}
			else if (R < 0)
			{
				R = 0;
			}

			R_matrix[j * xSize + i] = R;


		}
	}
	

	/* Find min and max of matrix R and scale entire matrix to [0, 255] */
	/* TO DO: */

	
	uchar Rmin = R_matrix[0];
	uchar Rmax = R_matrix[0];
	for (int i = 0; i < xSize * ySize; i++)
	{
		if (R_matrix[i] > Rmax)
		{
			Rmax = R_matrix[i];
		}
		if (R_matrix[i] < Rmin)
		{
			Rmin = R_matrix[i];
		}
		
	}



	for (int i = 0; i < xSize * ySize; i++)
	{
			R_matrix[i] = (R_matrix[i] - Rmin) * 255 / (Rmax - Rmin);
	}

	/* For each entry in R matrix, if the value is greater then threshold, check the 3x3 block around the pixel and if it is local maximum, draw a red circle in input image with radius 3 */
	
	for (int i = 0; i < xSize ; i++)
	{
		for (int j = 0; j < ySize ; j++)
		{

			if (R_matrix[j*xSize + i] > threshold) //treba da prelazi ovu granicu! bilo je "<"
			{

				uchar pix1, pix2, pix3, pix4, pix6, pix7, pix8, pix9;
				uchar* pixels = new uchar[8];
				//prvi pixel
			
				if (extendedImage[(j - 1) * xSize + (i - 1)] < 0 && extendedImage[(j - 1) * xSize + (i - 1)] > 255)
				{
					pix1 = 0;
				}
				else
				{
					pix1 = extendedImage[(j - 1) * xSize + (i - 1)];
				}
				pixels[0] = pix1;

				//drugi pixel
				if (extendedImage[(j - 1) * xSize + i] < 0 && extendedImage[(j - 1) * xSize + i] > 255)
				{
					pix2 = 0;
				}
				else
				{
					pix2 = extendedImage[(j - 1) * xSize + i];
				}
				pixels[1] = pix2;
				
				//treci pixel
				if (extendedImage[(j - 1) * xSize + (i + 1)] < 0 && extendedImage[(j - 1) * xSize + (i + 1)] > 255)
				{
					pix3 = 0;
				}
				else
				{
					pix3 = extendedImage[(j - 1) * xSize + (i + 1)];
				}
				pixels[2] = pix3;
				
				//cetvrti pixel
				if (extendedImage[(j) * xSize + (i - 1)] < 0 && extendedImage[(j) * xSize + (i - 1)] > 255)
				{
					pix4 = 0;
				}
				else
				{
					pix4 = extendedImage[(j) * xSize + (i - 1)];
				}
				pixels[3] = pix4;
				
				//sesti pixel
				if (extendedImage[(j) * xSize + (i + 1)] < 0 && extendedImage[(j) * xSize + (i + 1)] > 255)
				{
					pix6 = 0;
				}
				else
				{
					pix6 = extendedImage[(j) * xSize + (i + 1)];
				}
				pixels[4] = pix6;
				
				//sedmi pixel
				if (extendedImage[(j + 1) * xSize + (i - 1)] < 0 && extendedImage[(j + 1) * xSize + (i - 1)] > 255)
				{
					pix7 = 0;
				}
				else
				{
					pix7 = extendedImage[(j + 1) * xSize + (i - 1)];
				}
				pixels[5] = pix7;

				//osmi pixel
				if (extendedImage[(j + 1) * xSize + (i)] < 0 && extendedImage[(j + 1) * xSize + (i)] > 255)
				{
					pix8 = 0;
				}
				else
				{
					pix8 = extendedImage[(j + 1) * xSize + (i)];
				}
				pixels[6] = pix8;

				//devet pixel
				if (extendedImage[(j + 1) * xSize + (i+1)] < 0 && extendedImage[(j + 1) * xSize + (i+1)] > 255)
				{
					pix9 = 0;
				}
				else
				{
					pix9 = extendedImage[(j + 1) * xSize + (i+1)];
				}
				pixels[7] = pix9;

				bool radi = true;

				for (int k = 0; k < 8; k++)
				{
					if (pixels[k] > input[j * xSize + i])
					{
						radi = false;
					}
				}
			
			
				if (radi)
				{
					drawCircleRGB(input, j, i, 3, xSize, ySize, 0, 255, 0);
				}
			
			}
			
		}
	}

	delete[] sobelVertical;
	delete[] sobelHorizontal;
	delete[] Y_buff;
	delete[] U_buff;
	delete[] V_buff;

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