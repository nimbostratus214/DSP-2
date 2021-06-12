#pragma once

#ifndef COLORSPACES_H_
#define COLORSPACES_H_

#include <QDebug>


void RGBtoYUV420(const uchar rgbImg[], int x, int y, uchar Y_buff[], char U_buff[], char V_buff[]);

void YUV420toRGB(const uchar Y_buff[], const char U_buff[], const char V_buff[], int x, int y, uchar rgbImg[]);


#endif