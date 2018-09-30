/************************************************* 
Copyright:DataEye 
Author: xqwang
Date:2014-09-24 
Description:SDK初始化接口，android平台该类在C++侧只对外
提供一个接口，其它接口需要开发者在java侧调用    
**************************************************/  

#ifndef __DATAEYE_DCAGENT_H__
#define __DATAEYE_DCAGENT_H__
#include "DCAccountType.h"
#include "DCTaskType.h"
#include "DCGender.h"
#include <map>
#include <string>
#include "cocos2d.h"
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) 
#include <jni.h>
#endif

using namespace std;

class DCAgent
{
public: 
	/************************************************* 
	* Description: 自定义版本号
	* version    : 版本号	
	*************************************************/ 
	static void setVersion(const char* version);
	
	/************************************************* 
	* Description: 自定义错误接口
	* title      : 错误名
    * error      : 错误内容，建议传入错误堆栈信息	
	*************************************************/ 
	static void reportError(const char* title, const char* error);
	
	/*************************************************
     * Description: 立即上报数据，如用户非常在意一些数据，希望
                    其立即上报时，可以调用该接口
     *************************************************/
    static void uploadNow();
    
    /*************************************************
     * Description: 获取当前设备UID
     * return     : UID
     *************************************************/
    static const char* getUID();
};

//玩家信息接口
class DCAccount
{
public:
	/************************************************* 
	* Description: 玩家登陆，在DC_AFTER_LOGIN模式下使用
	* accountId  : 玩家账号ID，该参数请保证全局唯一   
	*************************************************/ 
    static void login(const char* accountId);
	
	/************************************************* 
	* Description: 玩家登陆，在DC_AFTER_LOGIN模式下使用
	* accountId  : 玩家账号ID，该参数请保证全局唯一
	* gameServer : 玩家账号所在区服	
	*************************************************/ 
    static void login(const char* accountId, const char* gameServer);
    
	/************************************************* 
	* Description: 玩家登出，在DC_AFTER_LOGIN模式下使用
	*************************************************/ 
    static void logout();
    
    /*************************************************
     * Description: 获取当前用户ID
     * return     : 用户ID
     *************************************************/
    static const char* getAccountId();

	/************************************************* 
	* Description: 设置玩家账号类型
	* accountType: 玩家账号类型，值为DCAccountType中的枚举   
	*************************************************/ 
    static void setAccountType(DCAccountType accountType);

	/************************************************* 
	* Description: 设置玩家等级
	* level      : 玩家账号等级   
	*************************************************/
    static void setLevel(int level);

	/************************************************* 
	* Description: 设置玩家性别
	* gender     : 玩家性别   
	*************************************************/ 
    static void setGender(DCGender gender);

	/************************************************* 
	* Description: 设置玩家年龄
	* age        : 玩家年龄   
	*************************************************/
    static void setAge(int age);

	/************************************************* 
	* Description: 设置玩家所在区服
	* gameServer : 玩家账号所在区服   
	*************************************************/
    static void setGameServer(const char* gameServer);
	
    /*************************************************
     * Description: 给玩家打标签，用户SDK的后续推送功能
     * tag        : 一级标签
     * subTag     : 二级标签
     *************************************************/
    static void addTag(const char* tag, const char* subTag);
    
    /*************************************************
     * Description: 取消玩家标签，用户SDK的后续推送功能
     * tag        : 一级标签
     * subTag     : 二级标签
     *************************************************/
    static void removeTag(const char* tag, const char* subTag);
};

//付费接口
class DCVirtualCurrency
{
public:
	
	/************************************************* 
	* Description: 付费接口，支付SDK付费成功回调时调用
	* orderId       : 订单ID
	* iapId         : 礼包ID
    * currencyAmount: 付费金额
	* currencyType  : 付费币种
	* paymentType   : 付费途径
	*************************************************/
	static void paymentSuccess(const char* orderId, const char* iapId, double currencyAmount, const char* currencyType, const char* paymentType);
    
    /*************************************************
     * Description: 付费接口，用于当前上报关卡内付费数据，支付SDK付费成功回调时调用
     * orderId       : 订单ID
	 * iapId         : 礼包ID
     * currencyAmount: 付费金额
     * currencyType  : 付费币种
     * paymentType   : 付费途径
     * levelId       : 关卡ID
     *************************************************/
    static void paymentSuccessInLevel(const char* orderId, const char* iapId, double currencyAmount, const char* currencyType, const char* paymentSuccess, const char* levelId);
};

//道具接口
class DCItem
{
public:
	/************************************************* 
	* Description: 购买道具
	* itemId    : 道具ID
    * itemType  : 道具类型
	* itemCount : 购买的道具数量
	* virtualCurrency:购买的道具的虚拟价值
	* currencyType:支付方式
	* consumePoint:消费点，如关卡内消费，可以为空
	*************************************************/
    static void buy(const char* itemId, const char* itemType, int itemCount, long long virtualCurrency, const char* currencyType, const char* consumePoint);
    
    /*************************************************
     * Description: 购买道具，用于关卡内购买道具事件
     * itemId    : 道具ID
     * itemType  : 道具类型
     * itemCount : 购买的道具数量
     * virtualCurrency:购买的道具的虚拟价值
     * currencyType:支付方式
     * consumePoint:消费点，如关卡内消费，可以为空
     * levelId:购买时所在的关卡Id，可以为空
     *************************************************/
    static void buyInLevel(const char* itemId, const char* itemType, int itemCount, long long virtualCurrency, const char* currencyType, const char* consumePoint, const char* levelId);
    
	/************************************************* 
	* Description: 获得道具
	* itemId    : 道具ID
    * itemType  : 道具类型
	* itemCount : 道具数量
	* reason    : 获得道具的原因
	*************************************************/
	static void get(const char* itemId, const char* itemType, int itemCount, const char* reason);
    
    /*************************************************
     * Description: 获得道具，用于关卡内获得道具事件
     * itemId    : 道具ID
     * itemType  : 道具类型
     * itemCount : 道具数量
     * reason    : 获得道具的原因
     * levelId   : 获得道具时所在的关卡Id
     *************************************************/
    static void getInLevel(const char* itemId, const char* itemType, int itemCount, const char* reason, const char* levelId);
    
	/************************************************* 
	* Description: 消耗道具
	* itemId    : 道具ID
    * itemType  : 道具类型
	* itemCount : 道具数量
	* reason    : 消耗道具的原因
	*************************************************/
	static void consume(const char* itemId, const char* itemType, int itemCount, const char* reason);
    
    /*************************************************
     * Description: 消耗道具，用于关卡内消耗道具事件
     * itemId    : 道具ID
     * itemType  : 道具类型
     * itemCount : 道具数量
     * reason    : 消耗道具的原因
     * levelId   : 关卡内消耗道具的原因
     *************************************************/
    static void consumeInLevel(const char* itemId, const char* itemType, int itemCount, const char* reason, const char* levelId);
};

//任务接口
class DCTask
{
public:
	/************************************************* 
	* Description: 开始任务
	* taskId     : 任务ID
    * taskType   : 任务类型
	*************************************************/
    static void begin(const char* taskId, DCTaskType type);
	
	/************************************************* 
	* Description: 任务完成
	* taskId     : 任务ID
	*************************************************/
    static void complete(const char* taskId);
	
	/************************************************* 
	* Description: 任务失败
	* taskId     : 任务ID
    * reason     : 任务失败原因
	*************************************************/
    static void fail(const char* taskId, const char* reason);
};

//自定义事件
class DCEvent
{
public:
	/************************************************* 
	* Description: 登陆前自定义事件，用户登陆之后该接口无效
	* eventId    : 事件ID
    * map        : 事件发生时关注的属性map
	* duration   : 事件发生时长
	*************************************************/
	static void onEventBeforeLogin(const char* eventId, map<string, string>* map, long long duration);
	
	/************************************************* 
	* Description: 事件计数
	* eventId    : 事件ID
    * count      : 事件发生次数
	*************************************************/
    static void onEventCount(const char* eventId, int count);
	
	/************************************************* 
	* Description: 事件发生
	* eventId    : 事件ID
	*************************************************/
    static void onEvent(const char* eventId);
	
	/************************************************* 
	* Description: 事件发生
	* eventId    : 事件ID
    * label      : 事件发生时关注的一个属性
	*************************************************/
    static void onEvent(const char* eventId, const char* label);
	
	/************************************************* 
	* Description: 事件发生
	* eventId    : 事件ID
    * label      : 事件发生时关注的多个属性map
	*************************************************/
    static void onEvent(const char* eventId, map<string, string>* map);
    
	/************************************************* 
	* Description: 时长事件发生
	* eventId    : 事件ID
    * duration   : 事件发生时长
	*************************************************/
    static void onEventDuration(const char* eventId, long long duration);
	
	/************************************************* 
	* Description: 时长事件发生
	* eventId    : 事件ID
	* label      : 事件发生时关注的单个属性
    * duration   : 事件发生时长
	*************************************************/
    static void onEventDuration(const char* eventId, const char* label, long long duration);
	
	/************************************************* 
	* Description: 时长事件发生
	* eventId    : 事件ID
	* map        : 事件发生时关注的多个属性
    * duration   : 事件发生时长
	*************************************************/
    static void onEventDuration(const char* eventId, map<string, string>* map, long long duration);
    
	/************************************************* 
	* Description: 过程性事件开始，与onEventEnd(eventId)配合使用
	* eventId    : 事件ID
	*************************************************/
    static void onEventBegin(const char* eventId);
	
	/************************************************* 
	* Description: 过程性事件开始，与onEventEnd(eventId)配合使用
	* eventId    : 事件ID
	* map        : 事件发生时关注的多个属性
	*************************************************/
    static void onEventBegin(const char* eventId, map<string, string>* map);
	
	/************************************************* 
	* Description: 过程性事件结束，与onEventBegin(eventId)或onEventBegin(eventId, map)配合使用
	* eventId    : 事件ID
	*************************************************/
    static void onEventEnd(const char* eventId);
    
	/************************************************* 
	* Description: 过程性事件开始，与onEventEnd(eventId, flag)配合使用
	* eventId    : 事件ID
	* map        : 事件发生时关注的多个属性
	* flag       : eventId的一个标识，与eventId共同决定一个事件，比如玩家升级事件，
	               eventId可以为玩家升级，flag则为玩家具体的等级
	*************************************************/
    static void onEventBegin(const char* eventId, map<string, string>* map, const char* flag);
	
	/************************************************* 
	* Description: 过程性事件结束，与onEventBegin(eventId, map, flag)配合使用
	* eventId    : 事件ID
	* flag       : eventId的一个标识，与eventId共同决定一个事件，比如玩家升级事件，
	               eventId可以为玩家升级，flag则为玩家具体的等级
	*************************************************/
    static void onEventEnd(const char* eventId, const char* flag);
};

//虚拟币接口
class DCCoin
{
public:
	/************************************************* 
	* Description: 设置虚拟币总量
	* coinNum    : 虚拟币总量
    * coinType   : 虚拟币类型
	*************************************************/
    static void setCoinNum(long long total, const char* coinType);
	
	/************************************************* 
	* Description: 消耗虚拟币
	* id         : 消耗虚拟币时关注的属性，如消耗原因
    * coinType   : 虚拟币类型
	* lost       : 消耗虚拟币的数量
	* left       : 玩家剩余虚拟总量
	*************************************************/ 
    static void lost(const char* reason, const char* coinType, long long lost, long long left);
    
    /*************************************************
     * Description: 消耗虚拟币，用于关卡内消耗事件
     * id         : 消耗虚拟币时关注的属性，如消耗原因
     * coinType   : 虚拟币类型
     * lost       : 消耗虚拟币的数量
     * left       : 玩家剩余虚拟总量
     * levelId    : 玩家当前所在的关卡ID
     *************************************************/
    static void lostInLevel(const char* reason, const char* coinType, long long lost, long long left, const char* levelId);
	
	/************************************************* 
	* Description: 消耗虚拟币
	* id         : 消耗虚拟币时关注的属性，如消耗原因
    * coinType   : 虚拟币类型
	* gain       : 消耗虚拟币的数量
	* left       : 玩家剩余虚拟总量
	*************************************************/
    static void gain(const char* reason, const char* coinType, long long gain, long long left);
    
    /*************************************************
     * Description: 获得虚拟币，用于关卡内获得事件
     * id         : 获得虚拟币时关注的属性，如消耗原因
     * coinType   : 虚拟币类型
     * gain       : 获得虚拟币的数量
     * left       : 玩家剩余虚拟总量
     * levelId    : 玩家当前所在的关卡ID
     *************************************************/
    static void gainInLevel(const char* reason, const char* coinType, long long gain, long long left, const char* levelId);
};

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) 
#ifdef __cplusplus
extern "C"
{
#endif

//配置更新成功后,java侧回调C++侧的接口，内部使用
JNIEXPORT void JNICALL Java_com_dataeye_data_DCConfigParams_updateSuccess(JNIEnv *, jobject obj);

#ifdef __cplusplus
}
#endif
#endif

//配置更新成功后，SDK向游戏发送的更新成功消息，开发者需要在监听该消息
#define DCCONFIGPARAMS_UPDATE_COMPLETE "DataeyeConfigParamsComplete"

class DCConfigParams
{
public:
	/************************************************* 
	* Description: 在线参数更新接口
	*************************************************/
    static void update();
	
	/************************************************* 
	* Description: 获取String型参数
	* key        : 需要获取的参数的key值
    * defaultValue: 需要获取的参数的默认值
	*************************************************/
    static const char* getParameterString(const char* key, const char* defaultValue);
	
	/************************************************* 
	* Description: 获取int型参数
	* key        : 需要获取的参数的key值
    * defaultValue: 需要获取的参数的默认值
	*************************************************/
    static int getParameterInt(const char* key, int defaultValue);
	
	/************************************************* 
	* Description: 获取long long型参数
	* key        : 需要获取的参数的key值
    * defaultValue: 需要获取的参数的默认值
	*************************************************/
	static long long getParameterLong(const char* key, long long defaultValue);
	
	/************************************************* 
	* Description: 获取bool型参数
	* key        : 需要获取的参数的key值
    * defaultValue: 需要获取的参数的默认值
	*************************************************/
    static bool getParameterBool(const char* key, bool defaultValue);
};

//关卡接口
class DCLevels
{
public:
	/************************************************* 
	* Description: 开始关卡
    * levelId    : 关卡ID
	*************************************************/
	static void begin(const char* levelId);
	
	/************************************************* 
	* Description: 成功完成关卡
    * levelId    : 关卡ID
	*************************************************/
	static void complete(const char* levelId);
	
	/************************************************* 
	* Description: 关卡失败，玩家关卡中退出游戏时，建议调用该接口
    * levelId    : 关卡ID
	* failPoint  : 失败原因
	*************************************************/
	static void fail(const char* levelId, const char* failPoint);
};

class DCCardsGame
{
public:
    /**
     *  @brief 玩家房间完成一局游戏后调用
	 *  @brief roomId 房间ID
     *  @brief lostOrGainCoin    获得或者丢失的虚拟币数量（不能为零）
	 *  @brief roomType 房间类型
     *  @param taxCoin  完成一局游戏时系统需要回收的虚拟币数量（税收）
     *  @param leftCoin   玩家剩余的虚拟币总量
     */
    static void play(const char* roomId, const char* roomType, const char* coinType, long long lostOrGainCoin, long long taxCoin, long long leftCoin);
    
    /**
     *  @brief 玩家房间内丢失虚拟币时调用（完成一局游戏调用play接口后不必再调用该接口）
	 *  @brief roomId 房间ID
     *  @brief roomType 房间类型
     *  @param lostCoin  丢失的虚拟币数量
     *  @param leftCoin   剩余的虚拟币数量
     */
    static void lost(const char* roomId, const char* roomType, const char* coinType, long long lostCoin, long long leftCoin);
    
    /**
     *  @brief 玩家房间内获得虚拟币时调用（完成一局游戏调用play接口后不必再调用该接口）
	 *  @brief roomId 房间ID
     *  @brief roomType 房间类型
     *  @param gainCoin   赢得的虚拟币数量
     *  @param leftCoin  剩余的虚拟币数量
     */
    static void gain(const char* roomId, const char* roomType, const char* coinType, long long gainCoin, long long leftCoin);
};

#endif
