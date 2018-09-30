/*Helper functions
* jacey
* 20150506
* all expand about cocos2d-x in this file
*/
#pragma once
#include "Singleton.h"

#include "cocos2d.h"
USING_NS_CC;


class HelperFunc
{
public:
	
	static std::vector<std::string> tokenize(const std::string& src, std::string tok);
	static std::vector<int> tokenize_2int(const std::string& src, std::string tok);
	static std::list<int> tokenize_list(const std::string& src, std::string tok);
	/*
	* @brief split with "()".
	* @tok split token in bracket
	*/
	static std::vector<Point> tokenize_brackets(const std::string& src, std::string tok);

	static std::string replaceAllCharacters(std::string&, const std::string&, const std::string&);

	static std::string getFromatTime(int time);

	static std::string int2String(unsigned int id);

	static unsigned int getRandomValule(int startValue, int endValue);

	static std::string getComputeID();
};

void decode(unsigned char* pszFile, unsigned char** pszNew, size_t* size);
bool decodeXXTEA(unsigned char* input, size_t inputSize, unsigned char ** output, size_t* outputSize);

#define CREATE_FUNC_UNRETAIN(__TYPE__) \
	static __TYPE__* create() \
{ \
	__TYPE__ *pRet = new(std::nothrow) __TYPE__(); \
if (pRet && pRet->init()) \
{ \
	return pRet; \
} \
	else \
{ \
	delete pRet; \
	pRet = NULL; \
	return NULL; \
} \
}
