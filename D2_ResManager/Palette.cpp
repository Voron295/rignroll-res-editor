#include "Palette.h"
#include "FileManager.h"

Palette::Palette()
{
	memset(m_pBrushes, 0, sizeof(m_pBrushes));
}

Palette::~Palette()
{
	for (int i = 0; i < 256; i++)
	{
		if (m_pBrushes[i])
			DeleteObject(m_pBrushes[i]);
	}
}

bool Palette::Load(FILE *f, char *name)
{
	m_sName = name;
	fseek(f, 4, SEEK_CUR); // PLM

	fseek(f, 12, SEEK_CUR); // PLM file offset

	for (int i = 0; i < 256; i++)
	{
		fread(&m_pPalette[i].R, 1, 1, f);
		fread(&m_pPalette[i].G, 1, 1, f);
		fread(&m_pPalette[i].B, 1, 1, f);
	}
	InitBrushes();
	return true;
}

bool Palette::Save(FILE *f)
{
	FileManager::WriteStringZ(f, "PLM");
	FileManager::WriteDword(f, 0x308);
	FileManager::WriteByte(f, 'P');
	FileManager::WriteByte(f, 'A');
	FileManager::WriteByte(f, 'L');
	FileManager::WriteByte(f, 'T');
	FileManager::WriteDword(f, 0x300);

	for (int i = 0; i < 256; i++)
	{
		fwrite(&m_pPalette[i].R, 1, 1, f);
		fwrite(&m_pPalette[i].G, 1, 1, f);
		fwrite(&m_pPalette[i].B, 1, 1, f);
	}
	return true;
}

DWORD Palette::GetSize()
{
	return 0x310;
}

Palette &Palette::operator=(Palette &plt)
{
	m_sName = plt.m_sName;
	memcpy(m_pPalette, plt.m_pPalette, sizeof(m_pPalette));
	InitBrushes();
	return *this;
}

void Palette::InitBrushes()
{
	for (int i = 0; i < 256; i++)
	{
		if (m_pBrushes[i])
			DeleteObject(m_pBrushes[i]);
		m_pBrushes[i] = CreateSolidBrush(RGB(m_pPalette[i].R, m_pPalette[i].G, m_pPalette[i].B));
	}
}