
#include <QDebug>
#include "TrainSVM.h"
#include "DataBase.h"
#include "BoW.h"
#include "svm.h"
#include <fstream>

using namespace std;

// modify this function to tune SVM classifier
static void setDefaultParams(svm_parameter& param)
{
	
	//  svm_type : set type of SVM:
	//		0 -- C-SVC		(multi-class classification)
	//		1 -- nu-SVC		(multi-class classification)
	//		2 -- one-class SVM
	//		3 -- epsilon-SVR(regression)
	//		4 -- nu-SVR		(regression)
	//	kernel_type : set type of kernel function
	//		0 -- linear: u'*v
	//		1 -- polynomial: (gamma*u'*v + coef0)^degree
	//		2 -- radial basis function: exp(-gamma*|u-v|^2)
	//		3 -- sigmoid: tanh(gamma*u'*v + coef0)
	//	degree : set degree in kernel function
	//	gamma : set gamma in kernel function 
	//	coef0 : set coef0 in kernel function (default 0)
	//	C : set the parameter C of C-SVC, epsilon-SVR, and nu-SVR (default 1)
	//	nu : set the parameter nu of nu-SVC, one-class SVM, and nu-SVR (default 0.5)
	//	cachesize : set cache memory size in MB 
	//	eps : set the epsilon in loss function of epsilon-SVR 
	//	p : set tolerance of termination criterion
	//	shrinking : whether to use the shrinking heuristics, 0 or 1 \n"
	//	probability : whether to train a SVC or SVR model for probability estimates, 0 or 1 \n"
	//	weight : set the parameter C of class i to weight*C, for C-SVC \n"

	param.svm_type = C_SVC;
	param.kernel_type = RBF;
	param.degree = 3;
	param.gamma = 3;
	param.coef0 = 0;
	param.C = 10;
	param.nu = 0.5;
	param.cache_size = 100;
	param.eps = 1e-3;
	param.p = 0.1;
	param.shrinking = 1;
	param.probability = 0;
	param.nr_weight = 0;
	param.weight_label = NULL;
	param.weight = NULL;
}

bool trainSVM(const ImgDataBase& dataBase, bool createNewBoWModel, std::string BOWModelFileName, std::string SVMModelFileName)
{
	svm_problem svmProblemSet;
	svm_parameter svmParam;

	/* Create problem set */
	svmProblemSet.l = dataBase.size();
	svmProblemSet.y = new double[svmProblemSet.l];
	svmProblemSet.x = new svm_node* [svmProblemSet.l];

	/* Load SVM params */
	setDefaultParams(svmParam);
	BagOfWordsModel bowModel;
	if (createNewBoWModel)
	{
		qInfo() << "Making Bag of Words model ...";
		bowModel = createBoWModel(dataBase, histogramSize);
		if (!dumpBoWModel(bowModel, BOWModelFileName))
		{
			qWarning() << "WARNING:Unable to save BOW model (model.bow)";
		}
		qInfo() << "DONE" << endl;
	}
	else
	{
		qInfo() << "Load Bag of Words model ...";
		if (loadBoWModel(bowModel, BOWModelFileName))
		{
			qInfo() << "DONE" << endl;
		}
		else
		{	
			qCritical() << "ERROR: Unable to load BOW model (" << BOWModelFileName.c_str() << ")";
				return false;
		}
	}

	qInfo() << "Calculating features for SVM...";
	int i = 0;
	for (const DBImage& img : dataBase)
	{
		/////////////////////////////////////////////
		vector<double> feature;
		//TO DO: Calculate feature vector for img

		feature = getBoWDescripton(img.image->bits(), img.image->width(), img.image->height(), bowModel);


		//////////////////////////////////////////////

		svmProblemSet.y[i] = img.labelNumber;
		svmProblemSet.x[i] = new svm_node[bowModel.numOfClusters+1];
		
		int j = 0;
		for (int index = 0; index < bowModel.numOfClusters; index++)
		{
			if (feature[index] != 0)
			{
				svmProblemSet.x[i][j] = { index + 1, feature[index] };
				j++;
			}
		}
		svmProblemSet.x[i][bowModel.numOfClusters] = { -1, 0.0 };
		i++;
	}
	
	qInfo() << "DONE" << endl;
	
	qInfo() << "Training SVM model ...";

	if (svm_check_parameter(&svmProblemSet, &svmParam))
	{
		qCritical() << "ERROR: invalid problem set!" << endl;
		return false;
	}
	/////////////////////////////////////////////////////////////
	svm_model* model;
	//TO DO: Invoke svm_train function and place result in model

	model = svm_train(&svmProblemSet, &svmParam); //check


	/////////////////////////////////////////////////////////////

	
	qInfo() << "DONE" << endl;

	if(svm_save_model(SVMModelFileName.c_str(), model))
	{
		qWarning() <<"ERROR: can't save model to file";
	}
	
	svm_free_and_destroy_model(&model);

	delete[] svmProblemSet.y;
	delete[] svmProblemSet.x;

	return true;
}


bool crossValidateKFold(const ImgDataBase& dataBase, int K, bool createNewBoWModel, std::string BOWModelFileName)
{
	svm_problem svmProblemSet;
	svm_parameter svmParam;

	/* Create problem set */
	svmProblemSet.l = dataBase.size();
	svmProblemSet.y = new double[svmProblemSet.l];
	svmProblemSet.x = new svm_node*[svmProblemSet.l];

	/* Load SVM params */
	setDefaultParams(svmParam);
	BagOfWordsModel bowModel;
	if (createNewBoWModel)
	{
		qInfo() << "Making Bag of Words model ...";
		bowModel = createBoWModel(dataBase, histogramSize);
		if (!dumpBoWModel(bowModel, BOWModelFileName))
		{
			qWarning() << "WARNING:Unable to save BOW model (model.bow)";
		}
		qInfo() << "DONE" << endl;
	}
	else
	{
		qInfo() << "Load Bag of Words model ...";
		if (loadBoWModel(bowModel, BOWModelFileName))
		{
			qInfo() << "DONE" << endl;
		}
		else
		{	
			qCritical() << "ERROR: Unable to load BOW model (" << BOWModelFileName.c_str() << ")";
				return false;
		}
	}

	qInfo() << "Calculating features for SVM...";
	int i = 0;
	for (const DBImage& img : dataBase)
	{
		/////////////////////////////////////////////
		vector<double> feature;
		//TO DO: Calculate feature vector for img

		feature = getBoWDescripton(img.image->bits(), img.image->width(), img.image->height(), bowModel);


		//////////////////////////////////////////////

		svmProblemSet.y[i] = img.labelNumber;
		svmProblemSet.x[i] = new svm_node[bowModel.numOfClusters + 1];

		int j = 0;
		for (int index = 0; index < bowModel.numOfClusters; index++)
		{
			if (feature[index] != 0)
			{
				svmProblemSet.x[i][j] = { index + 1, feature[index] };
				j++;
			}
		}
		svmProblemSet.x[i][bowModel.numOfClusters] = { -1, 0.0 };
		i++;
	}


	qInfo() << "DONE" << endl;

	qInfo() << "Perform cross-validation, k = " << K << "...";

	int total_correct = 0;
	double *target = new double[svmProblemSet.l];

	if (svm_check_parameter(&svmProblemSet, &svmParam))
	{
		qCritical() << "ERROR: invalid problem set!" << endl;
		return false;
	}

	svm_cross_validation(&svmProblemSet, &svmParam, K, target);
	
	qInfo() << "DONE" << endl;

	for (int i = 0; i<svmProblemSet.l; i++)
		if (target[i] == svmProblemSet.y[i])
			++total_correct;
	qInfo() << "Cross Validation Accuracy = " << 100.0*total_correct / svmProblemSet.l << endl;
	
	delete[] target;
	delete[] svmProblemSet.y;
	delete[] svmProblemSet.x;

	return true;
}



