#include "ImageHistogram.h"
#include "qcustomplot.h"


/*******************************************************************************************************************************/
/* Simple contrast improvement */
/*******************************************************************************************************************************/
void simpleContrastImprovement (const uchar input[], int xSize, int ySize, uchar output[])
{
	/* TODO */
	uchar minPixel = 255;
	uchar maxPixel = 0;

	for (int i = 0; i < xSize; i++)
	{
		for (int j = 0; j < ySize; j++)
		{
			maxPixel = std::max(maxPixel, input[j * xSize + i]);
			minPixel = std::min(minPixel, input[j * xSize + i]);
		}
	}

	for (int i = 0; i < xSize; i++)
	{
		for (int j = 0; j < ySize; j++)
		{
			output[j * xSize + i] = (input[j * xSize + i] - minPixel) * 255 / (maxPixel - minPixel);
		}
	}

}


/*******************************************************************************************************************************/
/* Calculate image histogram */
/*******************************************************************************************************************************/
void calculateHistogram(const uchar input[], int xSize, int ySize, int histogram[])
{ //Y-buff->xSize->ySize->histogram_buff
	/* TODO */
	for (int i = 0; i < xSize; i++)
	{
		for (int j = 0; j < ySize; j++)
		{
			histogram[input[j * xSize + i]]++;
		}
	}
}


/*******************************************************************************************************************************/
/* Equalize image histogram */
/*******************************************************************************************************************************/
void equalizeHistogram(const uchar input[], int xSize, int ySize, uchar output[], uchar L)
{
	/* TODO */
	int histogram[256] = { 0 };
	calculateHistogram(input, xSize, ySize, histogram);

	int cdf[256] = { 0 }; //"kumulativna funkcija raspodele"
	int sum = 0;

	for (int i = 0; i < 256; i++)
	{
		sum += histogram[i];
		cdf[i] = sum;
	}

	int cdfMin = 255;
	for (int i = 0; i < 256; i++)
	{
		if (cdf[i] > 0) {
			cdfMin = cdf[i];
			break;
		}
	}
	//prolazak kroz celu sliku  i racunanje vrednosti svakog piksela po 
	//formuli iz pdf-a (koristi se vrednost piksela i vr. f-je raspodele
	for (int i = 0; i < xSize * ySize; i++)
	{
		uchar p = input[i];
		output[i] = round((L * (cdf[p] - cdfMin) / ((xSize * ySize) - cdfMin)));
	}

}

/*******************************************************************************************************************************/
/* Modify colour saturation */
/*******************************************************************************************************************************/
void modifySaturation(const uchar inputRGB[], const uchar inputY[], int xSize, int ySize, uchar outputRGB[], double S)
{
	/* TODO */
	double R, G, B;
	for (int i = 0; i < xSize * ySize; i++)
	{
		R = inputRGB[3 * i] * S + inputY[i] * (1 - S);
		G = inputRGB[3 * i + 1] * S + inputY[i] * (1 - S);
		B = inputRGB[3 * i + 2] * S + inputY[i] * (1 - S);

		if (R > 255)
			R = 255;
		else if (R < 0)
			R = 0;

		if (G > 255)
			G = 255;
		else if (G < 0)
			G = 0;

		if (B > 255)
			B = 255;
		else if (B < 0)
			B = 0;

		outputRGB[3 * i] = R;
		outputRGB[3 * i + 1] = G;
		outputRGB[3 * i + 2] = B;
		

	}


}

/*******************************************************************************************************************************/
/* Plot image histogram */
/* Returns QImage containing graph with histogram data. */
/* start and end parameters define first and last value for X axis */
/*******************************************************************************************************************************/
QImage createHistogramPlot(int histogram[], int start, int end)
{
     QCustomPlot* customPlot = new QCustomPlot();
	 
	 int vectorSize = end-start + 1;
	 int maxValue = 0;
	 QVector<double> x(vectorSize), y(vectorSize);
	 for (int i=0; i<vectorSize; i++)
	 {
	   x[i] = i + start;  
	   y[i] = histogram[i];
	   maxValue = maxValue>=histogram[i] ? maxValue : histogram[i];
	 }
	 
	 // create graph and assign data to it:
     customPlot->addGraph();
     customPlot->graph(0)->setData(x, y);
	 // give the axes some labels:
     customPlot->xAxis->setLabel("x");
     customPlot->yAxis->setLabel("y");
	 // set axes ranges, so we see all data:
     customPlot->xAxis->setRange(start, end);
     customPlot->yAxis->setRange(0, maxValue);
     QImage outputImage = customPlot->toPixmap().toImage();
     
	 delete customPlot;
     return outputImage;
}


