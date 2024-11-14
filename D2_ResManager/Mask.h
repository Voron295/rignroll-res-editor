#include <iostream>
#include "Palette.h"
#include <string>

class Mask
{
public:
	std::string m_sName;
	void *m_pData;
	void *m_pRawData;
	DWORD m_dwRawDataSize;
	RGB m_pPalette[256];
	unsigned short m_wWidth;
	unsigned short m_wHeight;
	BYTE m_bFormat;
	bool m_bNoLoad;
	class GameModule *m_pGameModule;
	class WinTreeItem *m_pTreeItem;

	Mask();
	Mask(Mask &msk);
	~Mask();
	Mask &operator=(Mask &msk);

	bool Load(FILE *f, char *name);
	bool Save(FILE *f);
	void MakeRawData();
	DWORD GetSize();
	void *GetData(void *data, Palette *plt);
	void SetData(void *data, int format, Palette *plt, int width = -1, int height = -1);
	void SetGameModule(class GameModule *mod);
	class GameModule *GetGameModule();
	void SetTreeItem(class WinTreeItem *item);
	class WinTreeItem *GetTreeItem();
	void Blank();
};