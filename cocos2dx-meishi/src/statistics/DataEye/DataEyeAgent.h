#pragma once

#include "cocos2d.h"
#include "Singleton.h"

class DataEyeAgent : cocos2d::Ref
{
public:

	enum AccountType
	{
		Anonymous = 0,
		Registered,
		SinaWeibo,
		QQ,
		QQWeibo,
		ND91,
		Type1,
		Type2,
		Type3,
		Type4,
		Type5,
		Type6,
		Type7,
		Type8,
		Type9,
		Type10
	};

	enum Gender
	{
		UNKNOWN = 0,
		MALE,
		FEMALE
	};

	enum TaskType
	{
		GuideLine = 1,
		MainLine,
		BranchLine,
		Daily,
		Activity,
		Other
	};

	typedef std::pair< std::string, std::string >   EventParamPair;
	typedef std::map< std::string, std::string >    EventParamMap;

	/*static const std::string Level_Id;

	static const std::string EventLevel_Id;
	static const std::string EventLevel_Key_Uid;
	static const std::string EventLevel_Key_StarCount;
	static const std::string EventLevel_Key_Result;
	static const std::string EventLevel_Key_Perfect;

	static const std::string EventBoss_Id;
	static const std::string EventBoss_Key_Unlock;
	static const std::string EventBoss_Key_Uid;
	static const std::string EventBoss_Key_Result;

	static const std::string EventChapter_Id;
	static const std::string EventChapter_Key_Unlock;
	static const std::string EventChapter_Key_Uid;

	static const std::string EventLostPoint_Id;
	static const std::string EventLostPoint_Key_Step;

	static const std::string EventRMBBuy_Id;*/

public:
	DataEyeAgent();
	~DataEyeAgent();

	bool initilized();

	/*************************************************
	* Description: 自定义版本号
	* version    : 版本号
	*************************************************/
	void setVersion(const std::string& version);

	/*************************************************
	* Description: 自定义错误接口
	* title      : 错误名
	* error      : 错误内容，建议传入错误堆栈信息
	*************************************************/
	void reportError(const std::string& title, const std::string& error);

	/*************************************************
	* Description: 立即上报数据，如用户非常在意一些数据，希望
	其立即上报时，可以调用该接口
	*************************************************/
	void uploadNow();

	/*************************************************
	* Description: 获取当前设备UID
	* return     : UID
	*************************************************/
	std::string getUID();


	//玩家信息接口

	/*************************************************
	* Description: 玩家登陆，在DC_AFTER_LOGIN模式下使用
	* accountId  : 玩家账号ID，该参数请保证全局唯一
	*************************************************/
	void accountLogin(const std::string& accountId);

	/*************************************************
	* Description: 玩家登陆，在DC_AFTER_LOGIN模式下使用
	* accountId  : 玩家账号ID，该参数请保证全局唯一
	* gameServer : 玩家账号所在区服
	*************************************************/
	void accountLogin(const std::string& accountId, const std::string& gameServer);

	/*************************************************
	* Description: 玩家登出，在DC_AFTER_LOGIN模式下使用
	*************************************************/
	void accountLogout();

	/*************************************************
	* Description: 获取当前用户ID
	* return     : 用户ID
	*************************************************/
	std::string accountGetId();

	/*************************************************
	* Description: 设置玩家账号类型
	* accountType: 玩家账号类型，值为DCAccountType中的枚举
	*************************************************/
	void accountSetType(AccountType accountType);

	/*************************************************
	* Description: 设置玩家等级
	* level      : 玩家账号等级
	*************************************************/
	void accountSetLevel(int level);

	/*************************************************
	* Description: 设置玩家性别
	* gender     : 玩家性别
	*************************************************/
	void accountSetGender(Gender gender);

	/*************************************************
	* Description: 设置玩家年龄
	* age        : 玩家年龄
	*************************************************/
	void accountSetAge(int age);

	/*************************************************
	* Description: 设置玩家所在区服
	* gameServer : 玩家账号所在区服
	*************************************************/
	void accountSetGameServer(const std::string& gameServer);

	/*************************************************
	* Description: 给玩家打标签，用户SDK的后续推送功能
	* tag        : 一级标签
	* subTag     : 二级标签
	*************************************************/
	void accountAddTag(const std::string& tag, const std::string& subTag);

	/*************************************************
	* Description: 取消玩家标签，用户SDK的后续推送功能
	* tag        : 一级标签
	* subTag     : 二级标签
	*************************************************/
	void accountRemoveTag(const std::string& tag, const std::string& subTag);


	//付费接口

	/*************************************************
	* Description: 付费接口，支付SDK付费成功回调时调用
	* orderId       : 订单ID
	* iapId         : 礼包ID
	* currencyAmount: 付费金额
	* currencyType  : 付费币种
	* paymentType   : 付费途径
	*************************************************/
	void virtualCurrencyPaymentSuccess(const std::string& orderId, const std::string& iapId, double currencyAmount, const std::string& currencyType, const std::string& paymentType);

	/*************************************************
	* Description: 付费接口，用于当前上报关卡内付费数据，支付SDK付费成功回调时调用
	* orderId       : 订单ID
	* iapId         : 礼包ID
	* currencyAmount: 付费金额
	* currencyType  : 付费币种
	* paymentType   : 付费途径
	* levelId       : 关卡ID
	*************************************************/
	void virtualCurrencyPaymentSuccessInLevel(const std::string& orderId, const std::string& iapId, double currencyAmount, const std::string& currencyType, const std::string& paymentSuccess, const std::string& levelId);


	//道具接口

	/*************************************************
	* Description: 购买道具
	* itemId    : 道具ID
	* itemType  : 道具类型
	* itemCount : 购买的道具数量
	* virtualCurrency:购买的道具的虚拟价值
	* currencyType:支付方式
	* consumePoint:消费点，如关卡内消费，可以为空
	*************************************************/
	void itemBuy(const std::string& itemId, const std::string& itemType, int itemCount, long long virtualCurrency, const std::string& currencyType, const std::string& consumePoint);

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
	void itemBuyInLevel(const std::string& itemId, const std::string& itemType, int itemCount, long long virtualCurrency, const std::string& currencyType, const std::string& consumePoint, const std::string& levelId);

	/*************************************************
	* Description: 获得道具
	* itemId    : 道具ID
	* itemType  : 道具类型
	* itemCount : 道具数量
	* reason    : 获得道具的原因
	*************************************************/
	void itemGet(const std::string& itemId, const std::string& itemType, int itemCount, const std::string& reason);

	/*************************************************
	* Description: 获得道具，用于关卡内获得道具事件
	* itemId    : 道具ID
	* itemType  : 道具类型
	* itemCount : 道具数量
	* reason    : 获得道具的原因
	* levelId   : 获得道具时所在的关卡Id
	*************************************************/
	void itemGetInLevel(const std::string& itemId, const std::string& itemType, int itemCount, const std::string& reason, const std::string& levelId);

	/*************************************************
	* Description: 消耗道具
	* itemId    : 道具ID
	* itemType  : 道具类型
	* itemCount : 道具数量
	* reason    : 消耗道具的原因
	*************************************************/
	void itemConsume(const std::string& itemId, const std::string& itemType, int itemCount, const std::string& reason);

	/*************************************************
	* Description: 消耗道具，用于关卡内消耗道具事件
	* itemId    : 道具ID
	* itemType  : 道具类型
	* itemCount : 道具数量
	* reason    : 消耗道具的原因
	* levelId   : 关卡内消耗道具的原因
	*************************************************/
	void itemConsumeInLevel(const std::string& itemId, const std::string& itemType, int itemCount, const std::string& reason, const std::string& levelId);


	//任务接口

	/*************************************************
	* Description: 开始任务
	* taskId     : 任务ID
	* taskType   : 任务类型
	*************************************************/
	void taskBegin(const std::string& taskId, TaskType type);

	/*************************************************
	* Description: 任务完成
	* taskId     : 任务ID
	*************************************************/
	void taskComplete(const std::string& taskId);

	/*************************************************
	* Description: 任务失败
	* taskId     : 任务ID
	* reason     : 任务失败原因
	*************************************************/
	void taskFail(const std::string& taskId, const std::string& reason);


	//自定义事件

	/*************************************************
	* Description: 登陆前自定义事件，用户登陆之后该接口无效
	* eventId    : 事件ID
	* map        : 事件发生时关注的属性map
	* duration   : 事件发生时长
	*************************************************/
	void onEventBeforeLogin(const std::string& eventId, EventParamMap* map, long long duration);

	/*************************************************
	* Description: 事件计数
	* eventId    : 事件ID
	* count      : 事件发生次数
	*************************************************/
	void onEventCount(const std::string& eventId, int count);

	/*************************************************
	* Description: 事件发生
	* eventId    : 事件ID
	*************************************************/
	void onEvent(const std::string& eventId);

	/*************************************************
	* Description: 事件发生
	* eventId    : 事件ID
	* label      : 事件发生时关注的一个属性
	*************************************************/
	void onEvent(const std::string& eventId, const std::string& label);

	/*************************************************
	* Description: 事件发生
	* eventId    : 事件ID
	* label      : 事件发生时关注的多个属性map
	*************************************************/
	void onEvent(const std::string& eventId, EventParamMap* map);

	/*************************************************
	* Description: 时长事件发生
	* eventId    : 事件ID
	* duration   : 事件发生时长
	*************************************************/
	void onEventDuration(const std::string& eventId, long long duration);

	/*************************************************
	* Description: 时长事件发生
	* eventId    : 事件ID
	* label      : 事件发生时关注的单个属性
	* duration   : 事件发生时长
	*************************************************/
	void onEventDuration(const std::string& eventId, const std::string& label, long long duration);

	/*************************************************
	* Description: 时长事件发生
	* eventId    : 事件ID
	* map        : 事件发生时关注的多个属性
	* duration   : 事件发生时长
	*************************************************/
	void onEventDuration(const std::string& eventId, EventParamMap* map, long long duration);

	/*************************************************
	* Description: 过程性事件开始，与OnEventEnd(eventId)配合使用
	* eventId    : 事件ID
	*************************************************/
	void onEventBegin(const std::string& eventId);

	/*************************************************
	* Description: 过程性事件开始，与OnEventEnd(eventId)配合使用
	* eventId    : 事件ID
	* map        : 事件发生时关注的多个属性
	*************************************************/
	void onEventBegin(const std::string& eventId, EventParamMap* map);

	/*************************************************
	* Description: 过程性事件结束，与OnEventBegin(eventId)或OnEventBegin(eventId, map)配合使用
	* eventId    : 事件ID
	*************************************************/
	void onEventEnd(const std::string& eventId);

	/*************************************************
	* Description: 过程性事件开始，与OnEventEnd(eventId, flag)配合使用
	* eventId    : 事件ID
	* map        : 事件发生时关注的多个属性
	* flag       : eventId的一个标识，与eventId共同决定一个事件，比如玩家升级事件，
	eventId可以为玩家升级，flag则为玩家具体的等级
	*************************************************/
	void onEventBegin(const std::string& eventId, EventParamMap* map, const std::string& flag);

	/*************************************************
	* Description: 过程性事件结束，与OnEventBegin(eventId, map, flag)配合使用
	* eventId    : 事件ID
	* flag       : eventId的一个标识，与eventId共同决定一个事件，比如玩家升级事件，
	eventId可以为玩家升级，flag则为玩家具体的等级
	*************************************************/
	void onEventEnd(const std::string& eventId, const std::string& flag);


	//虚拟币接口

	/*************************************************
	* Description: 设置虚拟币总量
	* coinNum    : 虚拟币总量
	* coinType   : 虚拟币类型
	*************************************************/
	void coinSetCoinNum(long long total, const std::string& coinType);

	/*************************************************
	* Description: 消耗虚拟币
	* id         : 消耗虚拟币时关注的属性，如消耗原因
	* coinType   : 虚拟币类型
	* lost       : 消耗虚拟币的数量
	* left       : 玩家剩余虚拟总量
	*************************************************/
	void coinLost(const std::string& reason, const std::string& coinType, long long lost, long long left);

	/*************************************************
	* Description: 消耗虚拟币，用于关卡内消耗事件
	* id         : 消耗虚拟币时关注的属性，如消耗原因
	* coinType   : 虚拟币类型
	* lost       : 消耗虚拟币的数量
	* left       : 玩家剩余虚拟总量
	* levelId    : 玩家当前所在的关卡ID
	*************************************************/
	void coinLostInLevel(const std::string& reason, const std::string& coinType, long long lost, long long left, const std::string& levelId);

	/*************************************************
	* Description: 消耗虚拟币
	* id         : 消耗虚拟币时关注的属性，如消耗原因
	* coinType   : 虚拟币类型
	* gain       : 消耗虚拟币的数量
	* left       : 玩家剩余虚拟总量
	*************************************************/
	void coinGain(const std::string& reason, const std::string& coinType, long long gain, long long left);

	/*************************************************
	* Description: 获得虚拟币，用于关卡内获得事件
	* id         : 获得虚拟币时关注的属性，如消耗原因
	* coinType   : 虚拟币类型
	* gain       : 获得虚拟币的数量
	* left       : 玩家剩余虚拟总量
	* levelId    : 玩家当前所在的关卡ID
	*************************************************/
	void coinGainInLevel(const std::string& reason, const std::string& coinType, long long gain, long long left, const std::string& levelId);

	//关卡接口

	/*************************************************
	* Description: 开始关卡
	* levelId    : 关卡ID
	*************************************************/
	void levelsBegin(const std::string& levelId);

	/*************************************************
	* Description: 成功完成关卡
	* levelId    : 关卡ID
	*************************************************/
	void levelsComplete(const std::string& levelId);

	/*************************************************
	* Description: 关卡失败，玩家关卡中退出游戏时，建议调用该接口
	* levelId    : 关卡ID
	* failPoint  : 失败原因
	*************************************************/
	void levelsFail(const std::string& levelId, const std::string& failPoint);



	/**
	*  @brief 玩家房间完成一局游戏后调用
	*  @brief roomId 房间ID
	*  @brief lostOrGainCoin    获得或者丢失的虚拟币数量（不能为零）
	*  @brief roomType 房间类型
	*  @param taxCoin  完成一局游戏时系统需要回收的虚拟币数量（税收）
	*  @param leftCoin   玩家剩余的虚拟币总量
	*/
	void cardsGamePlay(const std::string& roomId, const std::string& roomType, const std::string& coinType, long long lostOrGainCoin, long long taxCoin, long long leftCoin);

	/**
	*  @brief 玩家房间内丢失虚拟币时调用（完成一局游戏调用play接口后不必再调用该接口）
	*  @brief roomId 房间ID
	*  @brief roomType 房间类型
	*  @param lostCoin  丢失的虚拟币数量
	*  @param leftCoin   剩余的虚拟币数量
	*/
	void cardsGameLost(const std::string& roomId, const std::string& roomType, const std::string& coinType, long long lostCoin, long long leftCoin);

	/**
	*  @brief 玩家房间内获得虚拟币时调用（完成一局游戏调用play接口后不必再调用该接口）
	*  @brief roomId 房间ID
	*  @brief roomType 房间类型
	*  @param gainCoin   赢得的虚拟币数量
	*  @param leftCoin  剩余的虚拟币数量
	*/
	void cardsGameGain(const std::string& roomId, const std::string& roomType, const std::string& coinType, long long gainCoin, long long leftCoin);


private:
	bool mEnable;
};

#define sgDataEyeAgent() Singleton<DataEyeAgent>::Get()
