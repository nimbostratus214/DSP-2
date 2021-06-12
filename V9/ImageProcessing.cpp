
#include "ImageProcessing.h"
#include "kMeans.h" 
#include <QDebug>

void imageProcessingFun(const QString& progName, QImage& outImgs, const QImage& inImgs, const QVector<double>& params)
{
	int X_SIZE = inImgs.width();
	int Y_SIZE = inImgs.height();

	outImgs = QImage(inImgs);
	
	if (progName == "Intensity kMeans")
	{
		IntensityBasedKMeans(outImgs.bits(), X_SIZE, Y_SIZE, round(params[0]));
	}
	else if (progName == "Color kMeans")
	{
		ColorBasedKMeans(outImgs.bits(), X_SIZE, Y_SIZE, round(params[0]));
	}
	else if (progName == "Intensity+Position kMeans")
	{
		IntensityPlusPositionBasedKMeans(outImgs.bits(), X_SIZE, Y_SIZE, round(params[0]));
	}
	else if (progName == "Histogram kMeans")
	{
		histogramBasedKMeans(outImgs.bits(), X_SIZE, Y_SIZE, round(params[1]), round(params[0]));
	}
	else if (progName == "SIFT kMeans")
	{
		SIFTBasedKMeans(outImgs.bits(), X_SIZE, Y_SIZE, round(params[1]), round(params[0]));
	}

}

