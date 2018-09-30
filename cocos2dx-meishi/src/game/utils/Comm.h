#ifndef __COMM_H__
#define __COMM_H__
#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "EnvSet.h"
using namespace std;
USING_NS_CC; 
using namespace ui;

std::vector<std::string> splitStr(std::string str,std::string pattern); 
std::vector<int> splitStrToInt(std::string str,std::string pattern); 

bool parseUTFStr(const std::string& sin, std::vector<std::string>& ret);
std::string getRichFormatText(std::string str);

void setGray(cocos2d::Node* target);
//名字合法性检测
int checkCharacter(const std::string str, std::string &cStr, int numCount = 6);

void playDailyMissionTip(Node* node, bool isShow, Point pos = Point(0.0, 0.0));

class CommFunc
{
public:
	static std::vector<std::string> commSplitStr(std::string str, std::string pattern);
	static std::vector<int> commSplitStrToInt(std::string str, std::string pattern);

	static bool commParseUTFStr(const std::string& sin, std::vector<std::string>& ret);
	static std::string commGetRichFormatText(std::string str);

	static void commSetGray(cocos2d::Node* target);
	//名字合法性检测
	static int commCheckCharacter(const std::string str, std::string &cStr);

	static std::string getStrValue(const std::string source, const char* name);
	static std::string getStrValue(const std::string source, const char* begin, const char* end);

	static __String* randomStrFromPlist(const char *pszFilename);
	static std::string getRandName();

	//调用ep: sgEnvSet("MGAME_SHOW_FPS")
	static int getEnvSet(const char* val);
};

#define sgEnvSet(val) (CommFunc::getEnvSet(val))

#define HeadMaxCount 18


std::string GetPlayerBadgeIconNameById(int id = 1, bool have = false, const std::string& defaultFile = "");

std::string getPicMd5ByUrl(std::string picStr, int type = 1);

namespace FriendSystem
{
	//头像名字数组
	const std::string PlayerIconName[ HeadMaxCount + 1 ] =
	{
		"ui/Icon/Icon_head_bz.png",//shouQ头像容错处理
		"ui/Icon/Icon_head_bz.png",//容错处理
		"ui/Icon/Icon_head_bz.png",
		"ui/Icon/Icon_head_bz1.png",
		"ui/Icon/Icon_head_zz.png",
		"ui/Icon/Icon_head_zz1.png",
		"ui/Icon/Icon_head_cake.png",
		"ui/Icon/Icon_head_cake1.png",
		"ui/Icon/Icon_head_hbb.png",
		"ui/Icon/Icon_head_hbb1.png",
		"ui/Icon/Icon_head_ss.png",
		"ui/Icon/Icon_head_ss1.png",
		"ui/Icon/Icon_head_red.png",
		"ui/Icon/Icon_head_yellow.png",
		"ui/Icon/Icon_head_blue.png",
		"ui/Icon/Icon_head_purple.png",
		"ui/Icon/Icon_head_xm.png",
		"ui/Icon/Icon_head_zs.png",
		"ui/Icon/Icon_head_lj.png"
		/*"ui/Icon/Icon_head_bz.png"//fb头像容错处理*/
	};

	const std::string PlayerGrayIconName[] =
	{
		"ui/Icon/Icon_head_xm_gray.png",
		"ui/Icon/Icon_head_xm_gray.png",
		"ui/Icon/Icon_head_zs_gray.png",
		"ui/Icon/Icon_head_lj_gray.png",
	};

	//根据id获取头像字符串
	const char* GetPlayerIconNameFromId(std::string iconId = "1", const std::string fbicon = "");

	const char* GetPlayerGrayIconNameByVipLevel(int level = 1);

	Sprite* createPlayerIconNameFromId(int iconId = 1, const std::string fbicon = "");

	void setPlayerIconSpr(Sprite* spr, std::string iconId = "1", const std::string fbicon = "");

	void setPlayerIconBtn(ui::Button* spr, std::string iconId = "1", const std::string fbicon = "");

	void setPlayerIconImg(ui::ImageView* spr, std::string iconId = "1");
};
#endif
