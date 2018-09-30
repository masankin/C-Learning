
//********************************************
//	check dirty word
//********************************************

#ifndef __WORD_CHECKER__
#define __WORD_CHECKER__

#include "cocos2d.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <map>
#include <vector>
#include <algorithm> 
#include <unordered_set>  

USING_NS_CC;
using namespace std;

class WordChecker
{
public:
	WordChecker();
	~WordChecker();

public:

	bool createPinyinMap(const char* fileName);
	bool createDirtyWorldWithPinyin(const char* fileName);
	bool loadsignalCN(const char* fileName);
	string getPinyinWithHanzi(const char* hanzi);
	void addToSetWithFirstChar(char first,std::string wordStr);
	bool sortAllDirtyWorldVector();

public:

	unordered_set<std::string>* getSensitiveVectroWithChineseChar(const char* first);
	unordered_set<std::string>* getSensitiveVectroWithEnglishChar(const char first);
	string replaceSensitiveWord(string _Origin,string _temp,int type);
	string checkAllStr(string str,string _temp,int type);
	string checkStrIsSensitive(string str,string sub,int* subNumArray,string _temp,int type);
	bool checkIsSensitive(string str,std::unordered_set<string>* strs);
	static WordChecker* Get();
	//void OneUnicode2UTF8(const char* unicode_char,size_t unicode_char_length,  
//		char* utf_char);
	vector<int> m_vsensitiveList;
private:

	unordered_set<string> m_vsignalCN;
	map<string,string> m_mapPinyin;
	map<char,unordered_set<string>*> m_mapDirtyWorld;

//引入拼音表，但是对汉字和拼音有大量的消耗，寻找新的解决方法
//将汉语的敏感词作为指针放进hashset× 失败，原因是查找时比较对指针比毛线啊

public:

	bool createDirtyWorldIntoHash(const char* fileName);
	string replaceSensitiveWordWithHash(string _Origin,string _temp,int type);
	bool checkIsSensitiveWithHash(string);

private:

	unordered_set<string> m_hashSensitiveMap;
	vector<string> m_hashSensitiveList;

private:
	class CGarbo // 它的唯一工作就是在析构函数中删除单例的实例 
	{
	public:
		~CGarbo()
		{ 
			if (WordChecker::_wordChecker)
				delete WordChecker::_wordChecker;
		}
	};
	static CGarbo Garbo;
	static WordChecker* _wordChecker;
};

#endif
