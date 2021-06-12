#include "BoW.h"
#include "kMeans.h"
#include "FeatureDetection.h"
#include "SIFTLib.h"
#include <vector>
#include <fstream>
#include <QDebug>

using namespace std;

BagOfWordsModel createBoWModel(const ImgDataBase& dataBase, int K)
{
	vector<vector<double>> uncategorizedFeatures;
	for (DBImage inImg : dataBase)
	{
		SiftKeypointList features = GetSIFTFeatures(inImg.image->bits(), inImg.image->width(), inImg.image->height());
		for (SiftKeypoint& kp : features)
		{
			vector<double> descriptor(kp.descriptors, kp.descriptors + DEGREE_OF_DESCRIPTORS);
			uncategorizedFeatures.push_back(descriptor);
		}
	}
	qInfo() << endl << "Total number of features: " << uncategorizedFeatures.size() << endl;
	qInfo() << "Invoke kMeans...";
	vector<vector<double>> BoWData = kMeansCentroids(uncategorizedFeatures, DEGREE_OF_DESCRIPTORS, K);
	qInfo() << "DONE" << endl;

	BagOfWordsModel model;
	model.model = BoWData;
	model.numOfClusters = K;
	model.featureVectorSize = DEGREE_OF_DESCRIPTORS;

	return model;
}

int findBoWIndex(const BagOfWordsModel& model, vector<double> feature)
{
	return whichIsNearest(model.model, feature);
}

vector<double> getBoWDescripton(uchar input[], int xSize, int ySize, const BagOfWordsModel& model)
{
	//TO DO
	SiftKeypointList features = GetSIFTFeatures(input, xSize, ySize);

	vector<double> ret_vector(model.numOfClusters, 0); 



	for (SiftKeypoint& kp : features) {

		vector<double> kp_descriptor;
		for (int j = 0; j < DEGREE_OF_DESCRIPTORS; j++) 
			kp_descriptor.push_back(kp.descriptors[j]);
		

		int i = findBoWIndex(model, kp_descriptor);

		ret_vector[i]++;
	}

	for (vector<double>::iterator it = ret_vector.begin(); it != ret_vector.end(); it++) 
		*it /= features.size();
	

	return ret_vector;

}

bool dumpBoWModel(const BagOfWordsModel& bowModel, string filename)
{
	ofstream outFile(filename.c_str(), std::ios::out);

	if (!outFile.is_open())
		return false;
	
	outFile << bowModel.numOfClusters << endl;
	outFile << bowModel.featureVectorSize << endl;

	outFile.precision(17);

	for (const vector<double>& feature : bowModel.model)
	{
		for (const double& value : feature)
		{
			outFile << value;
			outFile << " ";
		}
		outFile << endl;
	}

	return true;
}

bool loadBoWModel(BagOfWordsModel& bowModel, string filename)
{
	std::ifstream inFile(filename.c_str(), std::ios::in);
	
	if (!inFile.is_open())
		return false;

	inFile >> bowModel.numOfClusters;
	inFile >> bowModel.featureVectorSize;

	bowModel.model.clear();

	for (int i = 0; i < bowModel.numOfClusters; i++)
	{
		vector<double> feature(bowModel.featureVectorSize);
		for (int j = 0; j < bowModel.featureVectorSize; j++)
		{
			inFile >> feature[j];
		}
		bowModel.model.push_back(feature);
	}

	return true;
}
