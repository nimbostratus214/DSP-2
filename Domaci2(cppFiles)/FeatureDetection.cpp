/*Autori: Dusan Stanisic i Radenko Mihajlovic */


#include "FeatureDetection.h"
#include "ColorSpaces.h"
#include <list>
#include <math.h>
#include <cmath>


using namespace std;

#define  PI 3.14159265358979323846


void extendBorders(uchar input[], int xSize, int ySize, uchar output[], int delta)
{
	int newXSize = xSize + 2 * delta;
	int newYSize = ySize + 2 * delta;

	for (int i = 0; i < xSize; i++)
	{
		for (int j = 0; j < ySize; j++)
		{
			output[(j + delta) * newXSize + i + delta] = input[j * xSize + i];
		}
	}

	for (int i = 0; i < delta; i++)
	{
		memcpy(&output[i * newXSize + delta], &input[0], xSize);
		memcpy(&output[(ySize + delta + i) * newXSize + delta], &input[(ySize - 1) * xSize], xSize);
	}

	for (int i = 0; i < newYSize; i++)
	{
		memset(&output[i * newXSize], output[i * newXSize + delta], delta);
		memset(&output[i * newXSize + delta + xSize], output[i * newXSize + xSize + delta - 1], delta);
	}

}

//histogram double vrednosti!

void calculateHistogram(const double input[], int xSize, int ySize, int histogram[])
{
	/* TODO */
	for (int i = 0; i < xSize; i++)
	{
		for (int j = 0; j < ySize; j++)
		{
			histogram[(int)input[j * xSize + i]] += 1 + input[j*xSize + i];
		}
	}
}





void performMedianFilter(uchar input[], int xSize, int ySize, int N)
{
	std::vector<uchar> temp(N * N);
	uchar* input_copy = new uchar[xSize * ySize];
	for (int i = 0; i < xSize * ySize; i++)
	{
		input_copy[i] = input[i];
	}


	for (int i = 0; i < xSize; i++)
	{
		for (int j = 0; j < ySize; j++)
		{
			int k = 0;
			for (int m = i - N / 2; m <= i + N / 2; m++)
			{
				for (int n = j - N / 2; n <= j + N / 2; n++)
				{
					if (m >= 0 && m < xSize && n >= 0 && n < ySize)
					{
						temp[k] = input_copy[m + n * xSize];
					}
					else
					{
						temp[k] = 0;
					}
					k++;
				}

				std::sort(temp.begin(), temp.end());
				input[i + j * xSize] = temp[N * N / 2];
			}

		}
	}
	delete[] input_copy;

}



//xSize -> horizontalno; ySize -> vertikalno

vector<double> calculateFeatureVector(const uchar input[], int xSize, int ySize)
{

	double* histogram = new double[9];


	for (int i = 0; i < 9; i++) {
		histogram[i] = 0;
	}

	vector<double> features(*histogram, 0); 
	features.clear();


	uchar* Y_buff = new uchar[xSize * ySize];
	char* U_buff = new char[xSize * ySize / 4];
	char* V_buff = new char[xSize * ySize / 4];

	//double* sobelVertical = new double[xSize * ySize];
	//double* sobelHorizontal = new double[xSize * ySize];

	int N = 3; //jer je velicina sobelovih operatora NxN (3x3)
	int delta = (N - 1) / 2;
	
	double hCoeff[] = { 0.25, 0.5, 0.25, 0, 0, 0,-0.25, -0.5, -0.25 };
	double vCoeff[] = { 0.25, 0, -0.25, 0.5, 0, -0.5, 0.25, 0, -0.25 };

	
	/* TO DO */

	//RGB -> YUV420 (ostatak raditi samo nad Y komponentom)
	
	RGBtoYUV420(input, xSize, ySize, Y_buff, U_buff, V_buff);


	//izracunati horizontalni i vertikalni gradijent primenom Sobel operatora nad Y komponentom (Gv i Gh)

	int newXSize = xSize + 2;
	int newYSize = ySize + 2;

	uchar* extendedImage = new uchar[newXSize * newYSize];
	double* edgeMagnitude = new double[xSize * ySize];
	double* edgeAngleBuff = new double[xSize * ySize];
	//prosirene ivice
	extendBorders(Y_buff, xSize, ySize, extendedImage, delta);
	//int iter = 0;
	double edgeAngle = 0;
	


	for (int i = 0; i < xSize; i++)
	{
		for (int j = 0; j < ySize; j++)
		{
			double Gh = 0, Gv = 0;

			for (int m = 0; m < N; m++)
			{
				for (int n = 0; n < N; n++)
				{
					Gh += extendedImage[(j + n) * newXSize + i + m] * hCoeff[(N - n) * N - m - 1];
					Gv += extendedImage[(j + n) * newXSize + i + m] * vCoeff[(N - n) * N - m - 1];
				}
			}

			edgeMagnitude[j * xSize + i] = sqrt(Gh * Gh + Gv * Gv);
			if (edgeMagnitude[j * xSize + i] > 255)
				edgeMagnitude[j * xSize + i] = 255;

			edgeAngle = atan2(Gv, Gh) + PI;  //[-PI,PI), moze se dodati +PI da se vrednost skalira na opseg [0, 2PI)

			if ((edgeAngle > 0) && (edgeAngle) < 2 * PI / 9) { //0 -> 40
				edgeAngleBuff[j * xSize + i] = 0;
				//histogram[0]++;
			}
			else if ((edgeAngle < 4 * PI / 9) && (edgeAngle > 2 * PI / 9)) { // 40 - 80
				edgeAngleBuff[j * xSize + i] = 40;
				//histogram[1]++;
			}
			else if ((edgeAngle < 6 * PI / 9) && (edgeAngle > 4 * PI / 9)) {
				edgeAngleBuff[j * xSize + i] = 80;
				//histogram[2]++;
			} 
			else if ((edgeAngle < 8 * PI / 9) && (edgeAngle > 6 * PI / 9)) {
				edgeAngleBuff[j * xSize + i] = 120;
				//histogram[3]++;
			}
			else if ((edgeAngle < 10 * PI / 9) && (edgeAngle > 8 * PI / 9)) {
				edgeAngleBuff[j * xSize + i] = 160;
				//histogram[4]++;
			}
			else if ((edgeAngle < 12 * PI / 9) && (edgeAngle > 10 * PI / 9)) {
				edgeAngleBuff[j * xSize + i] = 200;
				//histogram[5]++;
			}
			else if ((edgeAngle < 14 * PI / 9) && (edgeAngle > 12 * PI / 9)) {
				edgeAngleBuff[j * xSize + i] = 240;
				//histogram[6]++;
			}
			else if ((edgeAngle < 16 * PI / 9) && (edgeAngle > 14 * PI / 9)) {
				edgeAngleBuff[j * xSize + i] = 280;
				//histogram[7]++;
			}
			else {
				edgeAngleBuff[j * xSize + i] = 320;
				//histogram[8]++;
			}
		}
	}
	delete[] extendedImage;


	double* histPom = new double[9];

	for (int i = 0; i < 9; i++) {
		histPom[i] = 0; //histogram[i];
	}

	for (int i = 0; i < xSize / 8; i++) 
	{
		for (int j = 0; j < ySize /8; j++) 
		{
			
			for (int q = 0; q < 9; q++) {
				histPom[q] = 0; // histogram[i];
			}
		
			for (int k = 0; k < 8; k++) 
			{
				for (int l = 0; l < 8; l++)
				{
					//indeksiranje da bi se kretali po bloku
										//[(8 * i + k) * (ySize) + 8 * j + l]

					//proverimo za svaku tacku orijentaciju ugla
					//vrednost u histogramu uvecamo zza vrednost inteziteta gradijenta u toj tacki

					int ugao = edgeAngleBuff[(8 * i + k) * (ySize)+8 * j + l];	

					if (ugao == 0) {
						histPom[0]++;
						histPom[0] += edgeMagnitude[(8 * i + k) * (ySize)+8 * j + l];
					}
					else if (ugao == 40) {
						histPom[1]++;
						histPom[1] += edgeMagnitude[(8 * i + k) * (ySize)+8 * j + l];
					}
					else if (ugao == 80) {
						histPom[2]++;
						histPom[2] += edgeMagnitude[(8 * i + k) * (ySize)+8 * j + l];
					}
					else if (ugao == 120) {
						histPom[3]++;
						histPom[3] += edgeMagnitude[(8 * i + k) * (ySize)+8 * j + l];
					}
					else if (ugao == 160) {
						histPom[4]++;
						histPom[4] += edgeMagnitude[(8 * i + k) * (ySize)+8 * j + l];
					}
					else if (ugao == 200) {
						histPom[5]++;
						histPom[5] += edgeMagnitude[(8 * i + k) * (ySize)+8 * j + l];
					} 
					else if (ugao == 240) {
						histPom[6]++;
						histPom[6] += edgeMagnitude[(8 * i + k) * (ySize)+8 * j + l];
					}
					else if (ugao == 280) {
						histPom[7]++;
						histPom[7] += edgeMagnitude[(8 * i + k) * (ySize)+8 * j + l];
					}
					else {
						histPom[8]++;
						histPom[8] += edgeMagnitude[(8 * i + k) * (ySize)+8 * j + l];
					}
				
				}
			}
			
			for (int k = 0; k < 9; k++) {
				histPom[k] /= 64;
				features.push_back(histPom[k]);
			}

			//for (vector<double>::iterator it = features.begin(); it != features.end(); it++) {
			//	printf("Iterator: %f \n", *it);
			//}


		}
	}
	

	delete[] Y_buff;
	delete[] U_buff;
	delete[] V_buff;  
	delete[] edgeMagnitude;
	delete[] edgeAngleBuff;

	return features;
}




//NEW CUSTOM FUNCTION -- Old + Haris Edge Detection
//add to features vector for all 8x8 block
//izracunati broj uglova koji se nalaze u svakom od 8x8 blokova 
//i na stari vektor obelezja dodati tih 9 novih vrednosti!

vector<double> calculateFeatureVectorCustom(const uchar input[], int xSize, int ySize)
{

	double* histogram = new double[9];


	for (int i = 0; i < 9; i++) {
		histogram[i] = 0;
	}

	vector<double> features(*histogram, 0);
	features.clear();


	uchar* Y_buff = new uchar[xSize * ySize];
	char* U_buff = new char[xSize * ySize / 4];
	char* V_buff = new char[xSize * ySize / 4];

	//buffrs for Haris alg
	uchar* R_matrix = new uchar[xSize * ySize];
	double* sobelVertical = new double[xSize * ySize];
	double* sobelHorizontal = new double[xSize * ySize];

	int N = 3; //jer je velicina sobelovih operatora NxN (3x3)
	int delta = (N - 1) / 2;

	double hCoeff[] = { 0.25, 0.5, 0.25, 0, 0, 0,-0.25, -0.5, -0.25 };
	double vCoeff[] = { 0.25, 0, -0.25, 0.5, 0, -0.5, 0.25, 0, -0.25 };


	/* TO DO */

	//RGB -> YUV420 (ostatak raditi samo nad Y komponentom)

	RGBtoYUV420(input, xSize, ySize, Y_buff, U_buff, V_buff);


	//izracunati horizontalni i vertikalni gradijent primenom Sobel operatora nad Y komponentom (Gv i Gh)

	int newXSize = xSize + 2;
	int newYSize = ySize + 2;

	uchar* extendedImage = new uchar[newXSize * newYSize];
	double* edgeMagnitude = new double[xSize * ySize];
	double* edgeAngleBuff = new double[xSize * ySize];
	//prosirene ivice
	extendBorders(Y_buff, xSize, ySize, extendedImage, delta);
	//int iter = 0;
	double edgeAngle = 0;



	for (int i = 0; i < xSize; i++)
	{
		for (int j = 0; j < ySize; j++)
		{
			double Gh = 0, Gv = 0;

			for (int m = 0; m < N; m++)
			{
				for (int n = 0; n < N; n++)
				{
					Gh += extendedImage[(j + n) * newXSize + i + m] * hCoeff[(N - n) * N - m - 1];
					Gv += extendedImage[(j + n) * newXSize + i + m] * vCoeff[(N - n) * N - m - 1];
				}
			}

			sobelHorizontal[j * xSize + i] = Gh; //for Haris edge detector
			sobelVertical[j * xSize + i] = Gv;

			edgeMagnitude[j * xSize + i] = sqrt(Gh * Gh + Gv * Gv);
			if (edgeMagnitude[j * xSize + i] > 255)
				edgeMagnitude[j * xSize + i] = 255;

			edgeAngle = atan2(Gv, Gh) + PI;  //[-PI,PI), moze se dodati +PI da se vrednost skalira na opseg [0, 2PI)

			if ((edgeAngle > 0) && (edgeAngle) < 2 * PI / 9) {
				edgeAngleBuff[j * xSize + i] = 0;
				//histogram[0]++;
			}
			else if ((edgeAngle < 4 * PI / 9) && (edgeAngle > 2 * PI / 9)) {
				edgeAngleBuff[j * xSize + i] = 40;
				//histogram[1]++;
			}
			else if ((edgeAngle < 6 * PI / 9) && (edgeAngle > 4 * PI / 9)) {
				edgeAngleBuff[j * xSize + i] = 80;
				//histogram[2]++;
			}
			else if ((edgeAngle < 8 * PI / 9) && (edgeAngle > 6 * PI / 9)) {
				edgeAngleBuff[j * xSize + i] = 120;
				//histogram[3]++;
			}
			else if ((edgeAngle < 10 * PI / 9) && (edgeAngle > 8 * PI / 9)) {
				edgeAngleBuff[j * xSize + i] = 160;
				//histogram[4]++;
			}
			else if ((edgeAngle < 12 * PI / 9) && (edgeAngle > 10 * PI / 9)) {
				edgeAngleBuff[j * xSize + i] = 200;
				//histogram[5]++;
			}
			else if ((edgeAngle < 14 * PI / 9) && (edgeAngle > 12 * PI / 9)) {
				edgeAngleBuff[j * xSize + i] = 240;
				//histogram[6]++;
			}
			else if ((edgeAngle < 16 * PI / 9) && (edgeAngle > 14 * PI / 9)) {
				edgeAngleBuff[j * xSize + i] = 280;
				//histogram[7]++;
			}
			else {
				edgeAngleBuff[j * xSize + i] = 320;
				//histogram[8]++;
			}
		}
	}
	//delete[] extendedImage; //save for Haris alg!
	
	//////////////////haris edge detector ////////////////

	for (int i = 1; i < xSize-1; i++)
	{
		for (int j = 1; j < ySize-1; j++)
		{
			double Ix = 0, Iy = 0, IxIy = 0;

			//N = 3 (3x3 okolina)
			for (int m = 0; m < N; m++)
			{
				for (int n = 0; n < N; n++)
				{
					Ix += sobelHorizontal[(j + n) * ySize + i + m] * sobelHorizontal[(j + n) * ySize + i + m];
					Iy += sobelVertical[(j + n) * ySize + i + m] * sobelVertical[(j + n) * ySize + i + m];
					IxIy += sobelHorizontal[(j + n) * ySize + i + m] * sobelVertical[(j + n) * ySize + i + m];
				}
			}

			double det = 0;
			det = Ix * Iy - IxIy * IxIy;

			double R = 0;
			double alfa = 0.04; //modify this param for better results!
			double trace = Ix + Iy;

			R = det - alfa * trace * trace;

			if (R > 255) {
				R = 255;
			}
			else if (R < 0) {
				R = 0;
			}

			R_matrix[j * xSize + i] = R;

		}
	}

	//min and max of R_matrix
	uchar Rmin = R_matrix[0];
	uchar Rmax = R_matrix[0];

	for (int i = 0; i < xSize * ySize; i++)
	{
		if (R_matrix[i] > Rmax) {
			Rmax = R_matrix[i];
		}
		if (R_matrix[i] < Rmin)
		{
			Rmin = R_matrix[i];
		}
	}

	for (int i = 0; i < xSize * ySize; i++)
	{
		R_matrix[i] = (R_matrix[i] - Rmin) * 255 / (Rmax - Rmin);
	}

	//sada treba proci kroz sliku blokovski, i za svaki 
	//blok izracunati broj detektovanih uglova i upisati u features vektor!

	///////////////////////////////////


	double* histPom = new double[9];

	for (int i = 0; i < 9; i++) {
		histPom[i] = 0; //histogram[i];
	}
	
	int* brojacBuff = new int[9]; //for debug purpose?
	for (int i = 0; i < 9; i++) {
		brojacBuff[i] = 0;
	}

	int w = 0;
	
	for (int i = 0; i < xSize / 8; i++)
	{
		for (int j = 0; j < ySize / 8; j++)
		{

			for (int q = 0; q < 9; q++) {
				histPom[q] = 0;
			}
			int threshold = 1; //menjati i ovaj parametar (255- ne ulazi u if)
			int brojac = 0;
			for (int k = 0; k < 8; k++)
			{
				for (int l = 0; l < 8; l++)
				{
					
					//printf("Radi(%d)\n", brojac++);
					//prebrojati uglove Harisovim detektorom
					if (R_matrix[(8 * i + k) * ySize + 8 * j + l] > threshold)
					{
						//brojac++;
						//printf("Radi(%d)\n", brojac);
						uchar* pixels = new uchar[8]; //blok 3x3, posmatramo okolne pixele

						for (int s = 0; s < 8; s++) {
							pixels[s] = 0;
						}

						pixels[0] = extendedImage[(8 * i + k - 1) * ySize + 8 * j + l - 1];
						pixels[1] = extendedImage[(8 * i + k - 1) * ySize + 8 * j + l];
						pixels[2] = extendedImage[(8 * i + k - 1) * ySize + 8 * j + l + 1];
						pixels[3] = extendedImage[(8 * i + k) * ySize + 8 * j + l - 1];
						pixels[4] = extendedImage[(8 * i + k) * ySize + 8 * j + l + 1];
						pixels[5] = extendedImage[(8 * i + k + 1) * ySize + 8 * j + l - 1];
						pixels[6] = extendedImage[(8 * i + k + 1) * ySize + 8 * j + l];
						pixels[7] = extendedImage[(8 * i + k + 1) * ySize + 8 * j + l + 1];

						bool itsWork = true;

						for (int s = 0; s < 8; s++) {
							//printf("Pixels[%d]: %d\n", s, pixels[s]);
							if (pixels[k] > input[(8 * i + k) *ySize + 8 * j + l]) {
								itsWork = false;
							}
						}

						if (itsWork) {
							brojac++; //detektovan ugao
							
						}

					}


					//indeksiranje da bi se kretali po bloku
										//[(8 * i + k) * (ySize) + 8 * j + l]

					//proverimo za svaku tacku orijentaciju ugla
					//vrednost u histogramu uvecamo zza vrednost inteziteta gradijenta u toj tacki

					int ugao = edgeAngleBuff[(8 * i + k) * (ySize)+8 * j + l];

					if (ugao == 0) {
						histPom[0] += edgeMagnitude[(8 * i + k) * (ySize)+8 * j + l] + 1;
					}
					else if (ugao == 40) {
						histPom[1] += edgeMagnitude[(8 * i + k) * (ySize)+8 * j + l] + 1;
					}
					else if (ugao == 80) {
						histPom[2] += edgeMagnitude[(8 * i + k) * (ySize)+8 * j + l] + 1;
					}
					else if (ugao == 120) {
						histPom[3] += edgeMagnitude[(8 * i + k) * (ySize)+8 * j + l] + 1;
					}
					else if (ugao == 160) {
						histPom[4] += edgeMagnitude[(8 * i + k) * (ySize)+8 * j + l] + 1;
					}
					else if (ugao == 200) {
						histPom[5] += edgeMagnitude[(8 * i + k) * (ySize)+8 * j + l] + 1;
					}
					else if (ugao == 240) {
						histPom[6] += edgeMagnitude[(8 * i + k) * (ySize)+8 * j + l] + 1;
					}
					else if (ugao == 280) {
						histPom[7] += edgeMagnitude[(8 * i + k) * (ySize)+8 * j + l] + 1;
					}
					else {
						histPom[8] += edgeMagnitude[(8 * i + k) * (ySize)+8 * j + l] + 1;
					}

				}
			}


			//printf("Brojac radi: %d \n", brojac);
			

			for (int k = 0; k < 9; k++) {
				histPom[k] /= 64;
				features.push_back(histPom[k]);
			}
			brojacBuff[w] = brojac;
			w++;
			
			//features.push_back(brojac); //vratiti ovo 

			//for (vector<double>::iterator it = features.begin(); it != features.end(); it++) {
			//	printf("Iterator: %f \n", *it);
			//}


		}
	}

	for (int i = 0; i < 9; i++)
	{
		features.push_back(brojacBuff[i]);
	}

	
	delete[] Y_buff;
	delete[] U_buff;
	delete[] V_buff;
	delete[] edgeMagnitude;
	delete[] edgeAngleBuff;
	delete[] extendedImage;

	return features;
}


