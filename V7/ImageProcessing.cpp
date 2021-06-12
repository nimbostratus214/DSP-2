
#include "ImageProcessing.h"
#include "ImageInterpolation.h"
#include <QDebug>

void imageProcessingFun(const QString& progName, QImage& outImgs, const QImage& inImgs, const QVector<double>& params)
{
	int X_SIZE = inImgs.width();
	int Y_SIZE = inImgs.height();


	if (progName == "Sample and hold")
	{
		int newXSize = ceil(X_SIZE * params[1]);
		int newYSize = ceil(Y_SIZE * params[0]);

		/* Calculate output image resolution and construct output image object */
		outImgs = QImage(newXSize, newYSize, inImgs.format());

		/* Perform Sample and hold interpolation  */
		sampleAndHold(inImgs.bits(), X_SIZE, Y_SIZE, outImgs.bits(), newXSize, newYSize);

	}
	else if (progName == "Bilinear")
	{
		int newXSize = ceil(X_SIZE * params[1]);
		int newYSize = ceil(Y_SIZE * params[0]);

		/* Calculate output image resolution and construct output image object */
		outImgs = QImage(newXSize, newYSize, inImgs.format());

		/* Perform Bilinear interpolation  */
		bilinearInterpolate(inImgs.bits(), X_SIZE, Y_SIZE, outImgs.bits(), newXSize, newYSize);
	}
	else if(progName == "Rotation") 
	{	
		outImgs = QImage(X_SIZE, Y_SIZE, inImgs.format());

		/* TO DO: Perform image rotation */
		imageRotate(inImgs.bits(), X_SIZE, Y_SIZE, outImgs.bits(), X_SIZE/2, Y_SIZE/2, params[0]);
	
	}
	else if (progName == "Rotation Bilinear") 
	{
		outImgs = QImage(X_SIZE, Y_SIZE, inImgs.format());

		/* TO DO: Perform image rotation with bilinear interpolation */
		imageRotateBilinear(inImgs.bits(), X_SIZE, Y_SIZE, outImgs.bits(), X_SIZE / 2, Y_SIZE / 2, params[0]);
	}

}

