#include "ColorSpace.h"


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

