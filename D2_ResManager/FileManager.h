#pragma once
#include <Windows.h>
#include <iostream>

namespace FileManager
{
	extern char g_sBuffer[2048];

	char *ReadStringZ(FILE *f, char *buf = 0);
	int ReadInt(FILE *f);
	short ReadShort(FILE *f);
	float ReadFloat(FILE *f);
	DWORD ReadDword(FILE *f);
	WORD ReadWord(FILE *f);
	BYTE ReadByte(FILE *f);

	void WriteStringZ(FILE *f, const char *buf);
	void WriteDword(FILE *f, DWORD data);
	void WriteWord(FILE *f, WORD data);
	void WriteByte(FILE *f, BYTE data);
	void WriteFloat(FILE *f, float data);

	DWORD ArrangeSizeTo(DWORD size, DWORD arrange);

	const char *GetFileNameFromPath(const char *Path);
	const char *GetFileNameWithoutExt(const char *Path);
	const char *GetFilePath(const char *Path);
	const char *GetFileExtFromPath(const char *Path);
}