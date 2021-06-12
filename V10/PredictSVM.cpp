#include "PredictSVM.h"
#include <iostream>
#include <fstream>
#include "BoW.h"
#include <QDebug>

using namespace std;

bool predictSVM(const ImgDataBase& dataBase, std::string BOWModelFileName, std::string SVMModelFileName)
{
	BagOfWordsModel bowModel;

	qInfo() << "Load Bag of Words model ...";
	if (loadBoWModel(bowModel, BOWModelFileName))
	{
		qInfo() << "DONE" << endl;
	}
	else
	{
		qCritical() << "ERROR: Unable to load BOW model (" << BOWModelFileName.c_str()<<")";
		return false;
	}

	qInfo() << "DONE" << endl;

	qInfo() << "Load SVM model ...";
	
	svm_model* svmModel = svm_load_model(SVMModelFileName.c_str());
	if(svmModel == NULL)
	{
		qCritical() << "ERROR: Unable to load SVM model (" << SVMModelFileName.c_str() << ")";
		return false;
	}
	
	qInfo() << "DONE" << endl;

	qInfo() << "Do prediction...";
	int totalHits = 0;
	int imageIndex = 0;
	for (const DBImage& img : dataBase)
	{
		int predictedLabel = predictSingleImage(img.image->bits(), img.image->width(), img.image->height(), bowModel, svmModel);

		if (predictedLabel == img.labelNumber)
		{
			totalHits++;
		}
		
		imageIndex++;
	}

	qInfo() << "DONE" << endl;

	qInfo() << "Prediction accuracy: " << totalHits*100.0 / dataBase.size() << "%";
	
	svm_free_and_destroy_model(&svmModel);
}


int predictSingleImage(uchar input[], int xSize, int ySize, BagOfWordsModel& bowModel, svm_model* svmModel)
{
	/////////////////////////////////////////////
	vector<double> feature;
	//TO DO: Calculate feature vector for img
	feature = getBoWDescripton(input, xSize, ySize, bowModel);



	//////////////////////////////////////////////
	svm_node* featureVector = new svm_node[bowModel.numOfClusters + 1];
	
	int j = 0;

	for (int index = 0; index < bowModel.numOfClusters; index++)
	{
		if (feature[index] != 0)
		{
			featureVector[j] = { index+1, feature[index] };
			j++;
		}
	}
	featureVector[j] = { -1, 0.0 };
	
	double res = svm_predict(svmModel, featureVector);
	
	delete[] featureVector;

	return round(res);
}