#include "FeatureDetection.h"
#include "SIFTLib.h"
#include "ColorSpaces.h"
#include "ImageFilter.h"
#include "FeatureDraw.h"
#include "NoiseReduction.h"
#include <list>

using namespace std;


SiftKeypointList GetSIFTFeatures(const uchar input[], int xSize, int ySize)
{
	SiftKeypointList kptList;

	uchar* Y_buff = new uchar[xSize*ySize];
	char* U_buff = new char[xSize*ySize / 4];
	char* V_buff = new char[xSize*ySize / 4];

	/* Convert input image to YUV420 image */
	RGBtoYUV420(input, xSize, ySize, Y_buff, U_buff, V_buff);

	delete[] U_buff;
	delete[] V_buff;

	performGaussFilter(Y_buff, xSize, ySize, 5, 0.1);

	/* Perform SIFT transformation  */
	kptList = calculateSIFT(Y_buff, xSize, ySize);

	delete[] Y_buff;

	return kptList;
}

