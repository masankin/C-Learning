/*
* 该文件定义一些宏开关，只在发布版本时使用。
* 【务必保持本文件清晰】
*/

#pragma once


#define MGAME_IN_APP_SDK	1		//	打开sdk
//#define  MGAME_PLATFORM_MPLUS		//打开mplus环境
#define MGAME_TENCENT_PLATFORM		//QQ,微信渠道
#define PROTOBUF_CODE	1				// 协议版本号

//以下宏开关 请前往config.lua修改

//#define MGAME_SHOW_FPS  1			//显示帧率
//#define MGAME_OPEN_ALL_CP 1		// 打开所有关卡
//#define MGAME_POWER_OPEN 1
//#define MGAME_CUR_CLOSED			// 功能暂未开放
//#define MD5_CHECK					//打开签名md5验证功能
//#define MGAME_OPEN_BATTLE_RECORD	//打开战斗录制功能(只有给策划君用的时候才打开)
//#define  MGAME_OPEN_PVP_ROOM		//去除PVP限制
//#define STOP_SERVER_WHITE_LIST
//#define MGAME_CHAT_COLSE			//聊天关闭

//获取常量
//#define PVP_RAND_LEVEL 1090		//随机模式pve限制
//#define PVP_ROOM_LEVEL 1190		//开房模式pve限制
//#define PVE_CHAT_LEVEL 1220		//pve聊天显示pve限制
//#define MAIN_CHAT_LEVEL 1070	   //全服聊天pve限制


// 环境开关
class EnvSet
{
public:
	static bool jacey() { return false; }
	static bool alvin(){return false;}

	static bool sovio(){ return false; }
	static bool leon(){ return false; }

	static bool airel(){ return false; }

	static bool evix() { return false; }
	static bool junior() { return false; }
};


class AlvinTest
{
public:
	//关闭新手引导
	static bool alvin_closeGuide(){ return false; }
	//关闭打野
	static bool alvin_closeJungle(){return false;}
	//自由控制阵营
	static bool alvin_freeControl(){ return false; }

	//测试士气
	static bool alvin_testMorale(){ return false; }

	//关闭buff
	static bool alvin_closeBuff(){return false;}

	~AlvinTest(){}; 

};

		
