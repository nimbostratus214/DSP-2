#pragma once
#include "BitStreamWriter.h"
#include "ColorSpace.h"
#include "8x8DCT.h"
#include "QuantizationTable.h"

#define N 8
#define B1 8 //Y
#define B2 64  //U
#define B3 64  //V

// write a File header
bool BitStreamWriter::writeHeader()
{
	for (int i = 0; i < HEADER_SIZE; i++)
	{
		if (outputFile.putChar(FILE_HEADER[i]) == false)
			return false;
	}
	return true;
}

// write image info
bool BitStreamWriter::writeImageInfo(int width, int height)
{	
	if (writeShort((ushort)width) == false)
		return false;
	if (writeShort((ushort)height) == false)
		return false;
	return true;
}


// write a single byte
bool BitStreamWriter::writeByte(uchar oneByte)
{
	if (outputFile.putChar(oneByte) == false)
		return false;

	return true;
}

// write a short integer 16bit 
bool BitStreamWriter::writeShort(ushort data)
{
	if (outputFile.putChar(*(((uchar*)&data) + 1)) == false)
		return false;

	if (outputFile.putChar(*((uchar*)&data)) == false)
		return false;

	return true;
}

// write an array of bytes 8bit
int BitStreamWriter::writeData(uchar data[], int size)
{
	for (int i = 0; i < size; i++)
	{
		if (outputFile.putChar(data[i]) == false)
			break;
	}
	return size;
}

int BitStreamWriter::writeDataChar(char data[], int size)
{
	for (int i = 0; i < size; i++)
	{
		if (outputFile.putChar(data[i]) == false)
			break;
	}
	return size;
}

bool BitStreamWriter::encode(uchar input[], int xSize, int ySize)
{
	uchar* Y_buff = new uchar[xSize * ySize];
	char* U_buff = new char[xSize * ySize];
	char* V_buff = new char[xSize * ySize];

	// Write image header
	bool ret = writeHeader();
	
	if(!ret)
		return false;

	// Write image info
	ret = writeImageInfo(xSize, ySize); 

	if (!ret)
		return false;

	//rgb->yuv
	RGBtoYUV420(input, xSize, ySize, Y_buff, U_buff, V_buff);
	

	uchar* inBlock = new uchar[N * N];
	double* DCTKernel = new double[N * N];
	GenerateDCTmatrix(DCTKernel, N);

	short* dctCoeffs = new short[N * N];       
	long long k = 0;

	int xSize2, ySize2;
	uchar* input2;
	//podela na blokove NxN, NxN DCT
	extendBorders(Y_buff, xSize, ySize, N, &input2, &xSize2, &ySize2);
	writeImageInfo(xSize2, ySize2);  //save extended image size 
	writeDataChar("Y_coeffs", 9); 
	int brojac = 0;
	short* outputRle = new short[N * N * 2];

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

			DCT(inBlock, dctCoeffs, N, DCTKernel);

			//skaliranje koeficijenata na odgovarajuci opseg
			//short <=> -32, 768 to 32, 767
			for (int i = 0; i < N * N; i++) {
				//dctCoeffs[i] = 255;//abs(dctCoeffs[i]);
				//if (dctCoeffs[i] < -511)
				//{
				//	dctCoeffs[i] = 0;
				//}
				//else if (dctCoeffs[i] > 511)
				//{
				//	dctCoeffs[i] = 511;
				//}
				//dctCoeffs[i] = dctCoeffs[i];
				
			}
			
			//kvantizacija koeficijenata
			for (int j = 0; j < N; j++)
			{
				for (int i = 0; i < N; i++) 
				{
					dctCoeffs[j * N + i] = round(dctCoeffs[j * N + i] / DefaultQuantChrominance[j * N + i]); 
				}
			}

			//odbacivanje koeficijenata
			doZigZag(dctCoeffs, N, B1); 

			//rle kodovanje i zapis bloka u datoteku
			int sizeRle = 0;
			sizeRle = rle(dctCoeffs, outputRle, N);
			writeShort(sizeRle); //zapisemo broj koeficijenata (radi citanja kasnije)
			for (int i = 0; i < sizeRle; i++)
			{
				writeShort(outputRle[i]);  //zapisemo koef. bloka
			}
		}
	}


	writeDataChar("U_coeffs", 9);
	char* input2U;
	extendBorders2(U_buff, xSize, ySize, N, &input2U, &xSize2, &ySize2);
	brojac = 0;

	for (int y = 0; y < ySize2 / N; y++)
	{
		for (int x = 0; x < xSize2 / N; x++)
		{
			for (int j = 0; j < N; j++)
			{
				for (int i = 0; i < N; i++)
				{
					inBlock[j * N + i] = input2U[(N * y + j) * (xSize2)+N * x + i];
				}
			}
			DCT(inBlock, dctCoeffs, N, DCTKernel);

			//short <=> -32, 768 to 32, 767
			for (int i = 0; i < N * N; i++) {
				//dctCoeffs[i] = abs(dctCoeffs[i]);
				//if (dctCoeffs[i] < -256)
				//{
				//	dctCoeffs[i] = 0;
				//}
				//else if (dctCoeffs[i] > 255)
				//{
				//	dctCoeffs[i] = 255;
				//}
				
			}
			
			for (int j = 0; j < N; j++)
			{
				for (int i = 0; i < N; i++)
				{
					dctCoeffs[j * N + i] = round(dctCoeffs[j * N + i] / DefaultQuantChrominance[j * N + i]);
				}
			}
			
			doZigZag(dctCoeffs, N, B2);
			
			
			int sizeRle = 0;
			sizeRle = rle(dctCoeffs, outputRle, N);
			writeShort(sizeRle);
			for (int i = 0; i < sizeRle; i++)
			{
				writeShort(outputRle[i]);
				k++;
			}
		}
	}

	writeDataChar("V_coeffs", 9);
	char* input2V;
	extendBorders2(V_buff, xSize, ySize, N, &input2V, &xSize2, &ySize2);
	brojac = 0;

	for (int y = 0; y < ySize2 / N; y++)
	{
		for (int x = 0; x < xSize2 / N; x++)
		{
			for (int j = 0; j < N; j++)
			{
				for (int i = 0; i < N; i++)
				{
					inBlock[j * N + i] = input2V[(N * y + j) * (xSize2)+N * x + i];
				}
			}
			DCT(inBlock, dctCoeffs, N, DCTKernel);

			//short <=> -32, 768 to 32, 767
			for (int i = 0; i < N * N; i++) {
				//dctCoeffs[i] = abs(dctCoeffs[i]);
				if (dctCoeffs[i] < 0)
				{
				//	dctCoeffs[i] = 0;
				}
				else if (dctCoeffs[i] > 255)
				{
					//dctCoeffs[i] = 255;
				}
			}
			
			for (int j = 0; j < N; j++)
			{
				for (int i = 0; i < N; i++)
				{
					dctCoeffs[j * N + i] = round(dctCoeffs[j * N + i] / DefaultQuantChrominance[j * N + i]);
				}
			}
			

			doZigZag(dctCoeffs, N, B3);
			
			int sizeRle = 0;
			sizeRle = rle(dctCoeffs, outputRle, N);
			writeShort(sizeRle);
			for (int i = 0; i < sizeRle; i++)
			{
				writeShort(outputRle[i]);
			}
		}
	}

	return true;
}