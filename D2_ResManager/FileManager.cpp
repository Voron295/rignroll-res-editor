#include "FileManager.h"

char FileManager::g_sBuffer[2048];

char *FileManager::ReadStringZ(FILE *f, char *buf)
{
	if (!buf)
		buf = g_sBuffer;

	char *buff = buf;

	while (!feof(f))
	{
		fread(buff, 1, 1, f);
		if (!*buff)
			break;

		buff++;
	}
	*buff = 0;
	return buf;
}

int FileManager::ReadInt(FILE *f)
{
	int buf;
	fread(&buf, 4, 1, f);
	return buf;
}

short FileManager::ReadShort(FILE *f)
{
	short buf;
	fread(&buf, 2, 1, f);
	return buf;
}

float FileManager::ReadFloat(FILE *f)
{
	float buf;
	fread(&buf, 4, 1, f);
	return buf;
}

DWORD FileManager::ReadDword(FILE *f)
{
	DWORD buf;
	fread(&buf, 4, 1, f);
	return buf;
}

WORD FileManager::ReadWord(FILE *f)
{
	WORD buf;
	fread(&buf, 2, 1, f);
	return buf;
}

BYTE FileManager::ReadByte(FILE *f)
{
	BYTE buf;
	fread(&buf, 1, 1, f);
	return buf;
}

void FileManager::WriteStringZ(FILE *f, const char *buf)
{
	int len = strlen(buf) + 1;
	fwrite(buf, len, 1, f);
}

void FileManager::WriteDword(FILE *f, DWORD data)
{
	fwrite(&data, 4, 1, f);
}

void FileManager::WriteWord(FILE *f, WORD data)
{
	fwrite(&data, 2, 1, f);
}

void FileManager::WriteByte(FILE *f, BYTE data)
{
	fwrite(&data, 1, 1, f);
}

void FileManager::WriteFloat(FILE *f, float data)
{
	fwrite(&data, 4, 1, f);
}

DWORD FileManager::ArrangeSizeTo(DWORD size, DWORD arrange)
{
	if (size % arrange)
		size += arrange - (size % arrange);
	return size;
}

const char *FileManager::GetFileNameFromPath(const char *Path)
{
	for (int i = strlen(Path) - 1; i >= 0; i--)
	{
		if (Path[i] == '\\' || Path[i] == '/')
		{
			return &Path[i + 1];
		}
	}
	return Path;
}

const char *FileManager::GetFileNameWithoutExt(const char *Path)
{
	static char result[512];
	const char *data = Path;
	for (int i = strlen(Path) - 1; i >= 0; i--)
	{
		if (Path[i] == '\\' || Path[i] == '/')
		{
			data = &Path[i + 1];
		}
	}

	int len = strlen(data);
	for (int i = len - 1; i >= 0; i--)
	{
		if (data[i] == '.')
		{
			strncpy(result, data, i);
			result[i] = 0;
			return result;
		}
	}

	return data;
}

const char *FileManager::GetFilePath(const char *Path)
{
	static char result[512];
	const char *data = Path;
	int pos = 0;
	for (int i = strlen(Path) - 1; i >= 0; i--)
	{
		if (Path[i] == '\\' || Path[i] == '/')
		{
			pos = i;
			break;
		}
	}

	strncpy(result, data, pos + 1);
	result[pos + 1] = 0;

	return result;
}

const char *FileManager::GetFileExtFromPath(const char *Path)
{
	for (int i = strlen(Path) - 1; i >= 0; i--)
	{
		if (Path[i] == '\\' || Path[i] == '/')
		{
			return &Path[strlen(Path)];
		}
		if (Path[i] == '.')
		{
			return &Path[i + 1];
		}
	}
	return &Path[strlen(Path)];
}