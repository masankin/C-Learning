#include "TableReader.h"
#include "HelperFunc.h"
#include "cocos2d.h"
USING_NS_CC;

TableReader::~TableReader()
{

}

bool TableReader::parse(const std::string& filename, int jumplines)
{
	bool ret = false;
	std::string szFileData = FileUtils::getInstance()->getStringFromFile(filename).c_str();
	if (szFileData.empty())
		return false;
	unsigned long filesize = szFileData.length();
	const char* pBuffer = szFileData.c_str();

	char* newData = new char[filesize + 1];
	if (newData)
	{
		memcpy(newData, pBuffer, filesize);
		newData[filesize] = 0;

		unsigned char* dataPtr = (unsigned char*)newData;
		ret = parseFromBuffer(newData, jumplines);
		delete[] newData;

		CCLOG("[trace]TableReader::parse file:%s success!", filename.c_str());
		return ret;
	}

	CCLOG("[trace]TableReader::parse file:%s fail!", filename.c_str());
	return ret;
}

bool TableReader::parseFromBuffer(const char* fileBuffer, int jumplines)
{
	std::stringstream filestream(fileBuffer);
	std::string str;

	try
	{
		for (int i = 0; i < jumplines; ++i)
		{
			std::getline(filestream, str);
		}
		while (!filestream.eof())
		{
			std::getline(filestream, str);
			if (str.length() <= 0)
				break;

			HelperFunc::replaceAllCharacters(str, ",", "\t");

			std::stringstream line1(str);
			readline(line1);
		}
		return true;
	}
	catch (...)
	{
		fprintf(stderr, "Parse Table failed!");
		return false;
	}
}
