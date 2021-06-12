
#include "ImageProcessing.h"
#include "ColorSpaces.h"

#include <QDebug>

void imageProcessingFun(const QString& progName, QImage& outImg, const QImage& inImg, const QVector<double>& params) 
{
	uchar* Y_buff;
	char* U_buff;
	char* V_buff;

	outImg = QImage(inImg.width(), inImg.height(), inImg.format());
	
	if(progName == "YUV444") 
	{	
		Y_buff = new uchar[inImg.width()*inImg.height()];
		U_buff = new char[inImg.width()*inImg.height()];
		V_buff = new char[inImg.width()*inImg.height()];

		RGBtoYUV444(inImg.bits(), inImg.width(), inImg.height(), Y_buff, U_buff, V_buff);
		
		decimate_Y(Y_buff, inImg.width(), inImg.height());

		procesing_YUV444(Y_buff, U_buff, V_buff, inImg.width(), inImg.height(), params[0], params[1], params[2]);
		YUV444toRGB(Y_buff, U_buff, V_buff, inImg.width(), inImg.height(), outImg.bits());

		delete[] Y_buff;
		delete[] U_buff;
		delete[] V_buff;

	}
	else if (progName == "YUV422") 
	{
		Y_buff = new uchar[inImg.width()*inImg.height()];
		U_buff = new char[inImg.width()*inImg.height()/2];
		V_buff = new char[inImg.width()*inImg.height()/2];

		RGBtoYUV422(inImg.bits(), inImg.width(), inImg.height(), Y_buff, U_buff, V_buff);
		procesing_YUV422(Y_buff, U_buff, V_buff, inImg.width(), inImg.height(), params[0], params[1], params[2]);
		YUV422toRGB(Y_buff, U_buff, V_buff, inImg.width(), inImg.height(), outImg.bits());

		delete[] Y_buff;
		delete[] U_buff;
		delete[] V_buff;
	}
	else if (progName == "YUV420") 
	{
		Y_buff = new uchar[inImg.width()*inImg.height()];
		U_buff = new char[inImg.width()*inImg.height()/4];
		V_buff = new char[inImg.width()*inImg.height()/4];

		RGBtoYUV420(inImg.bits(), inImg.width(), inImg.height(), Y_buff, U_buff, V_buff);
		procesing_YUV420(Y_buff, U_buff, V_buff, inImg.width(), inImg.height(), params[0], params[1], params[2]);
		YUV420toRGB(Y_buff, U_buff, V_buff, inImg.width(), inImg.height(), outImg.bits());

		delete[] Y_buff;
		delete[] U_buff;
		delete[] V_buff;
	}
	else if (progName == "RGB")
	{
		processing_RGB(inImg.bits(), inImg.width(), inImg.height(), outImg.bits(), params[0], params[1], params[2]);
	}

}

