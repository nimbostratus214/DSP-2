#include "DCT.h"
#include "8x8DCT.h"


void performDCT(uchar Y_buff[], char U_buff[], char V_buff[], int xSize, int ySize,int N, short data1[])
{
	uchar* inBlockY = new uchar[N * N];

	char* inBlockU = new char[N * N];
	char* inBlockV = new char[N * N];

	double* DCTKernel = new double[N * N];
	GenerateDCTmatrix(DCTKernel, N);

	short* dctCoeffsY = new short[N * N];

	short* dctCoeffsU = new short[N * N];
	short* dctCoeffsV = new short[N * N];
	
	int xSizeY, ySizeY, xSizeV, ySizeV, xSizeU, ySizeU;

	uchar* inputY2;

	char* inputU2;
	char* inputV2;

	extendBorders(Y_buff, xSize, ySize, N, &inputY2, &xSizeY, &ySizeY);

	extendBorders2(U_buff, xSize, ySize, N, &inputU2, &xSizeU, &ySizeU);
	extendBorders2(V_buff, xSize, ySize, N, &inputV2, &xSizeV, &ySizeV);

	short* data = new short[xSizeY * ySizeY];  //broj dctcoeficijenata je broj blokova u slici * broj koeficijenata u bloku... broj piksela u slici

	qInfo() << "xSize_extend_Y" << xSizeY;
	qInfo() << "ySize_extend_Y" << ySizeY;
	qInfo() << "Size data1 = " << xSizeY * ySizeY / 64; //4225 -- ovo je broj blokova u slici 
	long long k = 0;

	for (int y = 0; y < ySizeY / N; y++)
	{
		for (int x = 0; x < xSizeY / N; x++)
		{
			//obrada bloka 8x8

			for (int j = 0; j < N; j++)
			{
				for (int i = 0; i < N; i++)
				{
					//kroz blok
					inBlockY[j * N + i] = inputY2[(N * y + j) * (xSizeY)+N * x + i];
				}
			}
			DCT(inBlockY, dctCoeffsY, N, DCTKernel);
			
			for (int i = 0; i < N * N; i++)
			{
				data[k] = dctCoeffsY[i];
				k++;
			}
			

			//doZigZag(dctCoeffsY, N, 4);
			
			for (int j = 0; j < N; j++)
			{
				for (int i = 0; i < N; i++)
				{
					//inputY2[(N * y + j) * (xSizeY)+N * x + i] = inBlockY[j * N + i];
					data1[(N * y + j) * (xSizeY)+N * x + i] = data[j * N + i];
				}
			}
		}
	}
	xSize = xSizeY;
	ySize = ySizeY;
	//cropImage(inputY2, xSizeY, ySizeY, Y_buff, xSize, ySize);
	qInfo() << "DCT Y ";
	qInfo() << "k = " << k;

	/*
	for (int y = 0; y < ySizeU / N; y++)
	{
		for (int x = 0; x < xSizeU / N; x++)
		{

			for (int j = 0; j < N; j++)
			{
				for (int i = 0; i < N; i++)
				{
					inBlockU[j * N + i] = inputU2[(N * y + j) * (xSizeY)+N * x + i];
				}
			}
			DCT2(inBlockU, dctCoeffsU, N, DCTKernel);
			doZigZag(dctCoeffsU, N, 4);

			for (int i = 0; i < N * N; i++)
			{
				dctCoeffsU[i] = abs(dctCoeffsU[i]);
				if (dctCoeffsU[i] < -128)
				{
					dctCoeffsU[i] = -128;
				}
				else if (dctCoeffsU[i] > 127)
				{
					dctCoeffsU[i] = 127;
				}
				inBlockU[i] = dctCoeffsU[i];
			}

			for (int j = 0; j < N; j++)
			{
				for (int i = 0; i < N; i++)
				{
					inputU2[(N * y + j) * (xSizeY)+N * x + i] = inBlockU[j * N + i];
				}
			}
		}
	}

	cropImage2(inputU2, xSizeU, ySizeU, U_buff, xSize, ySize);


	qInfo() << "DCT _ U";

	
	for (int y = 0; y < ySizeV / N; y++)
	{
		for (int x = 0; x < xSizeV / N; x++)
		{

			for (int j = 0; j < N; j++)
			{
				for (int i = 0; i < N; i++)
				{
					inBlockV[j * N + i] = inputV2[(N * y + j) * (xSizeY)+N * x + i];
				}
			}
			DCT2(inBlockV, dctCoeffsV, N, DCTKernel);
			doZigZag(dctCoeffsV, N, 4);

			for (int i = 0; i < N * N; i++)
			{
				dctCoeffsV[i] = abs(dctCoeffsV[i]);
				if (dctCoeffsV[i] < -128)
				{
					dctCoeffsV[i] = -128;
				}
				else if (dctCoeffsV[i] > 127)
				{
					dctCoeffsV[i] = 127;
				}
				inBlockV[i] = dctCoeffsV[i];
			}

			for (int j = 0; j < N; j++)
			{
				for (int i = 0; i < N; i++)
				{
					inputV2[(N * y + j) * (xSizeY)+N * x + i] = inBlockV[j * N + i];
				}
			}
		}
	}
	cropImage2(inputV2, xSizeV, ySizeV, V_buff, xSize, ySize);


	qInfo() << "DCT _ V";

	*/

}


void performDCT(uchar Y_buff[], int xSize, int ySize, int N, short output[])
{
	uchar* inBlock = new uchar[N * N];
	double* DCTKernel = new double[N * N];
	GenerateDCTmatrix(DCTKernel, N);

	short* dctCoeffs = new short[N * N];

	int xSize2;
	int ySize2;

	uchar* input2;

	extendBorders(Y_buff, xSize, ySize, N, &input2, &xSize2, &ySize2);
	long long k = 0;

	for (int y = 0; y < ySize2 / N; y++)
	{
		for (int x = 0; x < xSize2 / N; x++)
		{
			for (int j = 0; j < N; j++)
			{
				for (int i = 0; i < N; i++)
				{
					inBlock[j * N + i] = input2[(N * y + j) * (xSize2)+N * x + i];
				}
			}

			DCT(inBlock, dctCoeffs, N, DCTKernel); //dctCoeffs je niz od 64 koeficijenta

			//treba sacuvati dct koeficijente u bafer na odredjenu poziciju 

			for (int i = 0; i < N * N; i++)
			{
				output[k] = dctCoeffs[i];
				k++;
			}

		}
	}


}


/*
void doZigZag(short block[], int N, int B)
{
	int currNum = 0;
	int currDiagonalWidth = 1;
	int i;
	int row;
	int col;

	while (currDiagonalWidth < N)
	{
		for (i = 0; i < currDiagonalWidth; i++)
		{
			if (currDiagonalWidth % 2 == 1)
			{
				row = currDiagonalWidth - i - 1;
				col = i;
			}
			else
			{
				row = i;
				col = currDiagonalWidth - i - 1;
			}

			if (currNum > B)
				block[row * N + col] = 0;
			currNum++;
		}
		currDiagonalWidth++;
	}

	while (currDiagonalWidth > 0)
	{
		for (i = currDiagonalWidth; i > 0; i--)
		{
			if (currDiagonalWidth % 2 == 1)
			{
				row = N - currDiagonalWidth + i - 1;
				col = N - i;
			}
			else
			{
				row = N - i;
				col = N - currDiagonalWidth + i - 1;
			}
			if (currNum > B)
				block[row * N + col] = 0;
			currNum++;
		}
		currDiagonalWidth--;



	}

}
*/