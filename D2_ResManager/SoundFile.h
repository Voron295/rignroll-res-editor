#include <Windows.h>
#include <iostream>
#include <string>

class Sound
{
public:
	std::string m_sName;
	DWORD m_dwId;
};

class SoundFile
{
public:
	SoundFile();
	~SoundFile();

	void Load(FILE *f, char *name, DWORD size);
	bool Save(FILE *f);
	DWORD GetSize();

	std::string m_sName;
private:
	DWORD m_dwSize;
	void *m_pData;
};