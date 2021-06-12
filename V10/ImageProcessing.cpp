
#include "ImageProcessing.h"
#include "PredictSVM.h"
#include "BoW.h"
#include "svm.h"
#include <QDebug>
#include <QPainter>

static bool modelLoaded = false;
static svm_model* svmModel;
static BagOfWordsModel  bowModel;
static const std::string pathBoWModelFile = "precalculated_models\\model.bow";
static const std::string pathSVMModelFile = "precalculated_models\\vezba12.model";

void imageProcessingFun( const QString& progName, QImage& outImgs, const QImage& inImgs, const QVector<double>& params)
{
	if (!modelLoaded)
	{
		if (!loadBoWModel(bowModel, pathBoWModelFile))
		{
			return;
		}

		svmModel = svm_load_model(pathSVMModelFile.c_str());
		if (svmModel == NULL)
		{
			return;
		}
		modelLoaded = true;

	}
	// Scale image dimensions to be suitable for 8x8 block division
	if (inImgs.width() % 8 != 0 || inImgs.height() % 8 != 0)
		outImgs = inImgs.scaled(inImgs.width() + (8 - inImgs.width() % 8), inImgs.height() + (8 - inImgs.height() % 8));
	else 
		outImgs = QImage(inImgs);

	if (progName == "SVM Classification")
	{
		int predicted_label;
		/////////////////////////////////////////////
		//TO DO: Predict image class for outImage.

		predicted_label = predictSingleImage(outImgs.bits(), outImgs.width(), outImgs.height(), bowModel, svmModel );

		//////////////////////////////////////////////	
		
		//Paint image label
		QPainter p(&outImgs);
		p.setPen(QPen(Qt::red));
		p.setFont(QFont("Times", 16, QFont::Bold));
		if(predicted_label == 1)
			p.drawText(outImgs.rect(), Qt::AlignCenter, "Wild Cat!");
		else
			p.drawText(outImgs.rect(), Qt::AlignCenter, "Not Wild Cat!");
	}
}

