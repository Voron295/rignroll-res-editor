#pragma once
#include <iostream>
#include "RGB.h"
#include <string>

class Palette
{
public:
	Palette();
	~Palette();

	bool Load(FILE *f, char *name);
	bool Save(FILE *f);
	DWORD GetSize();
	void InitBrushes();

	Palette &operator=(Palette &plt);

	std::string m_sName;
	RGB m_pPalette[256];
	HBRUSH m_pBrushes[256];
};