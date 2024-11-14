#include "SoundFile.h"
#include "FileManager.h"

SoundFile::SoundFile()
{
	m_dwSize = 0;
	m_pData = 0;
}

SoundFile::~SoundFile()
{
	if (m_pData)
		free(m_pData);
}

void SoundFile::Load(FILE *f, char *name, DWORD size)
{
	m_sName = name;
	m_dwSize = size;
	m_pData = malloc(m_dwSize);
	fread(m_pData, 1, m_dwSize, f);
}

bool SoundFile::Save(FILE *f)
{
	fwrite(m_pData, 1, m_dwSize, f);
	return true;
}

DWORD SoundFile::GetSize()
{
	return m_dwSize;
}