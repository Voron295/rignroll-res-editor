#include "GameModule.h"
#include "FileManager.h"

GameModule::GameModule()
{
	m_bLoadedFromFile = false;
}

GameModule::~GameModule()
{
	for (auto it = m_pBackFiles.begin(); it != m_pBackFiles.end(); it++)
		delete *it;
	for (auto it = m_pMasks.begin(); it != m_pMasks.end(); it++)
		delete *it;
	for (auto it = m_pPalettes.begin(); it != m_pPalettes.end(); it++)
		delete *it;
	for (auto it = m_pSoundFiles.begin(); it != m_pSoundFiles.end(); it++)
		delete *it;
	for (auto it = m_pTextures.begin(); it != m_pTextures.end(); it++)
		delete *it;
	for (auto it = m_pMaterials.begin(); it != m_pMaterials.end(); it++)
		delete *it;
}

bool GameModule::LoadRes(const char *path, void (CALLBACK *callback)(GameModule *, int, int))
{
	FILE *f = fopen(path, "rb");
	if (!f)
		return false;

	m_sFullPath = path;
	m_sName = FileManager::GetFileNameFromPath(path);

	LONG start = ftell(f);
	fseek(f, 0, SEEK_END);
	LONG end = ftell(f);
	fseek(f, start, SEEK_SET);

	int totalCount = 0;
	int currentRes = 0;

	if (callback)
	{
		while (ftell(f) < end)
		{
			char *buf = FileManager::ReadStringZ(f);

			char type[80];
			strcpy(type, strtok(buf, " "));
			int count = atoi(strtok(NULL, " "));

			if (!strcmp(type, "PALETTEFILES") ||
				!strcmp(type, "SOUNDFILES") ||
				!strcmp(type, "BACKFILES") ||
				!strcmp(type, "MASKFILES") ||
				!strcmp(type, "TEXTUREFILES"))
			{
				for (int i = 0; i < count; i++)
				{
					FileManager::ReadStringZ(f);
					DWORD size = FileManager::ReadDword(f);
					fseek(f, size, SEEK_CUR);
				}
				totalCount += count;
			}
			else
			{
				for (int i = 0; i < count; i++)
				{
					FileManager::ReadStringZ(f);
				}
			}
		}
		callback(this, currentRes, totalCount);
	}

	fseek(f, start, SEEK_SET);
	while (ftell(f) < end)
	{
		char *buf = FileManager::ReadStringZ(f);

		char type[80];
		strcpy(type, strtok(buf, " "));
		int count = atoi(strtok(NULL, " "));

		if (!strcmp(type, "PALETTEFILES"))
		{
			for (int i = 0; i < count; i++)
			{
				char *filename = FileManager::ReadStringZ(f);
				DWORD size = FileManager::ReadDword(f);
				DWORD fileStart = ftell(f);
				Palette *p = new Palette();
				p->Load(f, filename);
				m_pPalettes.push_back(p);
				fseek(f, fileStart + size, SEEK_SET);

				if (callback)
					callback(this, ++currentRes, totalCount);
			}
		}
		else if (!strcmp(type, "SOUNDFILES"))
		{
			for (int i = 0; i < count; i++)
			{
				char *filename = FileManager::ReadStringZ(f);
				DWORD size = FileManager::ReadDword(f);
				DWORD fileStart = ftell(f);
				SoundFile *p = new SoundFile();
				p->Load(f, filename, size);
				m_pSoundFiles.push_back(p);
				fseek(f, fileStart + size, SEEK_SET);

				if (callback)
					callback(this, ++currentRes, totalCount);
			}
		}
		else if (!strcmp(type, "SOUNDS"))
		{
			for (int i = 0; i < count; i++)
			{
				m_pSounds.push_back(FileManager::ReadStringZ(f));
			}
		}
		else if (!strcmp(type, "BACKFILES"))
		{
			for (int i = 0; i < count; i++)
			{
				char *filename = FileManager::ReadStringZ(f);
				DWORD size = FileManager::ReadDword(f);
				DWORD fileStart = ftell(f);
				ResTexture *m = new ResTexture();
				m->Load(f, filename);
				m_pTextures.push_back(m);
				fseek(f, fileStart + size, SEEK_SET);

				if (callback)
					callback(this, ++currentRes, totalCount);
			}
		}
		else if (!strcmp(type, "MASKFILES"))
		{
			for (int i = 0; i < count; i++)
			{
				char *filename = FileManager::ReadStringZ(f);
				DWORD size = FileManager::ReadDword(f);
				DWORD fileStart = ftell(f);
				Mask *m = new Mask();
				m->Load(f, filename);
				m_pMasks.push_back(m);
				fseek(f, fileStart + size, SEEK_SET);

				if (callback)
					callback(this, ++currentRes, totalCount);
			}
		}
		else if (!strcmp(type, "TEXTUREFILES"))
		{
			for (int i = 0; i < count; i++)
			{
				char *filename = FileManager::ReadStringZ(f);
				DWORD size = FileManager::ReadDword(f);
				DWORD fileStart = ftell(f);
				ResTexture *m = new ResTexture();
				m->Load(f, filename);
				m_pTextures.push_back(m);
				fseek(f, fileStart + size, SEEK_SET);

				if (callback)
					callback(this, ++currentRes, totalCount);
			}
		}
		else if (!strcmp(type, "MATERIALS"))
		{
			for (int i = 0; i < count; i++)
			{
				Material *mat = new Material();
				mat->Load(f);
				m_pMaterials.push_back(mat);
			}
		}
		else if (!strcmp(type, "COLORS"))
		{
			for (int i = 0; i < count; i++)
			{
				FileManager::ReadStringZ(f);
			}
		}
		else
		{
			fclose(f);
			return false;
		}
	}
	fclose(f);
	m_bLoadedFromFile = true;
	return true;
}

bool GameModule::SaveRes(const char *path, void (CALLBACK *callback)(GameModule *, int, int))
{
	if (!path)
		path = m_sFullPath.c_str();

	FILE *f = fopen(path, "wb");
	if (!f)
		return false;

	LONG start = ftell(f);

	fseek(f, 0, SEEK_END);
	int totalCount = 0;
	int currentRes = 0;
	if (callback)
	{
		totalCount = m_pMasks.size() + m_pPalettes.size() + m_pSoundFiles.size() + m_pTextures.size();
		callback(this, currentRes, totalCount);
	}

	std::string res_type = std::string("PALETTEFILES ") + std::to_string(m_pPalettes.size());
	FileManager::WriteStringZ(f, res_type.c_str());
	for (auto it = m_pPalettes.begin(); it != m_pPalettes.end(); it++)
	{
		auto obj = *it;
		FileManager::WriteStringZ(f, obj->m_sName.c_str());
		FileManager::WriteDword(f, obj->GetSize());
		obj->Save(f);
		if (callback)
			callback(this, ++currentRes, totalCount);
	}

	res_type = std::string("SOUNDFILES ") + std::to_string(m_pSoundFiles.size());
	FileManager::WriteStringZ(f, res_type.c_str());
	for (auto it = m_pSoundFiles.begin(); it != m_pSoundFiles.end(); it++)
	{
		auto obj = *it;
		FileManager::WriteStringZ(f, obj->m_sName.c_str());
		FileManager::WriteDword(f, obj->GetSize());
		obj->Save(f);
		if (callback)
			callback(this, ++currentRes, totalCount);
	}

	res_type = std::string("SOUNDS ") + std::to_string(m_pSounds.size());
	FileManager::WriteStringZ(f, res_type.c_str());
	for (auto it = m_pSounds.begin(); it != m_pSounds.end(); it++)
	{
		auto obj = *it;
		FileManager::WriteStringZ(f, obj.c_str());
	}

	res_type = std::string("MASKFILES ") + std::to_string(m_pMasks.size());
	FileManager::WriteStringZ(f, res_type.c_str());
	for (auto it = m_pMasks.begin(); it != m_pMasks.end(); it++)
	{
		auto obj = *it;
		FileManager::WriteStringZ(f, obj->m_sName.c_str());
		FileManager::WriteDword(f, obj->GetSize());
		obj->Save(f);
		if (callback)
			callback(this, ++currentRes, totalCount);
	}

	res_type = std::string("TEXTUREFILES ") + std::to_string(m_pTextures.size());
	FileManager::WriteStringZ(f, res_type.c_str());
	for (auto it = m_pTextures.begin(); it != m_pTextures.end(); it++)
	{
		auto obj = *it;
		obj->Save(f);
		if (callback)
			callback(this, ++currentRes, totalCount);
	}

	res_type = std::string("MATERIALS ") + std::to_string(m_pMaterials.size());
	FileManager::WriteStringZ(f, res_type.c_str());
	for (auto it = m_pMaterials.begin(); it != m_pMaterials.end(); it++)
	{
		auto obj = *it;
		obj->Save(f);
	}

	fclose(f);
	return true;
}

std::string GameModule::GetName()
{
	return m_sName;
}

int GameModule::GetTextureId(ResTexture *tex)
{
	int i = 0;
	for (auto it = m_pTextures.begin(); it != m_pTextures.end(); it++)
	{
		if ((*it) == tex)
		{
			return i;
		}
		i++;
	}
	return -1;
}

int GameModule::GetMaterialId(Material *mat)
{
	int i = 0;
	for (auto it = m_pMaterials.begin(); it != m_pMaterials.end(); it++)
	{
		if ((*it) == mat)
		{
			return i;
		}
		i++;
	}
	return -1;
}

int GameModule::GetMaskId(Mask *msk)
{
	int i = 0;
	for (auto it = m_pMasks.begin(); it != m_pMasks.end(); it++)
	{
		if ((*it) == msk)
		{
			return i;
		}
		i++;
	}
	return -1;
}

void GameModule::RemoveTexture(ResTexture *tex)
{
	for (auto it = m_pTextures.begin(); it != m_pTextures.end(); it++)
	{
		if ((*it) == tex)
		{
			m_pTextures.erase(it);
			break;
		}
	}
}

void GameModule::RemoveMaterial(Material *mat)
{
	for (auto it = m_pMaterials.begin(); it != m_pMaterials.end(); it++)
	{
		if ((*it) == mat)
		{
			m_pMaterials.erase(it);
			break;
		}
	}
}

void GameModule::RemoveMask(Mask *msk)
{
	int i = 0;
	for (auto it = m_pMasks.begin(); it != m_pMasks.end(); it++)
	{
		if ((*it) == msk)
		{
			m_pMasks.erase(it);
			break;
		}
	}
}