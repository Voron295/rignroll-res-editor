#include <Windows.h>
#include <iostream>
#include "Palette.h"
#include "Texture.h"
#include "Mask.h"
#include "SoundFile.h"
#include "Material.h"
#include <string>
#include <vector>

class GameModule
{
public:
	GameModule();
	~GameModule();

	bool LoadRes(const char *name, void (CALLBACK *callback)(GameModule *, int, int));
	bool SaveRes(const char *name, void (CALLBACK *callback)(GameModule *, int, int));
	void Export();
	std::string GetName();

	int GetTextureId(ResTexture *tex);
	int GetMaterialId(Material *mat);
	int GetMaskId(Mask *msk);
	void RemoveTexture(ResTexture *tex);
	void RemoveMaterial(Material *mat);
	void RemoveMask(Mask *msk);

	friend class ResUI;

private:
	std::string m_sName;
	std::string m_sFullPath;
	std::vector<Palette *> m_pPalettes;
	std::vector<SoundFile *> m_pSoundFiles;
	std::vector<std::string> m_pSounds;
	std::vector<Mask *> m_pMasks;
	std::vector<ResTexture *> m_pTextures;
	std::vector<ResTexture *> m_pBackFiles;
	std::vector<Material *> m_pMaterials;
	
	class WinTreeItem *m_pTreeItem;
	class WinTreeItem *m_pPalettesTree;
	class WinTreeItem *m_pSoundFilesTree;
	class WinTreeItem *m_pSoundsTree;
	class WinTreeItem *m_pMasksTree;
	class WinTreeItem *m_pTexturesTree;
	class WinTreeItem *m_pMaterialsTree;

	bool m_bLoadedFromFile;
};