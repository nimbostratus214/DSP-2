#include "ColorSpace.h"




void RGBtoYUV420(const uchar rgbImg[], int x, int y, uchar Y_buff[], char U_buff[], char V_buff[])
{
	double R, G, B;
	double Y, U[4], V[4];

	for (int j = 0; j < y; j += 2)
	{
		for (int i = 0; i < x; i += 2)
		{
			for (int k1 = 0; k1 < 2; k1++)
			{
				for (int k2 = 0; k2 < 2; k2++)
				{
					R = rgbImg[(j + k2) * 3 * x + (i + k1) * 3];
					G = rgbImg[(j + k2) * 3 * x + (i + k1) * 3 + 1];
					B = rgbImg[(j + k2) * 3 * x + (i + k1) * 3 + 2];

					Y = 0.299 * R + 0.587 * G + 0.114 * B;
					Y_buff[(j + k2) * x + (i + k1)] = Y;

					U[k1 * 2 + k2] = -0.14713 * R - 0.28886 * G + 0.436 * B;
					V[k1 * 2 + k2] = 0.615 * R - 0.51499 * G - 0.10001 * B;
				}
			}
			U_buff[j / 2 * x / 2 + i / 2] = (U[0] + U[1] + U[2] + U[3]) / 4;
			V_buff[j / 2 * x / 2 + i / 2] = (V[0] + V[1] + V[2] + V[3]) / 4;
		}
	}
}


void YUV420toRGB(const uchar Y_buff[], const char U_buff[], const char V_buff[], int x, int y, uchar rgbImg[])
{
	double R, G, B;
	double Y, U, V;

	for (int j = 0; j < y; j += 2)
	{
		for (int i = 0; i < x; i += 2)
		{
			U = U_buff[j / 2 * x / 2 + i / 2];
			V = V_buff[j / 2 * x / 2 + i / 2];

			Y = Y_buff[j * x + i];

			R = Y + 1.13983 * V;
			G = Y - 0.39465 * U - 0.58060 * V;
			B = Y + 2.03211 * U;

			if (R < 0)
				R = 0;
			else if (R > 255)
				R = 255;
			if (G < 0)
				G = 0;
			else if (G > 255)
				G = 255;
			if (B < 0)
				B = 0;
			else if (B > 255)
				B = 255;

			rgbImg[j * 3 * x + i * 3] = R;
			rgbImg[j * 3 * x + i * 3 + 1] = G;
			rgbImg[j * 3 * x + i * 3 + 2] = B;

			Y = Y_buff[j * x + (i + 1)];

			R = Y + 1.13983 * V;
			G = Y - 0.39465 * U - 0.58060 * V;
			B = Y + 2.03211 * U;

			if (R < 0)
				R = 0;
			else if (R > 255)
				R = 255;
			if (G < 0)
				G = 0;
			else if (G > 255)
				G = 255;
			if (B < 0)
				B = 0;
			else if (B > 255)
				B = 255;

			rgbImg[j * 3 * x + (i + 1) * 3] = R;
			rgbImg[j * 3 * x + (i + 1) * 3 + 1] = G;
			rgbImg[j * 3 * x + (i + 1) * 3 + 2] = B;

			Y = Y_buff[(j + 1) * x + i];

			R = Y + 1.13983 * V;
			G = Y - 0.39465 * U - 0.58060 * V;
			B = Y + 2.03211 * U;

			if (R < 0)
				R = 0;
			else if (R > 255)
				R = 255;
			if (G < 0)
				G = 0;
			else if (G > 255)
				G = 255;
			if (B < 0)
				B = 0;
			else if (B > 255)
				B = 255;

			rgbImg[(j + 1) * 3 * x + i * 3] = R;
			rgbImg[(j + 1) * 3 * x + i * 3 + 1] = G;
			rgbImg[(j + 1) * 3 * x + i * 3 + 2] = B;

			Y = Y_buff[(j + 1) * x + (i + 1)];

			R = Y + 1.13983 * V;
			G = Y - 0.39465 * U - 0.58060 * V;
			B = Y + 2.03211 * U;

			if (R < 0)
				R = 0;
			else if (R > 255)
				R = 255;
			if (G < 0)
				G = 0;
			else if (G > 255)
				G = 255;
			if (B < 0)
				B = 0;
			else if (B > 255)
				B = 255;

			rgbImg[(j + 1) * 3 * x + (i + 1) * 3] = R;
			rgbImg[(j + 1) * 3 * x + (i + 1) * 3 + 1] = G;
			rgbImg[(j + 1) * 3 * x + (i + 1) * 3 + 2] = B;
		}
	}
}

