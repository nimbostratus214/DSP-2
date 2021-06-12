#pragma once
#include "BitStreamReader.h"
#include "ColorSpace.h"
#include "QString"
#include "QDebug"
#include "8x8IDCT.h"
#include "QuantizationTable.h"

#define N 8

// Read and check header
bool BitStreamReader::readHeader()
{
	char* header = new char[HEADER_SIZE];

	// Read header
	readData((uchar*)header, HEADER_SIZE);
	
	// Check header content
	for (int i = 0; i < HEADER_SIZE; i++)
	{
		if (header[i] != FILE_HEADER[i])
			return false;
	}

	delete[] header;

	return true;

}

// Read image size
bool BitStreamReader::readImageInfo(int& width, int& height)
{
	width = readShort();
	height = readShort();

	return true;
}

// Decode image
// Expcted output is image in RGB888 format
bool BitStreamReader::decode(uchar* &output, int &xSize, int &ySize)
{
	// Read and check header
	if (!readHeader())
	{
		qCritical() << "ERROR::File Header corrupted.";
		return false;
	}

	// Get image size
	readImageInfo(xSize, ySize);

	// Create output image buffer
	output = new uchar[xSize*ySize * 3];

	uchar* Y_buff = new uchar[xSize * ySize];
	char* U_buff = new char[xSize * ySize];
	char* V_buff = new char[xSize * ySize];

	int xSize2;
	int ySize2;
	readImageInfo(xSize2, ySize2); //dimenzije prosirene slike
	skip(9); 

	uchar* inBlock = new uchar[N * N];
	char* inBlock1 = new char[N * N];

	double* DCTKernel = new double[N * N];
	GenerateDCTmatrix(DCTKernel, N);

	short* dctCoeffs = new short[N * N];
	uchar* input2 = new uchar[xSize2 * ySize2];
	
	for (int y = 0; y < ySize2 / N; y++)
	{
		for (int x = 0; x < xSize2 / N; x++)
		{
			//citanje iz datoteke i rle dekodovanje
			int sizeRle = 0;
			int s = 0;
			sizeRle = readShort(); 
			short* rleCoefs = new short[sizeRle];
			for (int i = 0; i < sizeRle/2; i++)
			{				
				int times = 0;
				short karakter = readShort();
				times = readShort();
				while (times != 0)
				{
					dctCoeffs[s] = karakter;
					s++;
					times--;
				}
			}

			//vracanje u opseg koristeci quant matricu
			for (int j = 0; j < N; j++)
			{
				for (int i = 0; i < N; i++)
				{
					dctCoeffs[j * N + i] *= DefaultQuantChrominance[j * N + i];
				}
			}
			//IDCT
			IDCT(dctCoeffs, inBlock, N, DCTKernel); 
			//sastavljanje blokova u veliku sliku
			for (int j = 0; j < N; j++)
			{
				for (int i = 0; i < N; i++)
				{
					input2[(N * y + j) * (xSize2)+N * x + i] = inBlock[j * N + i];
				}
			}
		}
	}
	//vracanje slike na originalne dimenzije i smestanje podataka u odgovarajuci buffer
	cropImage(input2, xSize2, ySize2, Y_buff, xSize, ySize);

	skip(9); 

	char* input2U = new char[xSize2 * ySize2]; 

	for (int y = 0; y < ySize2 / N; y++)
	{
		for (int x = 0; x < xSize2 / N; x++)
		{

			int sizeRle = 0;
			int s = 0;
			sizeRle = readShort();
			for (int i = 0; i < sizeRle / 2; i++)
			{
				short karakter = readShort();
				short times = readShort();
				while (times != 0)
				{
					dctCoeffs[s] = karakter;
					s++;
					times--;
				}
			}
			
			for (int j = 0; j < N; j++)
			{
				for (int i = 0; i < N; i++)
				{
					dctCoeffs[j * N + i] *= DefaultQuantChrominance[j * N + i];
				}
			}
			

			IDCT2(dctCoeffs, inBlock1, N, DCTKernel);

			for (int j = 0; j < N; j++)
			{
				for (int i = 0; i < N; i++)
				{
					input2U[(N * y + j) * (xSize2)+N * x + i] = inBlock1[j * N + i];
				}
			}
		}
	}

	cropImage2(input2U, xSize2, ySize2, U_buff, xSize, ySize);

	skip(9);

	char* input2V = new char[xSize2 * ySize2];

	for (int y = 0; y < ySize2 / N; y++)
	{
		for (int x = 0; x < xSize2 / N; x++)
		{
			int sizeRle = 0;
			int s = 0;
			sizeRle = readShort();
			for (int i = 0; i < sizeRle / 2; i++)
			{
				short karakter = readShort();
				short times = readShort();
				while (times != 0)
				{
					dctCoeffs[s] = karakter;
					s++;
					times--;
				}
			}
			

			for (int j = 0; j < N; j++)
			{
				for (int i = 0; i < N; i++)
				{
					dctCoeffs[j * N + i] *= DefaultQuantChrominance[j * N + i];
				}
			}
			
			IDCT2(dctCoeffs, inBlock1, N, DCTKernel);

			for (int j = 0; j < N; j++)
			{
				for (int i = 0; i < N; i++)
				{
					input2V[(N * y + j) * (xSize2)+N * x + i] = inBlock1[j * N + i];
				}
			}
		}
	}

	cropImage2(input2V, xSize2, ySize2, V_buff, xSize, ySize);


	//yuv -> rgb

	YUV420toRGB(Y_buff, U_buff, V_buff, xSize, ySize, output); //write to output buff
	
	return true;
}


uchar BitStreamReader::readByte()
{
	uchar data;
	inputFile.getChar((char*)&data);
	return data;
}


ushort BitStreamReader::readShort()
{
	ushort data;
	inputFile.getChar(((char*)&data) + 1);
	inputFile.getChar((char*)&data);

	return data;
}

int BitStreamReader::readData(uchar data[], int size)
{
	return inputFile.read((char*)data, size);
}

int BitStreamReader::readData1(char data[], int size)
{
	return inputFile.read(data, size);
}

bool BitStreamReader::skip(int bytes)
{
	inputFile.seek(inputFile.pos() + bytes);
	return true;
}