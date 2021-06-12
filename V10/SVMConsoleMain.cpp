#include <QtCore/QCoreApplication>
#include "TrainSVM.h"
#include "PredictSVM.h"
#include "DataBase.h"

using namespace std;

/***********************************************************/
/*                     PARAMS                              */
/***********************************************************/
// Image database path and name
static const string dataBasePath = "img_database/";
static const string dataBaseName = "imageList.txt";

static const string trainBoWModelOutFile = "myBoWModel.bow";
static const string trainSVMModelOutFile = "mySVMModel.model";

static const string predictBoWModelFile = "precalculated_models\\model.bow";
static const string predictSVMModelFile = "precalculated_models\\vezba12.model";


// Number of database subsets K for K-fold cross validaton
static const int K = 5;

/***********************************************************/

typedef enum
{
	SVM_TRAIN,
	SVM_PREDICT,
	SVM_CROSSVALIDATE
} ProgramOpt_t;


int main(int argc, char *argv[])
{
	ProgramOpt_t opt = SVM_TRAIN;
	if (argc == 2)
	{
		opt = ProgramOpt_t(stoi(argv[1]));
	}

	ImgDataBase imgDataBase;
	if (!readDatabase(imgDataBase, dataBasePath, dataBaseName))
	{
		return -1;
	}

	switch (opt)
	{
		case SVM_TRAIN:
			trainSVM(imgDataBase, false, predictBoWModelFile, trainSVMModelOutFile);
			break;
		case SVM_PREDICT:
			predictSVM(imgDataBase, predictBoWModelFile, trainSVMModelOutFile);
			break;
		case SVM_CROSSVALIDATE:
			crossValidateKFold(imgDataBase, K, false, predictBoWModelFile);
			break;
		default:
			return -1;
	}

	return 0;
}
