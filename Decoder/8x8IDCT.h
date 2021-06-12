
#pragma once

#ifndef NXNDCT_H_
#define NXNDCT_H_
#include <QDebug>

void GenerateDCTmatrix(double* DCTKernel, int order);

void DCT(const uchar input[], short output[], int N, double* DCTKernel);
void DCT2(const char input[], short output[], int N, double* DCTKernel);


void IDCT(const short input[], uchar output[], int N, double* DCTKernel);
void IDCT2(const short input[], char output[], int N, double* DCTKernel);


void extendBorders(uchar* input, int xSize, int ySize, int N, uchar** output, int* newXSize, int* newYSize);
void extendBorders2(char* input, int xSize, int ySize, int N, char** p_output, int* newXSize, int* newYSize);

void cropImage(uchar* input, int xSize, int ySize, uchar* output, int newXSize, int newYSize);
void cropImage2(char* input, int xSize, int ySize, char* output, int newXSize, int newYSize);

#endif // NXNDCT_H_
