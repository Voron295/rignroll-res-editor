#include "Texture.h"
#include "FileManager.h"
#include <d3dx9.h>

ResTexture::ResTexture()
{
	m_sName = "";
	m_iWidth = 0;
	m_iHeight = 0;
	m_bDepth = 0;
	m_bFormat = 0;
	m_pData = 0;
	m_pMipData = 0;
	m_bNoLoad = false;
	m_bMemfix = false;
	m_dwMipDataSize = 0;
	m_dwMipmapCount = 0;
	memset(&m_pPalette, 0, sizeof(m_pPalette));
	m_bHasPalette = false;
	m_pTreeItem = 0;
	m_pGameModule = 0;
	m_bIsFont = false;
}

ResTexture::ResTexture(ResTexture &tex)
{
	m_sName = tex.m_sName;
	m_iWidth = tex.m_iWidth;
	m_iHeight = tex.m_iHeight;
	m_bDepth = tex.m_bDepth;
	m_bFormat = tex.m_bFormat;
	m_pData = 0;
	m_pMipData = 0;
	m_bNoLoad = tex.m_bNoLoad;
	m_bMemfix = tex.m_bMemfix;
	m_dwMipDataSize = tex.m_dwMipDataSize;
	m_dwMipmapCount = tex.m_dwMipmapCount;
	m_bIsFont = tex.m_bIsFont;
	if (tex.m_pData)
	{
		m_pData = malloc(m_iWidth * m_iHeight * m_bDepth / 8);
		memcpy(m_pData, tex.m_pData, m_iWidth * m_iHeight * m_bDepth / 8);
	}
	if (tex.m_pMipData)
	{
		m_pMipData = malloc(m_dwMipDataSize);
		memcpy(m_pMipData, tex.m_pMipData, m_dwMipDataSize);
	}
	memcpy(&m_pPalette, tex.m_pPalette, sizeof(m_pPalette));
	m_bHasPalette = tex.m_bHasPalette;
	m_pTreeItem = tex.m_pTreeItem;
	m_pGameModule = tex.m_pGameModule;
}

ResTexture &ResTexture::operator=(ResTexture &tex)
{
	m_sName = tex.m_sName;
	m_iWidth = tex.m_iWidth;
	m_iHeight = tex.m_iHeight;
	m_bDepth = tex.m_bDepth;
	m_bFormat = tex.m_bFormat;
	m_bNoLoad = tex.m_bNoLoad;
	m_bMemfix = tex.m_bMemfix;
	if (m_pData)
		free(m_pData);
	if (m_pMipData)
		free(m_pMipData);
	m_pData = 0;
	m_pMipData = 0;
	m_dwMipDataSize = tex.m_dwMipDataSize;
	m_dwMipmapCount = tex.m_dwMipmapCount;
	if (tex.m_pData)
	{
		m_pData = malloc(m_iWidth * m_iHeight * m_bDepth / 8);
		memcpy(m_pData, tex.m_pData, m_iWidth * m_iHeight * m_bDepth / 8);
	}
	if (tex.m_pMipData)
	{
		m_pMipData = malloc(m_dwMipDataSize);
		memcpy(m_pMipData, tex.m_pMipData, m_dwMipDataSize);
	}
	memcpy(&m_pPalette, tex.m_pPalette, sizeof(m_pPalette));
	m_bHasPalette = tex.m_bHasPalette;
	m_pTreeItem = tex.m_pTreeItem;
	m_pGameModule = tex.m_pGameModule;
	return *this;
}

ResTexture::~ResTexture()
{
	if (m_pData)
		free(m_pData);
	if (m_pMipData)
		free(m_pMipData);
}

void ResTexture::Load(FILE *f, const char *path, bool isFont)
{
	int nop, start, blockOffset;
	char paletteOffset = 0, flag1 = 0, depthIndicator = 0;
	char paletteColorBits = 0;
	WORD paletteCount = 0;
	m_bHasPalette = 0;
	m_pMipData = 0;

	m_iWidth = 0;
	m_iHeight = 0;
	m_bDepth = 0;

	m_bNoLoad = 0;
	m_bMemfix = 0;

	m_dwMipmapCount = 0;

	m_bIsFont = isFont;

	m_sName = strtok((char*)path, " ");// FileManager::GetFileNameFromPath(path);
	while (char *str = strtok(0, " "))
	{
		if (strcmp(str, "noload") == 0)
			m_bNoLoad = true;
		else if (strcmp(str, "memfix") == 0)
			m_bMemfix = true;
	}

	start = ftell(f);

	fread(&paletteOffset, 1, 1, f);
	fread(&m_bHasPalette, 1, 1, f); // has palette inside
	fread(&depthIndicator, 1, 1, f);
	fread(&nop, 1, 2, f);
	fread(&paletteCount, 2, 1, f);
	fread(&paletteColorBits, 1, 1, f); // bit per color

	fread(&nop, 4, 1, f); // always 0

	fread(&m_iWidth, 2, 1, f);
	fread(&m_iHeight, 2, 1, f);
	fread(&m_bDepth, 1, 1, f);

	fread(&flag1, 1, 1, f); // always 0x20

	int endOfHeader = ftell(f);
	int loffStart = 0;
	if (paletteOffset <= 0)
	{
		fseek(f, paletteOffset + endOfHeader, SEEK_SET);
		if (m_bHasPalette)
		{
			for (int i = 0; i < paletteCount; i++)
			{
				fread(&m_pPalette[i].r, 1, 1, f);
				fread(&m_pPalette[i].g, 1, 1, f);
				fread(&m_pPalette[i].b, 1, 1, f);
			}
		}

		m_pData = malloc(m_iWidth*m_iHeight);

		if (depthIndicator == 1) // 8 bit
		{
			for (int i = 0; i < m_iHeight; i++)
			{
				int j = flag1 & 0x20 ? i : m_iHeight - i - 1;
				fread(&((char*)m_pData)[m_iWidth * j], m_iWidth, 1, f);
			}
		}
		m_bFormat = 1;
	}
	else
	{
		fread(&nop, 4, 1, f); // LOFF
		fread(&nop, 4, 1, f); // 4?
		fread(&blockOffset, 4, 1, f); // end of LOFF block
		loffStart = ftell(f);

		fseek(f, start + blockOffset, SEEK_SET); // Seek to other blocks in texture
		while (!feof(f))
		{
			char blockName[4];
			fread(&blockName, 1, 4, f);
			fread(&blockOffset, 4, 1, f); // end of block
			int blockStart = ftell(f);

			if (strncmp(blockName, "LVMP", 4) == 0) // mip map data
			{
				m_dwMipmapCount = FileManager::ReadDword(f);
				int startWidth = FileManager::ReadDword(f);
				int startHeight = FileManager::ReadDword(f);
				int lvmpDepth = FileManager::ReadDword(f);
				m_dwMipDataSize = blockOffset - 16;
				m_pMipData = malloc(m_dwMipDataSize);
				fread(m_pMipData, m_dwMipDataSize, 1, f);
			}
			else if (strncmp(blockName, "PFRM", 4) == 0)
			{
				int flagR, flagG, flagB, flagA, flag5, flag6, flag7;
				fread(&flagR, 4, 1, f);
				fread(&flagG, 4, 1, f);
				fread(&flagB, 4, 1, f);
				fread(&flagA, 4, 1, f);
				if (blockOffset > 0x10)
				{
					fread(&flag5, 4, 1, f);
					fread(&flag6, 4, 1, f);
					fread(&flag7, 4, 1, f);
				}

				if (flagB == 0x1F)
				{
					if (flagG == 0x3E0)
					{
						if (flagR == 0x7C00 && !flagA)
							m_bFormat = 3; // 555
					}
					else if (flagG == 0x7E0 && flagR == 0xF800 && !flagA)
						m_bFormat = 2; // 565
				}
				else if (flagB == 0xF)
				{
					if (flagG == 0xF0 && flagR == 0xF00 && flagA == 0xF000)
					{
						m_bFormat = 5; // 4444
					}
				}
				else if (flagB == 0xFF && flagG == 0xFF00 && flagR == 0xFF0000)
				{
					if (!flagA)
					{
						m_bFormat = 6; // 888
					}
					else if (flagA == 0xFF000000)
					{
						m_bFormat = 7; // 8888
					}
				}
				else if (flag5 == 0xFF00 && flag6 == 0xFF)
					m_bFormat = 8; // bump
				else
					m_bFormat = 0;
			}
			else if (strncmp(blockName, "ENDR", 4) == 0)
			{
				break;
			}

			blockOffset = FileManager::ArrangeSizeTo(blockOffset, 4);
			fseek(f, blockStart + blockOffset, SEEK_SET);
		}

		fseek(f, loffStart, SEEK_SET);
		m_pData = malloc(m_iWidth*m_iHeight*(m_bDepth / 8));
		memset(m_pData, 0, m_iWidth*m_iHeight*(m_bDepth / 8));
		
		char *data = (char *)m_pData;
		for (int i = 0; i < m_iWidth*m_iHeight; i++)
		{
			fread(data, 1, m_bDepth / 8, f);
			data += m_bDepth / 8;
		}
	}
}

const char *ResTexture::GetName()
{
	return m_sName.c_str();
}

bool ResTexture::Save(FILE *f)
{
	using namespace FileManager;

	std::string _name = m_sName;
	if (m_bNoLoad)
		_name += " noload ";
	if (m_bMemfix)
		_name += " memfix ";

	FileManager::WriteStringZ(f, _name.c_str());
	FileManager::WriteDword(f, GetSize());

	if (m_bFormat == 1)
	{
		// write header
		WriteByte(f, 0); // palette offset
		WriteByte(f, m_bHasPalette);
		WriteByte(f, m_bDepth / 8);
		WriteWord(f, 0); // unk
		WriteWord(f, 256); // palette count
		WriteByte(f, 24); // bits per color
		WriteDword(f, 0); // unk
		WriteWord(f, m_iWidth);
		WriteWord(f, m_iHeight);
		WriteByte(f, m_bDepth);
		WriteByte(f, 32); // rows order

		if (m_bHasPalette)
		{
			for (int i = 0; i < 256; i++)
			{
				WriteByte(f, m_pPalette[i].r);
				WriteByte(f, m_pPalette[i].g);
				WriteByte(f, m_pPalette[i].b);
			}
		}
		
		fwrite(m_pData, m_iWidth * m_iHeight, 1, f);
	}
	else
	{
		WriteByte(f, 0xC);
		WriteByte(f, 0);
		WriteByte(f, m_bDepth / 8);
		WriteWord(f, 0); // unk
		WriteWord(f, 0); // palette count
		WriteByte(f, m_bDepth); // bits per color
		WriteDword(f, 0); // unk
		WriteWord(f, m_iWidth);
		WriteWord(f, m_iHeight);
		WriteByte(f, m_bDepth);
		WriteByte(f, 32); // rows order

		WriteByte(f, 'L');
		WriteByte(f, 'O');
		WriteByte(f, 'F');
		WriteByte(f, 'F');

		WriteDword(f, 4);
		DWORD realSize = ArrangeSizeTo(m_iWidth*m_iHeight*(m_bDepth / 8), 4);
		WriteDword(f, realSize + 0x1E); // loff size
		long loffStart = ftell(f);
		fwrite(m_pData, m_iWidth*m_iHeight*(m_bDepth / 8), 1, f);
		fseek(f, loffStart + realSize, SEEK_SET);

		if (m_pMipData && m_dwMipmapCount > 0)
		{
			WriteByte(f, 'L');
			WriteByte(f, 'V');
			WriteByte(f, 'M');
			WriteByte(f, 'P');
			WriteDword(f, 16 + m_dwMipDataSize); // size
			long mipStart = ftell(f);

			WriteDword(f, m_dwMipmapCount);
			WriteDword(f, m_iWidth / 2);
			WriteDword(f, m_iHeight / 2);
			WriteDword(f, m_bDepth / 8);
			fwrite(m_pMipData, m_dwMipDataSize, 1, f);

			fseek(f, mipStart + 16 + ArrangeSizeTo(m_dwMipDataSize, 4), SEEK_SET);
		}

		WriteByte(f, 'P');
		WriteByte(f, 'F');
		WriteByte(f, 'R');
		WriteByte(f, 'M');

		WriteDword(f, 0x1C);

		int flagR, flagG, flagB, flagA;
		int bumpR = 0;
		int bumpG = 0;
		int bumpB = 0;

		switch (m_bFormat)
		{
		case 2:
			flagA = 0;
			flagR = 0xF800;
			flagG = 0x7E0;
			flagB = 0x1F;
			break;

		case 3:
			flagA = 0;
			flagR = 0x7C00;
			flagG = 0x3E0;
			flagB = 0x1F;
			break;

		case 5:
			flagA = 0xF000;
			flagR = 0xF00;
			flagG = 0xF0;
			flagB = 0xF;
			break;

		case 6:
			flagA = 0;
			flagR = 0xFF0000;
			flagG = 0xFF00;
			flagB = 0xFF;
			break;

		case 7:
			flagA = 0xFF000000;
			flagR = 0xFF0000;
			flagG = 0xFF00;
			flagB = 0xFF;
			break;

		case 8:
			flagA = 0;
			flagR = 0;
			flagG = 0;
			flagB = 0;
			bumpR = 0xFF00;
			bumpG = 0xFF;
			break;
		}

		WriteDword(f, flagR);
		WriteDword(f, flagG);
		WriteDword(f, flagB);
		WriteDword(f, flagA);
		WriteDword(f, bumpR);
		WriteDword(f, bumpG);
		WriteDword(f, bumpB);

		WriteByte(f, 'E');
		WriteByte(f, 'N');
		WriteByte(f, 'D');
		WriteByte(f, 'R');
		WriteDword(f, 0);
	}
	

	return true;
}

DWORD ResTexture::GetSize()
{
	DWORD size = 0x12;
	if (m_bFormat == 1)
	{
		if (m_bHasPalette)
			size += 0x300;
		size += m_iWidth * m_iHeight;
	}
	else
	{
		size += 12 + FileManager::ArrangeSizeTo(m_iWidth*m_iHeight*(m_bDepth / 8), 4); // loff
		if (m_pMipData && m_dwMipmapCount > 0)
		{
			size += 8 + 16 + FileManager::ArrangeSizeTo(m_dwMipDataSize, 4); // lvmp
		}
		size += 0x1C + 8; // PFRM
		size += 8; // ENDR
	}
	return size;
}

inline BYTE ResTexture::GetNearestColor(Palette *plt, BYTE r, BYTE g, BYTE b)
{
	int id = 0;
	int minDiff = 9999;
	for (int i = 0; i < 256; i++)
	{
		int diff = 0;
		diff += abs(plt->m_pPalette[i].R - r);
		diff += abs(plt->m_pPalette[i].G - g);
		diff += abs(plt->m_pPalette[i].B - b);
		if (diff < minDiff)
		{
			id = i;
			minDiff = diff;
		}
	}
	return id;
}

void *ResTexture::GetData(void *pData, Palette *plt)
{
	D3DCOLOR *data = (D3DCOLOR *)pData;
	for (int i = 0; i < m_iWidth * m_iHeight; i++)
	{
		BYTE r, g, b, a;
		BYTE *byteData = (BYTE*)m_pData;
		WORD *shortData = (WORD*)m_pData;
		RGB *rgbData = (RGB*)m_pData;
		D3DCOLOR *rgbaData = (D3DCOLOR*)m_pData;
		Palette *palette = 0;
		switch (m_bFormat)
		{
		case 1:
			palette = plt;
			data[i] = D3DCOLOR_ARGB((byteData[i] || !m_bIsFont) ? 255 : 0, palette->m_pPalette[byteData[i]].B, palette->m_pPalette[byteData[i]].G, palette->m_pPalette[byteData[i]].R);
			break;
		case 2:
			GetRGBFrom565(shortData[i], r, g, b);
			data[i] = D3DCOLOR_ARGB((shortData[i] || !m_bIsFont) ? 255 : 0, r, g, b);
			break;
		case 3:
			GetRGBFrom555(shortData[i], r, g, b);
			data[i] = D3DCOLOR_ARGB((shortData[i] || !m_bIsFont) ? 255 : 0, r, g, b);
			break;
		case 5:
		case 8:
			GetRGBAFrom4444(shortData[i], r, g, b, a);
			data[i] = D3DCOLOR_ARGB(a, r, g, b);
			break;
		case 6:
			data[i] = D3DCOLOR_XRGB(rgbData[i].R, rgbData[i].G, rgbData[i].B);
			break;
		case 7:
			data[i] = D3DCOLOR_ARGB(D3DCOLOR_A(rgbaData[i]), D3DCOLOR_B(rgbaData[i]), D3DCOLOR_G(rgbaData[i]), D3DCOLOR_R(rgbaData[i]));
			break;
		}
	}
	return pData;
}

void ResTexture::SetData(void *pData, int format, Palette *plt, int width, int height)
{
	D3DCOLOR *data = (D3DCOLOR *)pData;
	if (m_pData)
		free(m_pData);
	m_bFormat = format;
	if (m_bFormat == 1)
		m_bDepth = 8;
	else if ((m_bFormat >= 2 && m_bFormat <= 5) || m_bFormat == 8)
		m_bDepth = 16;
	else if (m_bFormat == 6)
		m_bDepth = 24;
	else
		m_bDepth = 32;

	if (width != -1)
		m_iWidth = width;
	if (height != -1)
		m_iHeight = height;

	m_pData = malloc(m_iWidth * m_iHeight * m_bDepth/8);
	for (int i = 0; i < m_iWidth * m_iHeight; i++)
	{
		BYTE *byteData = (BYTE*)m_pData;
		WORD *shortData = (WORD*)m_pData;
		RGB *rgbData = (RGB*)m_pData;
		D3DCOLOR *rgbaData = (D3DCOLOR*)m_pData;
		switch (format)
		{
		case Texture8:
			byteData[i] = GetNearestColor(plt, D3DCOLOR_B(data[i]), D3DCOLOR_G(data[i]), D3DCOLOR_R(data[i]));
			break;

		case Texture565:
			shortData[i] = Get565FromRGB(D3DCOLOR_R(data[i]), D3DCOLOR_G(data[i]), D3DCOLOR_B(data[i]));
			break;

		case Texture555:
			shortData[i] = Get555FromRGB(D3DCOLOR_R(data[i]), D3DCOLOR_G(data[i]), D3DCOLOR_B(data[i]));
			break;

		case Texture4444:
		case TextureBump:
			shortData[i] = Get4444FromRGBA(D3DCOLOR_R(data[i]), D3DCOLOR_G(data[i]), D3DCOLOR_B(data[i]), D3DCOLOR_A(data[i]));
			break;

		case Texture888:
			rgbData[i] = { D3DCOLOR_R(data[i]), D3DCOLOR_G(data[i]), D3DCOLOR_B(data[i]) };
			break;

		case Texture8888:
			rgbaData[i] = D3DCOLOR_ARGB(D3DCOLOR_A(data[i]), D3DCOLOR_B(data[i]), D3DCOLOR_G(data[i]), D3DCOLOR_R(data[i]));
			break;
		}
	}
}

void *ResTexture::GetMipmapData(int id, void *pData, int &width, int &height)
{
	if (!m_pMipData || (m_dwMipmapCount <= id))
		return 0;

	int offset = 0;
	int _width = m_iWidth / 2;
	int _height = m_iHeight / 2;
	int size = _width * _height * m_bDepth / 8;
	while (id)
	{
		offset += size;
		_width = _height = _width / 2;
		size = _width * _height * m_bDepth / 8;
		id--;
	}

	width = _width;
	height = _height;

	if (!pData)
		return 0;

	D3DCOLOR *data = (D3DCOLOR *)pData;

	BYTE r, g, b, a;
	BYTE *byteData = (BYTE*)((char *)m_pMipData + offset);
	WORD *shortData = (WORD*)((char *)m_pMipData + offset);
	RGB *rgbData = (RGB*)((char *)m_pMipData + offset);
	D3DCOLOR *rgbaData = (D3DCOLOR*)((char *)m_pMipData + offset);
	for (int i = 0; i < _width * _height; i++)
	{
		switch (m_bFormat)
		{
		case 1:
			break;
		case 2:
			GetRGBFrom565(shortData[i], r, g, b);
			data[i] = D3DCOLOR_XRGB(r, g, b);
			break;
		case 3:
			GetRGBFrom555(shortData[i], r, g, b);
			data[i] = D3DCOLOR_XRGB(r, g, b);
			break;
		case 5:
		case 8:
			GetRGBAFrom4444(shortData[i], r, g, b, a);
			data[i] = D3DCOLOR_ARGB(a, r, g, b);
			break;
		case 6:
			data[i] = D3DCOLOR_XRGB(rgbData[i].R, rgbData[i].G, rgbData[i].B);
			break;
		case 7:
			data[i] = D3DCOLOR_ARGB(D3DCOLOR_A(rgbaData[i]), D3DCOLOR_B(rgbaData[i]), D3DCOLOR_G(rgbaData[i]), D3DCOLOR_R(rgbaData[i]));
			break;
		}
	}
	return pData;
}

void ResTexture::SetMipmap(int id, void *pData)
{
	if (!m_pMipData || (m_dwMipmapCount <= id))
		return;

	int offset = 0;
	int _width = m_iWidth / 2;
	int _height = m_iHeight / 2;
	int size = _width * _height * m_bDepth / 8;
	while (id)
	{
		offset += size;
		_width = _height = _width / 2;
		size /= 4;
		id--;
	}

	D3DCOLOR *data = (D3DCOLOR *)pData;
	
	for (int i = 0; i < _width * _height; i++)
	{
		BYTE *byteData = (BYTE*)((char *)m_pMipData + offset);
		WORD *shortData = (WORD*)((char *)m_pMipData + offset);
		RGB *rgbData = (RGB*)((char *)m_pMipData + offset);
		D3DCOLOR *rgbaData = (D3DCOLOR*)((char *)m_pMipData + offset);
		switch (m_bFormat)
		{
		case Texture8:

			break;

		case Texture565:
			shortData[i] = Get565FromRGB(D3DCOLOR_R(data[i]), D3DCOLOR_G(data[i]), D3DCOLOR_B(data[i]));
			break;

		case Texture555:
			shortData[i] = Get565FromRGB(D3DCOLOR_R(data[i]), D3DCOLOR_G(data[i]), D3DCOLOR_B(data[i]));
			break;

		case Texture4444:
		case TextureBump:
			shortData[i] = Get4444FromRGBA(D3DCOLOR_R(data[i]), D3DCOLOR_G(data[i]), D3DCOLOR_B(data[i]), D3DCOLOR_A(data[i]));
			break;

		case Texture888:
			rgbData[i] = { D3DCOLOR_R(data[i]), D3DCOLOR_G(data[i]), D3DCOLOR_B(data[i]) };
			break;

		case Texture8888:
			rgbaData[i] = D3DCOLOR_ARGB(D3DCOLOR_A(data[i]), D3DCOLOR_B(data[i]), D3DCOLOR_G(data[i]), D3DCOLOR_R(data[i]));
			break;
		}
	}
}

void ResTexture::GenerateMipmap(void (CALLBACK *callback)(int, int))
{
	if (m_iWidth == 1 || m_iWidth != m_iHeight)
	{
		if (m_pMipData)
		{
			free(m_pMipData);
			m_pMipData = 0;
		}
		m_dwMipmapCount = 0;
		return;
	}
	
	// расчёт памяти под мипмапы
	int _width = m_iWidth / 2;
	int _height = m_iHeight / 2;
	int _depth = m_bDepth / 8;
	int m_size = _width * _height * _depth;
	m_dwMipDataSize = 0;
	m_dwMipmapCount = 0;
	do
	{
		m_dwMipDataSize += m_size;
		m_dwMipmapCount++;
		_width = _height = _width / 2;
		m_size = _width * _height * _depth;
	} while (m_size);

	if (m_pMipData)
		free(m_pMipData);

	if (callback)
		callback(0, m_dwMipmapCount);

	m_pMipData = malloc(m_dwMipDataSize);

	// генерация мипмапов
	D3DCOLOR *data = (D3DCOLOR*)malloc(m_iWidth * m_iHeight * 4);
	GetData(data, 0);
	_width = m_iWidth;
	_height = m_iHeight;

	for (int i = 0; i < m_dwMipmapCount; i++)
	{
		if (callback)
			callback(i, m_dwMipmapCount);

		_width /= 2;
		_height /= 2;
		D3DCOLOR *outData = (D3DCOLOR*)malloc(_width * _height * 4);
		for (int j = 0; j < _height; j++)
		{
			for (int k = 0; k < _width; k++)
			{
				outData[j * _width + k] = data[j * _width * 4 + k * 2];
			}
		}
		free(data);
		data = outData;
		SetMipmap(i, data);
	}

	if (callback)
		callback(m_dwMipmapCount, m_dwMipmapCount);
	free(data);
}

void ResTexture::RemoveMipmap()
{
	if (m_pMipData)
		free(m_pMipData);
	m_pMipData = 0;

	m_dwMipDataSize = 0;
	m_dwMipmapCount = 0;
}

void ResTexture::SetTreeItem(class WinTreeItem *item)
{
	m_pTreeItem = item;
}

class WinTreeItem *ResTexture::GetTreeItem()
{
	return m_pTreeItem;
}

void ResTexture::SetGameModule(class GameModule *item)
{
	m_pGameModule = item;
}

class GameModule *ResTexture::GetGameModule()
{
	return m_pGameModule;
}

void ResTexture::Blank()
{
	if (m_pData)
		free(m_pData);
	if (m_pMipData)
		free(m_pMipData);

	m_bHasPalette = 0;
	m_pMipData = 0;
	m_pData = 0;

	m_bNoLoad = 0;
	m_bMemfix = 0;

	m_dwMipmapCount = 0;
	m_dwMipDataSize = 0;
	m_bIsFont = false;

	auto data = malloc(32 * 32 * 4);
	memset(data, 0xFF, 32 * 32 * 4);
	SetData(data, ResTextureFormat::Texture8888, 0, 32, 32);
	free(data);

	m_sName = "New Texture";
}