#include "Comm.h"
#include "Constant.h" 
#include "game/data/TableManager.h"
#include "tolua_fix.h"
#include "CCLuaEngine.h"
#include "update/md5.h"
#if CC_PLATFORM_ANDROID ==  CC_TARGET_PLATFORM
#include "platform/android/jni/ChannelJni.h"
#endif
//字符串分割函数
std::vector<std::string> splitStr(std::string str,std::string pattern)
{
	std::string::size_type pos;
	std::vector<std::string> result;
	str+=pattern;//扩展字符串以方便操作
	int size=str.size();

	for(int i=0; i<size; i++)
	{
		pos=str.find(pattern,i);
		if(pos<size)
		{
			std::string s=str.substr(i,pos-i);
			result.push_back(s);
			i=pos+pattern.size()-1;
		}
	}
	return result;
}

std::vector<int> splitStrToInt(std::string str,std::string pattern)
{
	std::string::size_type pos;
	std::vector<int> result;
	str+=pattern;//扩展字符串以方便操作
	int size=str.size();

	for(int i=0; i<size; i++)
	{
		pos=str.find(pattern,i);
		if(pos<size)
		{
			std::string s=str.substr(i,pos-i);
			result.push_back(atoi(s.c_str()));
			i=pos+pattern.size()-1;
		}
	}
	return result;
}


void setGray(cocos2d::Node* target)
{
	//注意图片是不是九宫格类型，fuck――cocostudio，九宫格类型传入会失效
	auto p = ShaderCache::getInstance()->getGLProgram(GLProgram::SHADER_ArmatureGray);
	//auto p = ShaderCache::getInstance()->getGLProgram(GLProgram::SHADER_NAME_POSITION_GRAYSCALE);
	if (target)
	{
		target->setGLProgram(p);
	}
}
/*
* 解析utf8文本sin的每个字符到ret中
* 返回true表示解析成功
*/
bool parseUTFStr(const std::string& sin, std::vector<std::string>& ret) {
	int l = sin.length();
	ret.clear();
	for (int p = 0; p < l;) {
		int size, n = l - p;
		unsigned char c = sin[p], cc = sin[p + 1];
		if (c < 0x80) {
			size = 1;
		}
		else if (c < 0xc2) {
			return false; //invalid seq
		}
		else if (c < 0xe0) {
			if (n < 2) {
				return false; //missing seq
			}
			if (!((sin[p + 1] ^ 0x80) < 0x40)) {
				return false; //invalid seq
			}
			size = 2;
		}
		else if (c < 0xf0) {
			if (n < 3) {
				return false; //missing seq
			}
			if (!((sin[p + 1] ^ 0x80) < 0x40 &&
				(sin[p + 2] ^ 0x80) < 0x40 &&
				(c >= 0xe1 || cc >= 0xa0))) {
				return false; //invalid seq
			}
			size = 3;
		}
		else if (c < 0xf8) {
			if (n < 4) {
				return false; //missing seq
			}
			if (!((sin[p + 1] ^ 0x80) < 0x40 &&
				(sin[p + 2] ^ 0x80) < 0x40 &&
				(sin[p + 3] ^ 0x80) < 0x40 &&
				(c >= 0xf1 || cc >= 0x90))) {
				return false; //invalid seq
			}
			size = 4;
		}
		else if (c < 0xfc) {
			if (n < 5) {
				return false; //missing seq
			}
			if (!((sin[p + 1] ^ 0x80) < 0x40 &&
				(sin[p + 2] ^ 0x80) < 0x40 &&
				(sin[p + 3] ^ 0x80) < 0x40 &&
				(sin[p + 4] ^ 0x80) < 0x40 &&
				(c >= 0xfd || cc >= 0x88))) {
				return false; //invalid seq
			}
			size = 5;
		}
		else if (c < 0xfe) {
			if (n < 6) {
				return false; //missing seq
			}
			if (!((sin[p + 1] ^ 0x80) < 0x40 &&
				(sin[p + 2] ^ 0x80) < 0x40 &&
				(sin[p + 3] ^ 0x80) < 0x40 &&
				(sin[p + 4] ^ 0x80) < 0x40 &&
				(sin[p + 5] ^ 0x80) < 0x40 &&
				(c >= 0xfd || cc >= 0x84))) {
				return false; //invalid seq
			}
			size = 6;
		}
		else {
			return false; //invalid seq
		}
		std::string temp = "";
		temp = sin.substr(p, size);
		ret.push_back(temp);
		p += size;
	}
	return true; //解析成功
}

//返回0 不合法 返回1合法 返回非1非0表示截取长度
int checkCharacter(const std::string str, std::string &cStr, int numCount)
{
	if (str.empty()) return 0;

	int idx = str.find(' ');
	if (idx != -1)return 0;

	std::string checkStr = ".'\"%,=;$&*\\+";

	int len = 0;
	int length = str.length();

	for (unsigned int i = 0; i < length; i++)
	{
		if (cStr.empty())
		{
			int idx = checkStr.find(str.at(i));
			if (idx != -1)
			{
				cStr = str.at(i);
			}
		}		
		int j = i + 1;
		if (str.at(i) < 33 || str.at(i) > 126)
			len += 1;//汉字
		if (len)
		{
			int num = (j - len) + (len / 3);
			if (num >= numCount){
				if (j >= length){
					return 1;
				}
				return j;
			}
		}
		else if (j > numCount * 3 / 2)
		{
			return i;
		}
	}
	if (cStr.empty())
		return 1;
	else
		return 0;
}

void playDailyMissionTip(Node* node, bool isShow, Point pos)
{
	auto missionTip = node->getChildByTag(1000);
	if (isShow)
	{
		if (!missionTip)
		{
			cocostudio::Armature* pTipArm = cocostudio::Armature::create("fla_RenwuJT");
			pTipArm->getAnimation()->play("play", -1, 1);
			pTipArm->setPosition(pos);
			node->addChild(pTipArm, 1000, 1000);
		}
		else
		{
			missionTip->setPosition(pos);
			missionTip->setVisible(true);
		}
	}
	else
	{
		if (missionTip)
			missionTip->setVisible(false);
	}
}

std::string getRichFormatText(std::string str)
{
	string text = "";
	vector<std::string>strList = splitStr(str, "#");

	for (int i = 0; i < strList.size(); i++)
	{
		RichHelp::pTextDefnition defnition;

		string str00 = strList[i];

		if (i % 2 == 0)
		{
			if (str00.empty())
				continue;

			defnition.text = str00;
		}
		else
		{
			vector<string> strList1 = splitStr(str00, "]");

			defnition.text = strList1[1];

			string str11 = strList1[0];
			str11 = str11.substr(1, str11.length());
			vector<string> strList2 = splitStr(str11, "_");


			for (int j = 0; j < strList2.size(); ++j)
			{
				string temp = strList2[j];
				if (temp.empty())
					continue;

				vector <string>tempList = splitStr(temp, "=");
				if (tempList.size() != 2)
					continue;

				if (temp.find("face") != std::string::npos)
				{
					defnition.fntName = tempList[1];
				}
				else if (temp.find("size") != std::string::npos)
				{
					defnition.fntSize = atoi(tempList[1].c_str());
				}
				else if (temp.find("color") != std::string::npos)
				{
					defnition.fntColor = "#" + tempList[1];
				}
			}
		}
		text += StringUtils::format(RICH_TEXT_ALL, defnition.fntName.c_str(), defnition.fntSize, defnition.fntColor.c_str(), defnition.text.c_str());
	}
	return text;
}


//////////////////////////////////
std::vector<std::string> CommFunc::commSplitStr(std::string str, std::string pattern)
{
	return splitStr(str, pattern);
}

std::vector<int> CommFunc::commSplitStrToInt(std::string str, std::string pattern)
{
	return splitStrToInt(str, pattern);
}

bool CommFunc::commParseUTFStr(const std::string& sin, std::vector<std::string>& ret)
{
	return parseUTFStr(sin, ret);
}

std::string CommFunc::commGetRichFormatText(std::string str)
{
	return getRichFormatText(str);
}

void CommFunc::commSetGray(cocos2d::Node* target)
{
	setGray(target);
}
//名字合法性检测
int CommFunc::commCheckCharacter(const std::string str, std::string &cStr)
{
	return checkCharacter(str, cStr);
}

std::string CommFunc::getStrValue(const std::string source, const char* name)
{
	if (source == "")
		return "";
	std::string myName = "@@";
	myName += name;
	myName += ":";
	if (name == NULL)
		return "";

	int first = source.find(myName);
	if (first == -1)
		return "";


	first += myName.length();
	int end = source.find("@@", first);

	if (end == -1)
		return "";

	std::string temp = "";
	temp = source.substr(first, end - first);
	return temp;
}

std::string CommFunc:: getStrValue(const std::string source, const char* begin, const char* end)
{
	if (source == "")
		return "";
	std::string beginStr = begin;
	int first = source.find(begin);
	if (first == -1)
		return "";

	first += beginStr.length();

	int first1 = source.find(end, first);
	if (first1 == -1)
		return "";

	std::string temp = "";
	temp = source.substr(first, first1 - first);

	return temp;
}


//从plist里面随机读取字符串
__String* CommFunc::randomStrFromPlist(const char *pszFilename)
{

	std::string nameFile = FileUtils::sharedFileUtils()->fullPathForFilename(pszFilename);
	__Dictionary * nameData = __Dictionary::createWithContentsOfFileThreadSafe(nameFile.c_str());

	if (nameData)
	{
		int randomKey = rand() % nameData->count();

		std::ostringstream randomNameOss;
		randomNameOss << randomKey;

		__String* nameStr = (__String*)nameData->objectForKey(randomNameOss.str());
		nameData->autorelease();

		return nameStr;
	}
	return NULL;
}

std::string CommFunc::getRandName()
{
	int iSex = rand() % 2;
	std::string sexStr = "man";

	if (iSex == 1) {
		sexStr = "woman";
	}

	//姓
	std::string firstName = sexStr + "_first_name.plist";
	__String* firstNameStr = randomStrFromPlist(firstName.c_str());

	firstName = firstNameStr == NULL ? "" : firstNameStr->getCString();

	//名字
	std::string lastName = sexStr + "_last_name.plist";
	__String* secondNameStr = randomStrFromPlist(lastName.c_str());

	lastName = secondNameStr == NULL ? "" : secondNameStr->getCString();

	char nameText[64] = {NULL,};
	sprintf(nameText, "%s%s", firstName.c_str(), lastName.c_str());
	return std::string(nameText);
}

int CommFunc::getEnvSet(const char* val) {

	lua_State* pL = LuaEngine::getInstance()->getLuaStack()->getLuaState();
	lua_getglobal(pL, "getEnvSet");
	lua_pushstring(pL, val);
	lua_call(pL, 1, 1);
	int iResult = lua_tonumber(pL, -1);
	CCLOG("[tarce]CommFunc::getEnvSet %s iResult = %d", val, iResult);
	return iResult;
}

///////////////////////////////////
std::string GetPlayerBadgeIconNameById(int id/* = 1*/, bool have/* = false*/, const std::string& defaultFile/* = ""*/)
{
	if (nullptr != sgTableManager()->getBadgeData(id))
	{
		if (have)
			return StringUtils::format("ui/Icon2/BadgeIcon%02d.png", id);
		else
			return StringUtils::format("ui/Icon2/BadgeIcon%02dGray.png", id);
	}
	else
	{
		return defaultFile;
	}
}


std::string getPicMd5ByUrl(std::string picStr, int type)
{
	MD5 md5;
	md5.update(picStr.c_str(),picStr.length());
	md5.finalize();
	std::string dMd5Str = md5.hexdigest();
	if (type == 1)
		dMd5Str.append("_circle.png");
	else
		dMd5Str.append(".png");
	return dMd5Str;
}

namespace FriendSystem
{
	const char* GetPlayerIconNameFromId(std::string iconId, const std::string fbicon)
	{
		int iIcon = std::atoi(iconId.c_str());
		if (iIcon == HeadMaxCount && !fbicon.empty())//fb头像对应id
			return fbicon.c_str();

		if (iIcon == 0 && iconId.size() > 1)//url图片
		{
			std::string liconStr = getPicMd5ByUrl(iconId);
			CCLOG("[trace] GetPlayerIconNameFromId getPicMd5ByUrl iconStr = %s",liconStr.c_str());
			std::string fullpath = FileUtils::getInstance()->fullPathForFilename(liconStr);
			if (fullpath.size() == 0)
			{
#if CC_PLATFORM_ANDROID ==  CC_TARGET_PLATFORM
				onLoadUrlImage(iconId);
#endif
				return "";
			}
			return liconStr.c_str();
		}
		if (iIcon <= HeadMaxCount)
		{
			return PlayerIconName[iIcon].c_str();
		}
		return "";
	}

	const char* GetPlayerGrayIconNameByVipLevel(int level)
	{
		return PlayerGrayIconName[level].c_str();
	}

	Sprite* createPlayerIconNameFromId(int iconId, const std::string fbicon)
	{
		Sprite* spr = nullptr;

		if (iconId == HeadMaxCount && !fbicon.empty())
		{
			spr = Sprite::create(fbicon);		
		}	
		else
		{ 
			spr = Sprite::createWithSpriteFrameName(PlayerIconName[iconId]);
		}
		return spr;
	}

	void setPlayerIconSpr(Sprite* spr, std::string iconId,const std::string fbicon)
	{
		int iIcon = std::atoi(iconId.c_str());
		if (iIcon == HeadMaxCount && !fbicon.empty())
		{
			spr->setTexture(fbicon);
		}
		if (iIcon == 0 && iconId.size() > 1)//url图片
		{
			std::string liconStr = getPicMd5ByUrl(iconId);
			spr->setTexture(liconStr);
		}
		else
		{
			spr->setSpriteFrame(PlayerIconName[iIcon]);
		}
	}

	void setPlayerIconBtn(ui::Button* spr, std::string iconId, const std::string fbicon)
	{
		int iIcon = std::atoi(iconId.c_str());
		if (iIcon == HeadMaxCount && !fbicon.empty())
		{		
			spr->loadTextureNormal(fbicon, cocos2d::ui::Widget::TextureResType::LOCAL);
		}
		else
		{
			if (iIcon == 0 && iconId.size() > 1)//url图片
			{
				std::string liconStr = getPicMd5ByUrl(iconId);
				spr->loadTextureNormal(liconStr);
			}
			else
			{
				std::string str = PlayerIconName[iIcon];
				spr->loadTextureNormal(str, cocos2d::ui::Widget::TextureResType::PLIST);
			}
			
		}
	}

	void setPlayerIconImg(ui::ImageView* img, std::string iconId)
	{
		int iIcon = std::atoi(iconId.c_str());

		if (iIcon == 0 && iconId.size() > 1)//url图片
		{
			std::string liconStr = getPicMd5ByUrl(iconId);
			img->loadTexture(liconStr);
		}
		else
		{
			img->loadTexture(PlayerIconName[iIcon],Widget::TextureResType::PLIST);
		}
	}
}
