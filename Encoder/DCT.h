#pragma once


#ifndef DCT_H_
#define DCT_H_
#include <QDebug>

void performDCT(uchar Y_buff[], char U_buff[], char V_buff[], int xSize, int ySize, int N, short data1[]);

void performDCT(uchar Y_buff[], int xSize, int ySize, int N, short output[]);

//void doZigZag(short block[], int N, int B);


#endif