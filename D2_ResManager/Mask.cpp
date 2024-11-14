#include "Mask.h"
#include "Targa.h"
#include "FileManager.h"
#include "Texture.h"
#include <d3dx9.h>

Mask::Mask()
{
	m_pData = 0;
	m_pRawData = 0;
	m_dwRawDataSize = 0;
	m_wWidth = 0;
	m_wHeight = 0;
	m_bFormat = 0;
	m_bNoLoad = false;
	memset(m_pPalette, 0, 3 * 256);
}

Mask::Mask(Mask &msk) : Mask()
{
	if (msk.m_pData)
	{
		m_pData = malloc(msk.m_wWidth * msk.m_wHeight * msk.m_bFormat);
		memcpy(m_pData, msk.m_pData, msk.m_wWidth * msk.m_wHeight * msk.m_bFormat);
	}

	if (msk.m_pRawData)
	{
		m_pRawData = malloc(msk.m_dwRawDataSize);
		memcpy(m_pRawData, msk.m_pRawData, msk.m_dwRawDataSize);
	}
	m_dwRawDataSize = msk.m_dwRawDataSize;
	m_wWidth = msk.m_wWidth;
	m_wHeight = msk.m_wHeight;
	m_bFormat = msk.m_bFormat;
	m_bNoLoad = msk.m_bNoLoad;
	m_sName = msk.m_sName;
	memcpy(m_pPalette, msk.m_pPalette, sizeof(m_pPalette));
}

Mask &Mask::operator=(Mask &msk)
{
	if (m_pData)
	{
		free(m_pData);
		m_pData = 0;
	}
	if (m_pRawData)
	{
		free(m_pRawData);
		m_pRawData = 0;
		m_dwRawDataSize = 0;
	}

	if (msk.m_pData)
	{
		m_pData = malloc(msk.m_wWidth * msk.m_wHeight * msk.m_bFormat);
		memcpy(m_pData, msk.m_pData, msk.m_wWidth * msk.m_wHeight * msk.m_bFormat);
	}

	if (msk.m_pRawData)
	{
		m_pRawData = malloc(msk.m_dwRawDataSize);
		memcpy(m_pRawData, msk.m_pRawData, msk.m_dwRawDataSize);
	}
	m_dwRawDataSize = msk.m_dwRawDataSize;
	m_wWidth = msk.m_wWidth;
	m_wHeight = msk.m_wHeight;
	m_bFormat = msk.m_bFormat;
	m_bNoLoad = msk.m_bNoLoad;
	m_sName = msk.m_sName;
	memcpy(m_pPalette, msk.m_pPalette, sizeof(m_pPalette));
	return *this;
}

Mask::~Mask()
{
	if (m_pData)
		free(m_pData);
	if (m_pRawData)
		free(m_pRawData);
}

bool Mask::Load(FILE *f, char *name)
{
	m_sName = strtok(name, " ");// FileManager::GetFileNameFromPath(path);
	while (char *str = strtok(0, " "))
	{
		if (strcmp(str, "noload") == 0)
			m_bNoLoad = true;
	}
	char buf[5];
	fread(buf, 1, 4, f);
	buf[4] = 0;

	fread(&m_wWidth, 2, 1, f);
	fread(&m_wHeight, 2, 1, f);
	m_bFormat = 0;
 
	for (int i = 0; i < 256; i++)
	{
		fread(&m_pPalette[i].R, 1, 1, f);
		fread(&m_pPalette[i].G, 1, 1, f);
		fread(&m_pPalette[i].B, 1, 1, f);
	}

	if (!strcmp(buf, "MASK"))
	{
		m_bFormat = 1;
		m_pData = malloc(m_wWidth * m_wHeight);
		memset(m_pData, 0, m_wWidth * m_wHeight);
	}
	else if (!strcmp(buf, "MS16"))
	{
		m_bFormat = 2;
		m_pData = malloc(2 * m_wWidth * m_wHeight);
		memset(m_pData, 0, 2 * m_wWidth * m_wHeight);
	}
	char *data = (char*)m_pData;

	BYTE offset = 0;
	for (int i = 0; i < m_wHeight; i++)
	{
		for (int j = 0; j < m_wWidth; j += offset)
		{
			unsigned char id;
			fread(&id, 1, 1, f);

			offset = id & 0x7F;

			if (id & 0x80)
			{
				memset(data, 0, offset * m_bFormat);
				data += offset * m_bFormat;
			}
			else
			{
				for (int k = 0; k < offset; k++)
				{
					if (m_bFormat == 1)
					{
						fread(data, 1, 1, f);
						data += 1;
					}
					else
					{
						fread(data, 2, 1, f);
						data += 2;
					}
				}
			}
		}
	}
	
	MakeRawData();
	return true;
}

void Mask::MakeRawData()
{
	if (!m_pData)
		return;
	if (!m_bFormat)
		return;

	if (m_pRawData)
		free(m_pRawData);

	m_pRawData = 0;
	m_dwRawDataSize = 0;

	// посчитаем размер данных
	BYTE *bdata = (BYTE *)m_pData;
	WORD *wdata = (WORD *)m_pData;
	/*
	for (int i = 0; i < m_wHeight; i++)
	{
		bool skipping = false;
		int skipped = 0;
		for (int j = 0; j < m_wWidth; j++)
		{
			if (m_bFormat == 1)
			{
				if (*bdata)
				{
					m_dwRawDataSize++;
					skipping = false;
				}
				else if (!skipping)
				{
					m_dwRawDataSize++;
					skipping = true;
					skipped = 1;
				}
				else if (skipped == 127)
				{
					m_dwRawDataSize++;
					skipped = 1;
				}
				else
				{
					skipped++;
				}
				bdata++;
			}
			else
			{
				if (*wdata)
				{
					m_dwRawDataSize += 2;
					skipping = false;
				}
				else if (!skipping)
				{
					m_dwRawDataSize++;
					skipping = true;
					skipped = 1;
				}
				else if (skipped == 0x7F)
				{
					m_dwRawDataSize++;
					skipped = 1;
				}
				else
				{
					skipped++;
				}
				
				wdata++;
			}
		}
	}
	*/
	// запишем данные
	m_pRawData = malloc(m_wWidth * m_wHeight * m_bFormat * 2);

	//bdata = (BYTE *)m_pData;
	//wdata = (WORD *)m_pData;

	BYTE *rawData = (BYTE *)m_pRawData;

	for (int i = 0; i < m_wHeight; i++)
	{
		bool skipping = false;
		int count = 0;
		BYTE *countPos = rawData;
		for (int j = 0; j < m_wWidth; j++)
		{
			if (m_bFormat == 1)
			{
				if (*bdata)
				{
					if (skipping)
						rawData++;

					if (count)
					{
						*countPos = ++count;
					}

					if (!count)
					{
						countPos = rawData;
						*countPos = ++count;
						rawData++;
					}

					*rawData = *bdata;
					skipping = false;
					rawData++;

					if (count == 127)
						count = 0;
				}
				else
				{
					count = 0;

					if (!skipping)
					{
						*rawData = 0x81;
						skipping = true;
					}
					else
					{
						*rawData = *rawData + 1;
						if (*rawData == 0xFF)
						{
							skipping = false;
							rawData++;
						}
					}
				}
				bdata++;
			}
			else
			{
				if (*wdata)
				{
					if (skipping)
						rawData++;

					if (count)
					{
						*countPos = ++count;
					}

					if (!count)
					{
						countPos = rawData;
						*countPos = ++count;
						rawData++;
					}

					*(WORD*)rawData = *wdata;
					skipping = false;
					rawData += 2;

					if (count == 127)
						count = 0;
				}
				else
				{
					count = 0;

					if (!skipping)
					{
						*rawData = 0x81;
						skipping = true;
					}
					else
					{
						*rawData = *rawData + 1;
						if (*rawData == 0xFF)
						{
							skipping = false;
							rawData++;
						}
					}
				}
				wdata++;
			}
		}
		if (skipping)
		{
			rawData++;
		}

	}

	m_dwRawDataSize = rawData - (BYTE*)m_pRawData;
}

bool Mask::Save(FILE *f)
{
	if (m_bFormat == 1)
	{
		FileManager::WriteByte(f, 'M');
		FileManager::WriteByte(f, 'A');
		FileManager::WriteByte(f, 'S');
		FileManager::WriteByte(f, 'K');
	}
	else
	{
		FileManager::WriteByte(f, 'M');
		FileManager::WriteByte(f, 'S');
		FileManager::WriteByte(f, '1');
		FileManager::WriteByte(f, '6');
	}

	FileManager::WriteWord(f, m_wWidth);
	FileManager::WriteWord(f, m_wHeight);

	for (int i = 0; i < 256; i++)
	{
		fwrite(&m_pPalette[i].R, 1, 1, f);
		fwrite(&m_pPalette[i].G, 1, 1, f);
		fwrite(&m_pPalette[i].B, 1, 1, f);
	}

	fwrite(m_pRawData, m_dwRawDataSize, 1, f);
	return true;
}

DWORD Mask::GetSize()
{
	return 0x308 + m_dwRawDataSize;
}

void *Mask::GetData(void *pData, Palette *plt)
{
	D3DCOLOR *data = (D3DCOLOR *)pData;
	for (int i = 0; i < m_wWidth * m_wHeight; i++)
	{
		BYTE r, g, b;
		BYTE *byteData = (BYTE*)m_pData;
		WORD *shortData = (WORD*)m_pData;
		switch (m_bFormat)
		{
		case 1:
			data[i] = D3DCOLOR_ARGB(byteData[i] ? 255 : 0, plt->m_pPalette[byteData[i]].B, plt->m_pPalette[byteData[i]].G, plt->m_pPalette[byteData[i]].R);
			break;
		case 2:
			GetRGBFrom565(shortData[i], r, g, b);
			data[i] = D3DCOLOR_ARGB(shortData[i] ? 255 : 0, r, g, b);
			break;
		}
	}
	return pData;
}

void Mask::SetData(void *pData, int format, Palette *plt, int width, int height)
{
	D3DCOLOR *data = (D3DCOLOR *)pData;
	free(m_pData);
	m_bFormat = format;

	if (width != -1)
		m_wWidth = width;
	if (height != -1)
		m_wHeight = height;

	m_pData = malloc(m_wWidth * m_wHeight * m_bFormat);
	for (int i = 0; i < m_wWidth * m_wHeight; i++)
	{
		BYTE *byteData = (BYTE*)m_pData;
		WORD *shortData = (WORD*)m_pData;
		switch (format)
		{
		case 1:
			if (D3DCOLOR_A(data[i]) < 255)
				byteData[i] = 0;
			else
				byteData[i] = ResTexture::GetNearestColor(plt, D3DCOLOR_B(data[i]), D3DCOLOR_G(data[i]), D3DCOLOR_R(data[i]));
			break;

		case 2:
			if (D3DCOLOR_A(data[i]) < 255)
				shortData[i] = 0;
			else
				shortData[i] = Get565FromRGB(D3DCOLOR_R(data[i]), D3DCOLOR_G(data[i]), D3DCOLOR_B(data[i]));
			break;
		}
	}
	MakeRawData();
}

void Mask::Blank()
{
	m_sName = "New Mask";
	void *data = malloc(32 * 32 * 4);
	memset(data, 0xFF, 32 * 32 * 4);
	SetData(data, 2, 0, 32, 32);
	free(data);
}

void Mask::SetGameModule(GameModule *mod)
{
	m_pGameModule = mod;
}

GameModule *Mask::GetGameModule()
{
	return m_pGameModule;
}

void Mask::SetTreeItem(class WinTreeItem *item)
{
	m_pTreeItem = item;
}

class WinTreeItem *Mask::GetTreeItem()
{
	return m_pTreeItem;
}