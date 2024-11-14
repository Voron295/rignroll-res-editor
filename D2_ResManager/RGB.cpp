
#include "RGB.h"

//RGB565 -> RGB888 using tables

BYTE Table5[] = { 0, 8, 16, 25, 33, 41, 49, 58, 66, 74, 82, 90, 99, 107, 115, 123, 132,
140, 148, 156, 165, 173, 181, 189, 197, 206, 214, 222, 230, 239, 247, 255 };

BYTE TableBack5[256];
BYTE TableBack6[256];

BYTE Table6[] = { 0, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 45, 49, 53, 57, 61, 65, 69,
73, 77, 81, 85, 89, 93, 97, 101, 105, 109, 113, 117, 121, 125, 130, 134, 138,
142, 146, 150, 154, 158, 162, 166, 170, 174, 178, 182, 186, 190, 194, 198,
202, 206, 210, 215, 219, 223, 227, 231, 235, 239, 243, 247, 251, 255 };

bool gInitRgbTable = false;

void InitRgbTables() 
{
	if (!gInitRgbTable)
	{
		int new5 = 0;
		int new6 = 0;
		for (int i = 0; i < 256; i++)
		{
			if (i >= Table5[new5 + 1])
				new5++;
			TableBack5[i] = new5;

			if (i >= Table6[new6 + 1])
				new6++;
			TableBack6[i] = new6;
		}
		gInitRgbTable = true;
	}
}

void GetRGBFrom555(WORD color, BYTE &r, BYTE &g, BYTE &b)
{
	b = (color & 0x7C00) >> 10;
	g = (color & 0x3E0) >> 5;
	r = (color & 0x1F);

	r = Table5[r];
	g = Table5[g];
	b = Table5[b];
}

void GetRGBFrom565(WORD color, BYTE &r, BYTE &g, BYTE &b)
{
	b = (color & 0xF800) >> 11;
	g = (color & 0x7E0) >> 5;
	r = (color & 0x1F);

	r = Table5[r];
	g = Table6[g];
	b = Table5[b];
}

WORD Get565FromRGB(BYTE r, BYTE g, BYTE b)
{
	InitRgbTables();
	/*r = r * 0x1F / 0xFF;
	g = g * 0x3F / 0xFF;
	b = b * 0x1F / 0xFF;*/
	r = TableBack5[r];
	g = TableBack6[g];
	b = TableBack5[b];
	return (b << 11) | (g << 5) | r;
}

WORD Get565From555(WORD color)
{
	BYTE r, g, b;
	r = (color & 0x7C00) >> 10;
	g = (color & 0x3E0) >> 5;
	b = (color & 0x1F);
	b = b * 255 / 31;
	g = g * 255 / 31;
	r = r * 255 / 31;
	return Get565FromRGB(r, g, b);
}

WORD Get555FromRGB(BYTE r, BYTE g, BYTE b)
{
	InitRgbTables();
	/*r = r * 0x1F / 0xFF;
	g = g * 0x1F / 0xFF;
	b = b * 0x1F / 0xFF;*/
	r = TableBack5[r];
	g = TableBack5[g];
	b = TableBack5[b];
	return (b << 10) | (g << 5) | r;
}

DWORD GetRGBAFrom4444(WORD color)
{
	BYTE out[4];
	out[0] = ((color & 0xF000) >> 12) * 17;
	out[3] = ((color & 0xF00) >> 8) * 17;
	out[2] = ((color & 0xF0) >> 4) * 17;
	out[1] = (color & 0xF) * 17;
	return *(DWORD *)&out;
}

void GetRGBAFrom4444(WORD color, BYTE &r, BYTE &g, BYTE &b, BYTE &a)
{
	a = ((color & 0xF000) >> 12) * 17;
	b = ((color & 0xF00) >> 8) * 17;
	g = ((color & 0xF0) >> 4) * 17;
	r = (color & 0xF) * 17;
}

WORD Get4444FromRGBA(BYTE r, BYTE g, BYTE b, BYTE a)
{
	r = r * 0xF / 0xFF;
	g = g * 0xF / 0xFF;
	b = b * 0xF / 0xFF;
	a = a * 0xF / 0xFF;
	return (a << 12) | (b << 8) | (g << 4) | r;
}

HSV rgb2hsv(RGB in)
{
	double inR = in.R / 255.0f;
	double inG = in.G / 255.0f;
	double inB = in.B / 255.0f;
	HSV         out;
	double      min, max, delta;

	min = inR < inG ? inR : inG;
	min = min  < inB ? min : inB;

	max = inR > inG ? inR : inG;
	max = max  > inB ? max : inB;

	out.v = max;                                // v
	delta = max - min;
	if (delta < 0.00001)
	{
		out.s = 0;
		out.h = 0; // undefined, maybe nan?
		return out;
	}
	if (max > 0.0) { // NOTE: if Max is == 0, this divide would cause a crash
		out.s = (delta / max);                  // s
	}
	else {
		// if max is 0, then r = g = b = 0              
		// s = 0, h is undefined
		out.s = 0.0;
		out.h = 0;                            // its now undefined
		return out;
	}
	if (inR >= max)                           // > is bogus, just keeps compilor happy
		out.h = (inG - inB) / delta;        // between yellow & magenta
	else
	if (inG >= max)
		out.h = 2.0 + (inB - inR) / delta;  // between cyan & yellow
	else
		out.h = 4.0 + (inR - inG) / delta;  // between magenta & cyan

	out.h *= 60.0;                              // degrees

	if (out.h < 0.0)
		out.h += 360.0;

	return out;
}