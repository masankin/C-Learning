#include "Utils.h"
#include <cstdio>
#include <fstream>

NAMESPACE_ZL_START

bool IsFileExist(const char* filepath)
{
	FILE *file = fopen(filepath, "rb");
	return file != NULL;
	//std::ifstream infile(filepath);
	//return infile.good();
}

long GetFileSize(FILE *file)
{
	if (file == NULL)
		return -1;
	fseek(file, 0, SEEK_END);
	long fileSize = ftell(file);
	fseek(file, 0, SEEK_SET);
	return fileSize;
}

long GetFileSize(const char* filepath)
{
	FILE *file = fopen(filepath, "rb");
	long fileSize = GetFileSize(file);
	fclose(file);
	return fileSize;
	//std::ifstream file(filepath);
	//file.seekg(0, std::ios::end);
	//long fileSize = file.tellg();
	//file.close();
	//return fileSize;
}

bool GetFileData(const char* filepath, std::string& buf)
{
	FILE *file = fopen(filepath, "rb");
	if (file == NULL)
		return false;

	const static int PER_READ_SIZE = 1024;
	char data[PER_READ_SIZE];
	size_t size = PER_READ_SIZE;
	while (!feof(file) && size == PER_READ_SIZE)
	{
		::memset(data, '\0', PER_READ_SIZE);
		size = fread(data, PER_READ_SIZE, 1, file);
		buf += data;
	}
	return true;
}

NAMESPACE_ZL_END