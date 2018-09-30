
#include "HelperFunc.h"
#include "Constant.h"
#include <random>

std::string HelperFunc::replaceAllCharacters(std::string& szText, const std::string& szOld, const std::string& szNew)
{
	while (true)
	{
		std::string::size_type pos(0);
		if ((pos = szText.find(szOld)) != std::string::npos)
			szText.replace(pos, szOld.length(), szNew);
		else
			break;
	}
	return szText;
}

std::vector<std::string> HelperFunc::tokenize(const std::string& src, std::string tok)
{
	if (src.empty() || tok.empty())
	{
		throw "tokenize: empty string\0";
	}
	std::vector<std::string> v;
    size_t pre_index = 0, index = 0, len = 0;
	while ((index = src.find_first_of(tok, pre_index)) != std::string::npos)
	{
		if ((len = index - pre_index) != 0)
		{
			v.push_back(src.substr(pre_index, len));
		}
		else
		{
			v.push_back("");
		}
		pre_index = index + 1;
	}
	std::string endstr = src.substr(pre_index);
	v.push_back(endstr.empty() ? "" : endstr);
	return v;
}

std::vector<int> HelperFunc::tokenize_2int(const std::string& src, std::string tok)
{
	 std::vector<std::string> tokens = HelperFunc::tokenize(src, tok);

	std::vector<int> v;
	for (int i = 0; i != tokens.size(); ++i)
	{
		v.push_back(atoi(tokens.at(i).c_str()));
	}
	return v;
}
std::list<int> HelperFunc::tokenize_list(const std::string& src, std::string tok)
{
	std::vector<std::string> tokens = HelperFunc::tokenize(src, tok);

	std::list<int> v;
	for (int i = 0; i != tokens.size(); ++i)
	{
		v.push_back(atoi(tokens.at(i).c_str()));
	}
	return v;
}
std::vector<Point> HelperFunc::tokenize_brackets(const std::string& src, std::string tok)
{
	if (src.empty())
	{
		throw "tokenize: empty string\0";
	}

	std::vector<std::string> szVec;
	unsigned int pre_index = 0, idx = 0, len = 0;

	while ((idx = src.find_first_of("(", pre_index)) != -1)
	{
		int idx_last = src.find_first_of(")", pre_index);

		if ((len = idx_last - idx) != 0)
		{
			szVec.push_back(src.substr(pre_index + 1, idx_last - 1 - idx));
		}
		else
		{
			szVec.push_back("");
		}
		pre_index = idx_last + 1;
	}

	std::string endstr = src.substr(pre_index);
	szVec.push_back(endstr.empty() ? "" : endstr);

	// string to point
	//

	std::vector<Point>	ret;

	for (int i = 0; i != szVec.size(); ++i)
	{
		std::string sz = szVec[i];
		if (!sz.empty())
		{
			float x = 0.0f, y = 0.0f;
			replaceAllCharacters(sz, tok, "\t");
			std::stringstream line(sz);
			line >> x >> y;

			ret.push_back(Vec2(x, y));
		}
	}
	return ret;
}

std::string HelperFunc::getFromatTime(int time)
{
	time_t timep;
	timep = time;
	char tmp[64];
	strftime(tmp, sizeof(tmp), "%Y-%m-%d-%X", localtime(&timep));

	return tmp;
}

std::string HelperFunc::int2String(unsigned int id)
{
	char att[128];
	snprintf(att, 128, "%d", id);
	return std::string(att);
}

unsigned int HelperFunc::getRandomValule(int startValue, int endValue)
{
	std::uniform_int_distribution<unsigned> u(startValue,endValue);
	std::default_random_engine e(utils::gettime()); 
	unsigned int tRandomValue = u(e);
	return tRandomValue;
}


std::string HelperFunc::getComputeID()
{
#if CC_PLATFORM_WIN32 == CC_TARGET_PLATFORM
	DWORD size = 0;
	std::wstring wstr;
	GetComputerName(NULL, &size);  //得到电脑名称长度

	wchar_t *name = new wchar_t[size];
	if (GetComputerName(name, &size))
	{
		wstr = name;
	}
	delete[] name; 

	std::string szName(wstr.length(), ' ');
	std::copy(wstr.begin(), wstr.end(), szName.begin());
	return szName;
#endif
	return "";
}

void decode(unsigned char* pszFile, unsigned char** pszNew, size_t* s_size){
	unsigned long size = strlen((char*)pszFile);
	unsigned char* dataIn = pszFile;
	unsigned char* dataOut = nullptr;
	unsigned int len = base64Decode((dataIn + 2), size, &dataOut);

	*s_size = len;
	unsigned long dataSize = len;
	unsigned char* copyData = new unsigned char[dataSize + 1];
	memset((char*)copyData, 0, dataSize + 1);
	memcpy(copyData, dataOut, dataSize);
	(*pszNew) = copyData;

	delete[] dataOut;
}

#define XXTEA_KEY_LENGTH 16
static int xxtea_crypt(int mode, unsigned char * pData, int nDataLen, unsigned char * pKey, int nKeyLen)
{
	uint32_t *v, *k;
	uint32_t z, y, sum, e, DELTA;
	int n, p, q;

	if (nKeyLen < XXTEA_KEY_LENGTH)
		return 1;

	v = (uint32_t *)pData;
	k = (uint32_t *)pKey;
	n = nDataLen / sizeof(uint32_t);

	z = v[n - 1];
	y = v[0];
	sum = 0;
	e = 0;
	DELTA = 0x9e3779b9;

	if (1 == mode)	/* Encoding Part */
	{
		z = v[n - 1];
		q = 6 + 52 / n;
		while (q-- > 0)
		{
			sum += DELTA;
			e = sum >> 2 & 3;
			for (p = 0; p < n - 1; p++)
			{
				y = v[p + 1],
					z = v[p] += (z >> 5 ^ y << 2) + (y >> 3 ^ z << 4) ^ (sum^y) + (k[p & 3 ^ e] ^ z);
			}
			y = v[0];
			z = v[n - 1] += (z >> 5 ^ y << 2) + (y >> 3 ^ z << 4) ^ (sum^y) + (k[p & 3 ^ e] ^ z);
		}
		return 0;

	}
	else if (2 == mode)	/* Decoding Part */
	{
		q = 6 + 52 / n;
		sum = q*DELTA;
		while (sum != 0)
		{
			e = sum >> 2 & 3;
			for (p = n - 1; p > 0; p--)
			{
				z = v[p - 1],
					y = v[p] -= (z >> 5 ^ y << 2) + (y >> 3 ^ z << 4) ^ (sum^y) + (k[p & 3 ^ e] ^ z);
			}
			z = v[n - 1];
			y = v[0] -= (z >> 5 ^ y << 2) + (y >> 3 ^ z << 4) ^ (sum^y) + (k[p & 3 ^ e] ^ z);
			sum -= DELTA;
		}
		return 0;
	}

	return 1;
}

#define DECODE_FILE_SIGNATURE_SIZE 16
#pragma pack(push, 1)
struct DecodeFileHeader
{
	char signature[DECODE_FILE_SIGNATURE_SIZE];
	char mode;
	int64_t size;
};
#pragma pack(pop)

bool decodeXXTEA(unsigned char* input, size_t inputSize, unsigned char ** output, size_t* outputSize)
{
	if (nullptr == input || 0 == inputSize)
	{
		CCLOG("decodeXXTEA : Input data is null or input size is 0");
		return false;
	}

	if (inputSize < sizeof(DecodeFileHeader))
		return false;

	char curSignature[DECODE_FILE_SIGNATURE_SIZE];
	memset(curSignature, 0, DECODE_FILE_SIGNATURE_SIZE);
	if (strlen(MGAME_FILE_SIGNATURE) <= DECODE_FILE_SIGNATURE_SIZE)
		memcpy(curSignature, MGAME_FILE_SIGNATURE, strlen(MGAME_FILE_SIGNATURE));
	else
		memcpy(curSignature, MGAME_FILE_SIGNATURE, DECODE_FILE_SIGNATURE_SIZE);

	DecodeFileHeader hdr;
	memcpy(&hdr, input, sizeof(DecodeFileHeader));

	if (0 != memcmp(curSignature, hdr.signature, DECODE_FILE_SIGNATURE_SIZE))
	{
		//CCLOG("decodeXXTEA : Signature not match");
		return false;
	}


	if (2 != hdr.mode)
	{
		CCLOG("decodeXXTEA : Algorithm not match");
		return false;
	}

	if (output != nullptr)
	{
		auto keyStringLen = strlen(MGAME_FILE_DECODE_KEY);
		auto keyLen = keyStringLen;
		if (keyLen < XXTEA_KEY_LENGTH)
			keyLen = XXTEA_KEY_LENGTH;

		auto key = new unsigned char[keyLen + 1];
		memset(key, 0, keyLen + 1);
		memcpy(key, MGAME_FILE_DECODE_KEY, keyStringLen);

		*output = (unsigned char*)malloc(sizeof(unsigned char) * hdr.size);
		memset(*output, 0, sizeof(unsigned char) * hdr.size);
		memcpy(*output, input + sizeof(DecodeFileHeader), hdr.size);
		if (0 != xxtea_crypt(2, *output, hdr.size, key, keyLen))
		{
			CCLOG("decodeXXTEA : Decode Failed: input addr: %p, output addr : %p, key : %s, key len: %d", input, *output, key, keyLen);
			delete[] key;
			free (*output);
			return false;
		}
		delete[] key;

	}

	if (outputSize)
		*outputSize = hdr.size;

	return true;
}
