#include <Windows.h>
#include <iostream>
#include "Targa.h"
#include <string>
#include "Palette.h"

#define D3DCOLOR_A(x) ((x>>24)&255)
#define D3DCOLOR_R(x) ((x>>16)&255)
#define D3DCOLOR_G(x) ((x>>8)&255)
#define D3DCOLOR_B(x) (x&255)

enum ResTextureFormat
{
	Texture8 = 1,
	Texture565 = 2,
	Texture555 = 3,
	Texture4444 = 5,
	Texture888 = 6,
	Texture8888 = 7,
	TextureBump = 8
};

class ResTexture
{
public:
	std::string m_sName;
	int m_iWidth;
	int m_iHeight;
	BYTE m_bDepth;
	BYTE m_bFormat;
	void *m_pData;
	void *m_pMipData;
	DWORD m_dwMipDataSize;
	int m_dwMipmapCount;
	bool m_bIsFont;

	class WinTreeItem *m_pTreeItem;
	class GameModule *m_pGameModule;

	bool m_bNoLoad;
	bool m_bMemfix;

	struct {
		BYTE r;
		BYTE g;
		BYTE b;
	} m_pPalette[256];
	BYTE m_bHasPalette;

	ResTexture();
	ResTexture(ResTexture &tex);
	~ResTexture();

	ResTexture &operator=(ResTexture &tex);
	void Load(FILE *f, const char *name, bool isFont = false);
	bool Save(FILE *f);
	DWORD GetSize();
	const char *GetName();
	void *GetData(void *data, Palette *plt);
	void SetData(void *data, int format, Palette *plt, int width = -1, int height = -1);
	void GenerateMipmap(void (CALLBACK *callback)(int, int));
	void *GetMipmap(int id);
	void *GetMipmapData(int id, void *pData, int &width, int &height);
	void SetMipmap(int id, void *data);
	void RemoveMipmap();
	void SetTreeItem(class WinTreeItem *item);
	class WinTreeItem *GetTreeItem();
	void SetGameModule(class GameModule *item);
	class GameModule *GetGameModule();
	void Blank();
	inline static BYTE GetNearestColor(Palette *plt, BYTE r, BYTE g, BYTE b);
};