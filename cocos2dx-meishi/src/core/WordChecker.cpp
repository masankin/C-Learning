//********************************************
//	check dirty word
//********************************************

#include "WordChecker.h"
#define WORD_MAX_SIZE 10
#define DIRTYWORD_MAX_SIZE 50

#define CHECK_MAX_WORD_EVERYTIME 30
#define CHECK_MIN_WORD_EVERYTIME 15

#define WORD_INPUT_MAX_SIZE 150
#define SIGNAL_CHINESE_SIZE 5

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
static int	PLATFORM_CHINESE_SIZE = 2;						
#else
static int	PLATFORM_CHINESE_SIZE = 3;
#endif

WordChecker* WordChecker::_wordChecker = NULL;
WordChecker::CGarbo WordChecker::Garbo;
static int minMatchTYpe = 1;			//min
static int maxMatchType = 2;			//max

WordChecker::WordChecker()
{
	m_mapPinyin.clear();
	m_mapDirtyWorld.clear();
	m_vsensitiveList.clear();
	m_hashSensitiveMap.clear();
	m_hashSensitiveList.clear();
	m_vsignalCN.clear();
}

WordChecker::~WordChecker()
{
	std::map<char,std::unordered_set<std::string>*>::iterator wordit;
	for ( wordit = m_mapDirtyWorld.begin() ; wordit != m_mapDirtyWorld.end() ; wordit++)
	{
		CC_SAFE_DELETE(wordit->second);
	}

	m_mapPinyin.clear();
	m_mapDirtyWorld.clear();
	m_vsensitiveList.clear();
	m_hashSensitiveMap.clear();
	m_hashSensitiveList.clear();
}

WordChecker* WordChecker::Get()
{
	if(_wordChecker == NULL){
		_wordChecker = new WordChecker();

		_wordChecker->createPinyinMap("pinyin.csv");
		_wordChecker->createDirtyWorldWithPinyin("dirty_word.txt");
		_wordChecker->loadsignalCN("chinesesignal.txt");

	}

	return _wordChecker;
}

bool WordChecker::loadsignalCN(const char* fileName)
{
	std::string path = FileUtils::sharedFileUtils()->fullPathForFilename(fileName);
	__String* pstrFileContent = __String::createWithContentsOfFile(path.c_str());
	unsigned int pstrLen;
	if (pstrFileContent)
	{
		pstrLen = pstrFileContent->length();
	}else{
		pstrLen = -1;
	}

	//file exist & length > 0
	if (pstrLen > 0)
	{
		char* m_strData = (char*)malloc( (size_t)(pstrLen + SIGNAL_CHINESE_SIZE));		//malloc a little more memory to make sure that overflow would not happen when free 
		memset( m_strData, 0, pstrLen );
		strcpy(m_strData, pstrFileContent->getCString());

		char temp[SIGNAL_CHINESE_SIZE] = {0};
		int count = 0;

		char k[SIGNAL_CHINESE_SIZE] = {0};

		int start = PLATFORM_CHINESE_SIZE;

		for (int i = start;i<pstrLen;i++)
		{

			if (m_strData[i]=='\r' || m_strData[i]=='\0')
			{
				//get Pinyin for value
				
				strcpy(k,temp);
				memset(temp,0,SIGNAL_CHINESE_SIZE);
				count = 0;
				m_vsignalCN.insert(k);
				memset(k,0,SIGNAL_CHINESE_SIZE);

				i++;
				continue;
			}


			temp[count] = m_strData[i];
			count++;
		}

		//free memory
		free(m_strData);
		m_strData = NULL;

		return true;
	}else
	{
		return false;
	}

	return false;
}

bool WordChecker::createPinyinMap(const char* fileName)
{
	std::string path = FileUtils::sharedFileUtils()->fullPathForFilename(fileName);
	__String* pstrFileContent = __String::createWithContentsOfFile(path.c_str());
	unsigned int pstrLen;
	if (pstrFileContent)
	{
		pstrLen = pstrFileContent->length();
	}else{
		pstrLen = -1;
	}

	//file exist & length > 0
	if (pstrLen > 0)
	{
		char* m_strData = (char*)malloc( (size_t)(pstrLen + WORD_MAX_SIZE));		//malloc a little more memory to make sure that overflow would not happen when free 
		memset( m_strData, 0, pstrLen );
		strcpy(m_strData, pstrFileContent->getCString());

		char temp[WORD_MAX_SIZE] = {0};
		int count = 0;

		char k[WORD_MAX_SIZE] = {0};
		char v[WORD_MAX_SIZE] = {0};

		for (int i = 0;i<pstrLen;i++)
		{

			if (m_strData[i] == ',')
			{
				//get Chinese char for key
				strcpy(k,temp);
				memset(temp,0,WORD_MAX_SIZE);
				count = 0;
				continue;
			}

			if (m_strData[i]=='\n' || m_strData[i]=='\0' || m_strData[i]=='\r')
			{
				//get Pinyin for value
				strcpy(v,temp);
				memset(temp,0,WORD_MAX_SIZE);
				count = 0;

				//CCLOG("key : %s  @@@  value : %s",k,v);
				m_mapPinyin.insert(std::map<std::string,std::string>::value_type(k,v));

				memset(k,0,WORD_MAX_SIZE);
				memset(v,0,WORD_MAX_SIZE);


				continue;
			}


			temp[count] = m_strData[i];
			count++;
		}

		//free memory
		free(m_strData);
		m_strData = NULL;

		return true;
	}else
	{
		return false;
	}

	return false;
}

bool WordChecker::createDirtyWorldWithPinyin(const char* fileName)
{
	std::string path = FileUtils::sharedFileUtils()->fullPathForFilename(fileName);
	__String* pstrFileContent = __String::createWithContentsOfFile(path.c_str());
	unsigned int pstrLen;
	if (pstrFileContent)
	{
		pstrLen = pstrFileContent->length();
	}else{
		pstrLen = -1;
	}

	//file exist & length > 0
	if (pstrLen > 0)
	{
		char* m_strData = (char*)malloc( (size_t)(pstrLen + WORD_MAX_SIZE) );		//malloc a little more memory to make sure that overflow would not happen when free
		memset( m_strData, 0, pstrLen );
		strcpy(m_strData, pstrFileContent->getCString());
		
		int count = 0;
		char temp[DIRTYWORD_MAX_SIZE] = {0};
		char dirtyWord[DIRTYWORD_MAX_SIZE] = {0};

		int start = PLATFORM_CHINESE_SIZE;

		for (int i = start;i < pstrLen;i++)
		{
			if (m_strData[i]=='\r' || m_strData[i]=='\0' || m_strData[i]=='\n')
			{
				strcpy(dirtyWord,temp);
				memset(temp,0,DIRTYWORD_MAX_SIZE);

				char k;
				char fst = dirtyWord[0];
				if (fst > 127 || fst < 0)
				{//first is Chinese

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
					char chnes[3] = {dirtyWord[0],dirtyWord[1],'\0'};
#else
					char chnes[4] = {dirtyWord[0],dirtyWord[1],dirtyWord[2],'\0'}; //add \0 to make one string
#endif
					

					//CCLOG(" first zi : % s",chnes);
					std::string chi = getPinyinWithHanzi(chnes);
					if ( "" == chi )
					{
						k = '0';
					}
					else
					{
						k = chi.c_str()[0];
					}
				}
				else
				{//first is English
					k = fst;
				}

				//put it in vector
				//make a set
				//CCLOG("dirty word : %s",dirtyWord);
				addToSetWithFirstChar(k,dirtyWord);

				memset(dirtyWord,0,DIRTYWORD_MAX_SIZE);



				count = 0;
				continue;
			}
			temp[count] = m_strData[i];
			count++;
		}

		//free memory
		free(m_strData);
		m_strData = NULL;

		return true;
	}

	return false;
}

std::string WordChecker::getPinyinWithHanzi(const char *hanzi)
{
	if (m_mapPinyin.size() > 0)
	{
		std::map<std::string,std::string>::iterator it;
		it = m_mapPinyin.find(hanzi);

		if (it == m_mapPinyin.end())
		{// not find
			return "";
		}
		else
		{//find
			return it->second;
		}
	}

	return "";
}

void WordChecker::addToSetWithFirstChar(char first,std::string strWord)
{
	if (m_mapDirtyWorld.size() <= 0)
	{
		std::unordered_set<std::string>* v = new std::unordered_set<std::string>();
		v->clear();
		v->insert(strWord);
		m_mapDirtyWorld.insert(std::map<char,std::unordered_set<std::string>*>::value_type(first,v));
		return ;
	}

	std::map<char,std::unordered_set<std::string>*>::iterator it;
	it = m_mapDirtyWorld.find(first);

	if (it == m_mapDirtyWorld.end())
	{//not found & create 
		std::unordered_set<std::string>* v = new std::unordered_set<std::string>();
		v->clear();
		v->insert(strWord);
		m_mapDirtyWorld.insert(std::map<char,std::unordered_set<std::string>*>::value_type(first,v));
		return ;
	}
	else
	{
		it->second->insert(strWord);
		return ;
	}
}

bool WordChecker::sortAllDirtyWorldVector()
{
	if (m_mapDirtyWorld.size() <= 0)
	{
		return false;
	}

	/*std::map<char,std::unordered_set<std::string*>*>::iterator it;


	for (it = m_mapDirtyWorld.begin(); it != m_mapDirtyWorld.end() ; it++)
	{
		std::sort(it->second->begin(),it->second->end());
	}*/

	return true;
}
std::string WordChecker::replaceSensitiveWord(std::string _Origin,std::string _temp,int type)
{
	if(m_mapDirtyWorld.size() <=0 ) return _Origin;
	m_vsensitiveList.clear();
	__String* originContent = __String::create(_Origin);
    int conLength = 1;
	string result = checkAllStr(_Origin,_temp,minMatchTYpe);                       //check all first with min match type


	//CCLOG("@@@@@@@@@@@@@  first check : %s",result.c_str());

	if (originContent)
	{
		conLength = originContent->length();
	}
	else
	{
		conLength = -1;
	}

	//get two strings to check
	//one of these is all English ,another is all Chinese (all no signal)
	if (conLength > 0)
	{
		char * m_strData = (char*)malloc((size_t)(conLength + 1));
		memset(m_strData , 0, conLength);
		strcpy(m_strData , originContent->getCString());

		std::string enStr = "";
		std::string cnStr = "";
		int enNumber[WORD_INPUT_MAX_SIZE] = {0};
		int chNumber[WORD_INPUT_MAX_SIZE] = {0};

        for (int i = 0 ; i < conLength ; i++)
		{
			if ((m_strData[i]>64 && m_strData[i]<91 )||(m_strData[i]>96 && m_strData[i]<123))
			{
				enNumber[enStr.length()] = i;
				enStr += m_strData[i];
			}
			
		}

		//CCLOG("English : %s",enStr.c_str());
		

		result = checkStrIsSensitive(result,enStr,enNumber,_temp,type);                          //second ,check English only 

		//CCLOG("Eng result str : %s",result.c_str());

		const char* firRe = result.c_str();

		for (int i = 0 ; i < result.length() ; i++)
		{
			if (firRe[i] > 127 || firRe[i] <0)
			{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
				char signa[3] = {firRe[i],firRe[i+1],'\0'};
#else
				char signa[4] = {firRe[i],firRe[i+1],firRe[i+2],'\0'};
#endif

				std::unordered_set<std::string>::iterator it = find(m_vsignalCN.begin(),m_vsignalCN.end(),signa);

				if (it != m_vsignalCN.end())
				{
					i += PLATFORM_CHINESE_SIZE;
					i --; 
					continue;
				}else
				{
					for (int j = 0 ; j < PLATFORM_CHINESE_SIZE ; j++)
					{
						chNumber[cnStr.length()] = i+j;
						cnStr += signa[j];
					}
					i += PLATFORM_CHINESE_SIZE;
					i --; 
				}
			}
			
		}
		//result = checkStrIsSensitive(result,engnumStr,engnumNumber,_temp,type;

		result = checkStrIsSensitive(result,cnStr,chNumber,_temp,type);                          //third , check Chinese only
		
		//CCLOG("Chinese : %s",cnStr.c_str());

		/*string engnumStr = "";
		int engnumNumber[WORD_INPUT_MAX_SIZE] = {0};*/

		return result;
	}
	return _Origin;

}

string WordChecker::checkAllStr(string str,string _temp,int type)
{
	string result = str;

	if (str == "")
	{
		return "";
	}

 


	char * m_strData = (char*)malloc((size_t)(str.length() + 1));
    memset( m_strData, 0, str.length() + 1 );
    strcpy(m_strData, str.c_str());
	
    char temp[WORD_INPUT_MAX_SIZE] = {0};			//temp
	int count = 0;									//count
    std::unordered_set<std::string>* strVec;				//for find temp 

	memset(temp,0,WORD_INPUT_MAX_SIZE);
	
	for (int i = 0 ; i < str.length() ; i++)
	{
		if (m_strData[i] > 127 || m_strData[i] <0)
		{//first is Chinese char
	
	
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
			char key[3] = {m_strData[i],m_strData[i+1],'\0'};
#else
			char key[4] = {m_strData[i],m_strData[i+1],m_strData[i+2],'\0'}; //add \0 to make one string
#endif
	
	
					
			strVec = getSensitiveVectroWithChineseChar(key);
		 }
		 else
		 {//first is English char
			char key = m_strData[i];
			strVec = getSensitiveVectroWithEnglishChar(key);
		  }
	
		  int start =  i;
		  int maxCount = 0;
		  bool hasflag = false; 
	
		 for (int j = i ; j < str.length() ; j++ )
		 {

			 temp[count] = m_strData[j];
	
			 if (checkIsSensitive(temp,strVec))
			 {
				 maxCount = count;
				 hasflag = true;
	
	
				 if (type == minMatchTYpe)
				 {
					 break;
				 }
			 }

			 count ++;
			 if (type == minMatchTYpe)
			 {
				 if (count >= CHECK_MIN_WORD_EVERYTIME)
					 break;
			 }
			 else
			 {
				 if (count >= CHECK_MAX_WORD_EVERYTIME)
					 break;
			 }
		 }
	
		 if (hasflag)
		 {
			 m_vsensitiveList.push_back(start*100 + maxCount);
			 i += maxCount;
		 }
	
		 count  = 0;
		 maxCount = 0;

		 memset(temp,0,WORD_INPUT_MAX_SIZE);
				
				
	}
	
	std::vector<int> strsss = m_vsensitiveList;
	for (int i = 0; i < strsss.size(); i++)
	{
	    string tempStr = "";
		for (int t = 0; t < strsss.at(i)%100+1 ; t++)
		{
			tempStr += _temp;
		}
		result = result.replace((int)(strsss.at(i)/100),strsss.at(i)%100+1,tempStr);
		//CCLOG("aaaa = %s",strsss.at(i).c_str());
	}
	
	free(m_strData);
	m_strData = NULL;
	
	return result;



}

string WordChecker::checkStrIsSensitive(string str,string sub,int* subNumArray,string _temp,int type)
{
	string result = str;
	const char*  m_CharArray = sub.c_str(); 
	std::unordered_set<std::string>* strVec;				//for find temp 
	for (int i = 0; i < sub.length(); i++)
	{
	    bool hasflag = false; 


		//get Set with first char
		if (m_CharArray[i] > 127 || m_CharArray[i] <0)
		{//first is Chinese

	   
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
		     char key[3] = {m_CharArray[i],m_CharArray[i+1],'\0'};
#else
	         char key[4] = {m_CharArray[i],m_CharArray[i+1],m_CharArray[i+2],'\0'}; //add \0 to make one string
#endif
	
			 strVec = getSensitiveVectroWithChineseChar(key);
		}
		else
		{//first is English char
			 char key = m_CharArray[i];
			 strVec = getSensitiveVectroWithEnglishChar(key);
		}


		//string will be checked

		int maxCount = 0;
		std::string willCheckStr = "";
		for (int j = i; j < sub.length(); j++)
		{
			willCheckStr += m_CharArray[j];
			if (checkIsSensitive(willCheckStr,strVec))
			{
				hasflag = true;
				maxCount = willCheckStr.length();

				if (type == minMatchTYpe)
				{//min match type
				  
					break;
				}
			}

			if (type == minMatchTYpe)
			{
				if (j - i >= CHECK_MIN_WORD_EVERYTIME)
	  			break;
		   }
		   else
		   {
				if (j - i >= CHECK_MAX_WORD_EVERYTIME)
				break;
		   }
		}

		if (hasflag)
		{
			std::string tempStr = "";
			for (int t = 0 ; t < maxCount;t++)
			{
				tempStr += _temp;
				if (t >= 5)
				{
					break;
				}
			}
			result = result.replace(subNumArray[i],subNumArray[maxCount+i - 1] - subNumArray[i] +1,tempStr);
			CCLOG("start = %d ; end = %d",subNumArray[i],subNumArray[maxCount+i - 1]);
			i = i + willCheckStr.length() - 1;    //for will ++,so -1
		}
	}

	
	m_CharArray = NULL;
	return result;
	//	if (conLength > 0)
	//	{
	//		char * m_strData = (char*)malloc((size_t)(conLength + 1));
	//		memset( m_strData, 0, conLength );
	//		strcpy(m_strData, originContent->getCString());
	//
	//		char temp[WORD_INPUT_MAX_SIZE] = {0};			//temp
	//		char willCheck[WORD_INPUT_MAX_SIZE] = {0};      //will be check with Chinese (no signal)
	//		char enWillCheck[WORD_INPUT_MAX_SIZE] = {0};    //will be check with English (no signal)
	//		int count = 0;									//count
	//		int checkCount = 0;
	//		int enCheckCount = 0;
	//		std::unordered_set<std::string>* strVec;				//for find temp 
	//
	//		memset(willCheck,0,WORD_INPUT_MAX_SIZE);
	//		memset(temp,0,WORD_INPUT_MAX_SIZE);
	//		memset(enWillCheck,0,WORD_INPUT_MAX_SIZE);
	//
	//		for (int i = 0 ; i < conLength ; i++)
	//		{
	//			if (m_strData[i] > 127 || m_strData[i] <0)
	//			{//first is Chinese char
	//
	//
	//#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	//				char key[3] = {m_strData[i],m_strData[i+1],'\0'};
	//#else
	//				char key[4] = {m_strData[i],m_strData[i+1],m_strData[i+2],'\0'}; //add \0 to make one string
	//#endif
	//
	//
	//				
	//				strVec = getSensitiveVectroWithChineseChar(key);
	//			}
	//			else
	//			{//first is English char
	//				char key = m_strData[i];
	//				strVec = getSensitiveVectroWithEnglishChar(key);
	//			}
	//
	//			int start =  i;
	//			int maxCount = 0;
	//			int breakCount = 0;
	//			bool hasflag = false; 
	//
	//			for (int j = i ; j < conLength ; j++ )
	//			{
	//
	//				if ((m_strData[j] < 0  || m_strData[j] > 127 )&& breakCount == 0)  //all Chinese get 
	//				{
	//#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	//					char signa[3] = {m_strData[j],m_strData[j+1],'\0'};
	//#else
	//					char signa[4] = {m_strData[j],m_strData[j+1],m_strData[j+2],'\0'};
	//#endif
	//					std::unordered_set<std::string>::iterator it = find(m_vsignalCN.begin(),m_vsignalCN.end(),signa);
	//
	//					if (it != m_vsignalCN.end())
	//					{
	//						breakCount = PLATFORM_CHINESE_SIZE - 1;
	//
	//						continue;
	//					}
	//
	//					willCheck[checkCount] = m_strData[j];
	//					checkCount ++;
	//				}
	//
	//				if ((m_strData[j]>64 && m_strData[j]<91 )||(m_strData[j]>96 && m_strData[j]<123))
	//				{
	//					enWillCheck[enCheckCount] = m_strData[j];
	//					enCheckCount ++;
	//				}
	//
	//				if (breakCount != 0)
	//				{
	//					breakCount -- ;
	//				}
	//
	//				temp[count] = m_strData[j];
	//
	//				if (checkIsSensitive(temp,strVec) || checkIsSensitive(enWillCheck,strVec))
	//				{
	//					maxCount = count;
	//					hasflag = true;
	//
	//
	//					if (type == minMatchTYpe)
	//					{
	//						break;
	//					}
	//				}
	//
	//				if (checkIsSensitive(willCheck,strVec))
	//				{
	//					maxCount = count + PLATFORM_CHINESE_SIZE - 1;
	//					hasflag = true;
	//
	//					if (type == minMatchTYpe)
	//					{
	//						break;
	//					}
	//				}
	//
	//				count ++;
	//				if (type == minMatchTYpe)
	//				{
	//					if (count >= CHECK_MIN_WORD_EVERYTIME)
	//						break;
	//				}
	//				else
	//				{
	//					if (count >= CHECK_MAX_WORD_EVERYTIME)
	//						break;
	//				}
	//			}
	//
	//			if (hasflag)
	//			{
	//				m_vsensitiveList.push_back(start*100 + maxCount);
	//				i += maxCount;
	//			}
	//
	//			count  = 0;
	//			checkCount = 0;
	//			maxCount = 0;
	//			breakCount = 0;
	//			enCheckCount = 0;
	//			memset(enWillCheck,0,WORD_INPUT_MAX_SIZE);
	//			memset(temp,0,WORD_INPUT_MAX_SIZE);
	//			memset(willCheck,0,WORD_INPUT_MAX_SIZE);
	//			
	//		}
	//
	//		std::vector<int> strsss = m_vsensitiveList;
	//		std::string str = _Origin;
	//		for (int i = 0; i < strsss.size(); i++)
	//		{
	//			str = str.replace((int)(strsss.at(i)/100),strsss.at(i)%100+1,_temp);
	//			//CCLOG("aaaa = %s",strsss.at(i).c_str());
	//		}
	//
	//		free(m_strData);
	//		m_strData = NULL;
	//
	//		return str;
	//	}
}
std::unordered_set<std::string>* WordChecker::getSensitiveVectroWithEnglishChar(const char first)
{
	std::map<char,std::unordered_set<std::string>*>::iterator it;
	it = m_mapDirtyWorld.find(first);

	if (it == m_mapDirtyWorld.end())
	{// not find
		it = m_mapDirtyWorld.find('0');
	}
	
	return it->second; 

}

std::unordered_set<std::string>* WordChecker::getSensitiveVectroWithChineseChar(const char* first)
{
	std::string str = getPinyinWithHanzi(first);

	char k;
	if ( "" == str )
	{
		k = '0';
	}
	else
	{
		k = str.c_str()[0];
	}

	return getSensitiveVectroWithEnglishChar(k);
}


bool WordChecker::checkIsSensitive(std::string str,std::unordered_set<std::string>* strs)
{
	if (strs->size() <= 0 )
	{
		return false;
	}


	std::unordered_set<std::string>::iterator it = find(strs->begin(),strs->end(),str);

	if (it == strs->end())
	{
		return false;
	}
	else
	{
		return true;
	}
	
	
}
//void WordChecker::OneUnicode2UTF8(const char* unicode_char,size_t unicode_char_length,  
//					 char* utf_char)  
//{  
//	//unicode: 0x192->110010010 ,utf8:0xC692->1100011010010010  
//	int value = 0;  
//	memcpy(&value,unicode_char,unicode_char_length);  
//
//	if (value >= 0x0000 && value <= 0x007F)    
//	{    
//		utf_char[0] = unicode_char[0];  
//	}    
//	else if (value >= 0x0080 && value <= 0x07FF)    
//	{    
//		utf_char[0] = ((value >> 6) | 0xC0);  
//		utf_char[1] = ((value & 0x3F) | 0x80);  
//	}    
//	else if (value >= 0x0800 && value <= 0xFFFF)    
//	{    
//		utf_char[0] = ((value >> 12) | 0xE0);  
//		utf_char[1] = ((value >> 6 & 0x3F) | 0x80);  
//		utf_char[2] = ((value & 0x3F) | 0x80);  
//	}
//	else if (value >= 0x10000 && value <= 0x10FFFF)    
//	{    
//		utf_char[0] = (value >> 18 | 0xF0);  
//		utf_char[1] = ((value >> 12 & 0x3F) | 0x80);  
//		utf_char[2] = ((value >> 6 & 0x3F) | 0x80);  
//		utf_char[3] = ((value & 0x3F) | 0x80);  
//	}  
//	else  
//	{  
//		assert(0);  
//	}  
//}  




bool WordChecker::createDirtyWorldIntoHash(const char* fileName)
{
	std::string path = CCFileUtils::sharedFileUtils()->fullPathForFilename(fileName);
	CCString* pstrFileContent = CCString::createWithContentsOfFile(path.c_str());
	unsigned int pstrLen;
	if (pstrFileContent)
	{
		pstrLen = pstrFileContent->length();
	}else{
		pstrLen = -1;
	}

	//file exist & length > 0
	if (pstrLen > 0)
	{
		char* m_strData = (char*)malloc( (size_t)(pstrLen + WORD_MAX_SIZE) );		//malloc a little more memory to make sure that overflow would not happen when free
		memset( m_strData, 0, pstrLen );
		strcpy(m_strData, pstrFileContent->getCString());

		int count = 0;
		char temp[DIRTYWORD_MAX_SIZE] = {0};
		char dirtyWord[DIRTYWORD_MAX_SIZE] = {0};

		for (int i = 3;i < pstrLen;i++)
		{
			if (m_strData[i]=='\r' || m_strData[i]=='\0' || m_strData[i]=='\n')
			{
				strcpy(dirtyWord,temp);
				memset(temp,0,DIRTYWORD_MAX_SIZE);

				if (dirtyWord[0] > 127 || dirtyWord[0] <0)
				{
					m_hashSensitiveMap.insert(dirtyWord);
				}
				
				memset(dirtyWord,0,DIRTYWORD_MAX_SIZE);


				count = 0;
				continue;
			}
			temp[count] = m_strData[i];
			count++;
		}

		//free memory
		free(m_strData);
		m_strData = NULL;

		return true;
	}

	return false;
}

bool WordChecker::checkIsSensitiveWithHash(std::string str)
{
	if (m_hashSensitiveMap.size() <= 0 )
	{
		return false;
	}


	std::unordered_set<std::string>::iterator it = find(m_hashSensitiveMap.begin(),m_hashSensitiveMap.end(),str);

	if (it == m_hashSensitiveMap.end())
	{
		return false;
	}
	else
	{
		return true;
	}
}

std::string WordChecker::replaceSensitiveWordWithHash(std::string _Origin,std::string _temp,int type)
{
	m_hashSensitiveList.clear();
	CCString* originContent = CCString::create(_Origin);
	unsigned int conLength;

	if (originContent)
	{
		conLength = originContent->length();
	}
	else
	{
		conLength = -1;
	}

	if (conLength > 0)
	{
		char * m_strData = (char*)malloc((size_t)(conLength + 1));
		memset( m_strData, 0, conLength );
		strcpy(m_strData, originContent->getCString());
		char temp[WORD_INPUT_MAX_SIZE] = {0};			//temp
		int count = 0;									//count
		std::unordered_set<std::string>* strVec;				//for find temp 

		memset(temp,0,WORD_INPUT_MAX_SIZE);

		for (int i = 0 ; i < conLength ; i++)
		{
			
			char maxOut[WORD_INPUT_MAX_SIZE] = {0};
			int maxCount = 0;
			bool hasflag = false; 

			for (int j = i ; j < conLength ; j++ )
			{
				temp[count] = m_strData[j];

				if (checkIsSensitiveWithHash(temp))
				{
					memset( maxOut, 0, WORD_INPUT_MAX_SIZE );
					strcpy(maxOut, temp);
					maxCount = count;
					hasflag = true;


					if (type == minMatchTYpe)
					{
						break;
					}
				}
				count ++;
			}

			if (hasflag)
			{
				m_hashSensitiveList.push_back(maxOut);
				i += maxCount;
			}

			count  = 0;
			maxCount = 0;
			memset(temp,0,WORD_INPUT_MAX_SIZE);
			memset( maxOut, 0, WORD_INPUT_MAX_SIZE );
		}

		std::string str = _Origin;
		for (int i = 0; i < m_hashSensitiveList.size(); i++)
		{
			str = str.replace(str.find_first_of(m_hashSensitiveList.at(i)),m_hashSensitiveList.at(i).length(),_temp);
			//CCLOG("aaaa = %s",strsss.at(i).c_str());
		}

		return str;
	}

	return _Origin;
}
