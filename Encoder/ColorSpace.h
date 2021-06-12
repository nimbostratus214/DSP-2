#pragma once

#ifndef COLORSPACES_H_
#define COLORSPACES_H_

#include <QDebug>

//Izabran Yuv420 prostor boja   
//uchar 0-255
//char -128 - 128
void RGBtoYUV420(const uchar rgbImg[], int x, int y, uchar Y_buff[], char U_buff[], char V_buff[]);

//korisceno za testiranje
void YUV420toRGB(const uchar Y_buff[], const char U_buff[], const char V_buff[], int x, int y, uchar rgbImg[]);


#endif