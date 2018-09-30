#pragma once

#include "Singleton.h"
#include "GameData.h"
#include "cocos2d.h"
#include "net/proto/SaveData.pb.h" 
#include "game/data/SyncData.h"
#include "Constant.h"
#include "UserProxy.h"
#include "CVTime.h"
#ifdef MGAME_PLATFORM_MPLUS
#include "mplus/MplusControl.h"
#endif
USING_NS_CC;

class UserObj
{
public:

	UserObj();
	~UserObj(); 

	enum PROPERTYTYPE
	{
		PT_PVEPOWER = 1,		//体力
		PT_PVPPOWER,		//精力
		PT_DIAMOND,			//钻石
		PT_ITEM,			//道具
		POWER_SOLT,			//--体力槽
		ENERGY_SOLT,		//--精力槽
		MONTH_CARD,			//--月卡
		TITLE_ADD,			//--徽章新增
		ROOM_TICKET,		//--开房券
		PLAYER_EXP,			//--玩家总经验
		Honor,				//荣誉
		STUFF,				//符文材料
		POINT,				//玩家总积分
		CHAT,				//聊天免费次数
		ASSISTANT = 15,		//将军
		GLYPH = 16,			//厨艺

	};

	

	void			sendGetCheckPtData();
	void			sendGetNewBieList();
	void			sendEnterPoint(int pointid);
	void			sendUpdateCheckPoint();
	void            sendDailyMissionData(bool isPvp);                      

	void			doGetCheckPtData(const ::fgame::CSGetCheckPtDataResp&);
	void			doSaveCheckPtData(const fgame::CSSaveCheckPtDataResp&);
	void			doGetGiftReward(const fgame::CSRetriveCheckPtRewardResp&);		//领取礼包
	void			doGetNewBieList(const ::fgame::CSGetNewBieListResp&);
	void			doGetNewBieReward(const ::fgame::CSPveCheckAwardResp&);			//新手训练营完成礼包
	void			doPropertyUpdate(const fgame::CSPropertyUpdateResp&);
	
	void			doEnterPoint();

	void			initUserObj(); 
	void			initPlayerData(); 
	void			loadPlayerData();
	void			saveToFile();
	
	void			update(float dt);
 
	// 	PVE体力相关
	void			changePvePower(int value);

	// 	PVP体力相关
	void			changePvpPower(int value);

	//	货币单位相关
	void			setDiamond(int value, int src = 0);

	//  新手引导数据相关
	void			setGuideFinish(int groupID); 
	void			syncGuideFinish(std::map<int, bool>& idMap);
	bool			getGuideFinish(int groupID);
	int				getMaxGuideGroup();
	void			sendAddGuideDiamond(int count);
	void			doAddGuideDiamond(int count); 
	void			addShopGuideNum(int value);

	void			sendAddGuideGlyphStuff();
	void			doAddGuideGlyphStuff(int value);
	void			addGlyphStuffGuideNum(int value);

	//新手宝典数据相关
	void			setBibleTabItemId(int tabItemID);
	int 			getBibleTabItemId();

	//pve结算失败界面数据相关
	void			setFirstFail(int failTag);
	int 			getFirstFail();

	//	道具相关 
	void			setProp(int id, int count);
	int				getProp(int id);  
	void			initProp(const fgame::CSSycPlayerInfoResp&);
	std::map<int, int> getPropList();

	//道具选择
	void			setPropItem(int pos, int id);
	void			cleanPropItem(int id);
	int				getPropItem(int pos);
	bool			refreshPropItemData(int pos, int id);
	std::map<int, int> getPropItemList();

	//关卡相关	
	bool			getIsBranch(int levelID);													//是否支线(废弃接口)
	int				getCheckPointStar(int levelID);												//获取星星数(废弃接口)
	int				getTotalStars();															//查询所有的星星
	int				getMaxOpenCheckPoint(bool realTime = false);								//获取最大开启关卡(废弃接口)
	void			updateCheckPoint(int id, int star, int  passTime =0,int openCheckPt = 0);	//更新关卡状态
	int				getCheckPointState(int id);													//获取关卡状态(废弃接口)
	int				getNowPassCheckPoint();
	int				getNextCheckPt();

	///////////////////////////////begin//////////////////////////////
	int			    getMaxOpenGateID() { return mMaxOpenGateID;  };
	void			setMaxOpenGateID(int gateID) { mMaxOpenGateID = gateID; }
	int				getGateState(int id); 
	void			setGateState(int id, int state);
	void			clearGateState();
	///////////////////////////////end//////////////////////////////
	int             getHasSkipCount();                                                           //获取当日跳关次数
	void            onSkip(int id);                                                              //跳关
	void            setIsSkipSuccess(bool isSkip){ mIsSkipSuccess = isSkip; };
	bool            getIsSkipSuccess(){ return mIsSkipSuccess; };

	//礼包相关
	bool			getReceiveAward(int pointId ,int type);		//查询是否领取  type: 1通关礼包 2满星礼包
	void			setReceiveAward(int pointId,int type);		//添加领取通关礼包 
	void			getNewBieReward();							//新手训练营完成礼包
	//流失点相关
	int				getOneLostPointData(int id);								//获取流失点数据状态，不存在该流失点：-1 存在但不通过：0 存在并且通过： 1
	void			updateLostPointData(int id, bool pass, bool save = true);	//更新流失点
	bool			refreshLostPointData(int id, int state);
	 
	unsigned int	getUin();
	void			setUin(unsigned int value);

	unsigned int	getNewUin();
	unsigned int 	convertToNewUin(unsigned int Uin);
	unsigned int	convertToUin(unsigned int NewUin);
	std::string		getPlayerName();

	std::string		getOpenID();
	void			setOpenID(std::string value);

	void			setInputID(std::string value);

	//月卡
	bool			isCanGetMonthCard();				//是否有月卡

	//登陆相关
	bool			isInitLoginData();
	void			updateLastServerTime();
	int64_t			getLastServerTime();
	void			setGameServerId(std::string name);
	std::string		getGameServerName();
	void			setLoginServerDir(std::string);
	std::string		getLoginServerDir();

	int				getRankGroup() const;				//获取排行组别

	void			setScore(int val);					//设置pvp积分，原方法setPvpPoint

	int64_t			getNowTime();

	void			addCostMoney(int money);
	bool			isCostMax(int money);
	void			addTestMoney(int money);

	std::string		getPlayerDataStr();
	void			setPlayerDataStr(std::string s1);

	bool			checkNoSavaData();									//检测是否新存档标记
	void			setSaveDataOnLine(bool value);						//从服务器获取存档
	void			setSaveDataOffLine(bool value);						//离线设置存档
	
	bool			syncSaveDataServer(fgame::UserData data);			//同步存档
	int				getCurSaveDataVersion();
	void			setCurSaveDataVersion(int value);

	//徽章相关
	void			initBadges(const fgame::CSSycPlayerInfoResp&);
	void			addBadge(int iBadge);
	void			addBadgeItem(std::string strJason);
	std::vector<int> getBadges();
	const std::map<unsigned int, BadgeItem>& getObtainedBadges() const;
	bool			isHaveBadge(int iBadge);
	BadgeItem*		getBadgeItem(int iBadge);

	void			cleanCheckPtDataMap(){ mCheckPtDataMap.clear(); }

	void			addActNoticeData(int id);
	bool			checkIsHaveAllNoticeData();
	bool			checkIsHaveNoticeData(int id);
	void			cleanmActNoticeList() { mActNoticeList.clear(); }
	void			updateActInformData();
	int64_t			getActInformTime();
	void			initVipTime(std::string str);
	CVTimeSpan		getVipTime(int id);
	
	void			updatePlayerExp(string str); 

	bool			loadGlyphStuff(); 
	int				getGlyphStuffNum(int stuffID);

	fgame::CSSycPlayerInfoResp* getPersonalInfo() { return &m_stPersonalInfo; }	//个人信息
	std::string		getPersonalString();										//个人信息序列化串
	
	bool		isOpenGamePassTip();
	void		changeGamePassTip(bool bOpen);

	unsigned int	getChatBuyTip() { return mPlayerData.chatbuytip(); }
	void			setChatBuyTip(unsigned int tip) { mPlayerData.set_chatbuytip(tip); }
	unsigned int	getChatFreeCount() { return m_stPersonalInfo.chatfreecount(); }
	void			setChatFreeCount(unsigned int count) { m_stPersonalInfo.set_chatfreecount(count); }
	int64_t			getChatTimeLeft() { return mPlayerData.chattimeleft(); }
	void			setChatTimeLeft(int64_t left) { mPlayerData.set_chattimeleft(left); }

	// 保存显示礼包的ID
	CC_SYNTHESIZE(int, mShowGiftId, ShowGiftId);
	void			showGiftInfoById(int gift_id);

	CC_SYNTHESIZE(int, mOccupyModeFlag, OccupyModeFlag);  
	CC_SYNTHESIZE(bool, mHaveGlyph, HaveGlyph);
	// 登录相关
	void			addLoginSrv2List(int channelId, const std::string& address);
	void			dltLoginSrvFromList(int channelId, const std::string& address);
	std::string		sltLoginSrvRand(int channelId);

	fgame::UserData* getSaveData() { return &mPlayerData; }


	//每日任务相关
	typedef std::map<int, Point> DailyMissionTips;
	void clearDailyMissionTips(){ mDailyMissionTipList.clear(); }
	void addDailyMissionTips(int type, Point pos);//type--0:1级指引 >0：2级指引类型
	Point getDailyMissionTipPointByType(int type);

	void setIsShowDailyMissionBtn(bool isShow){ mIsShowDailyMissionBtn = isShow; };
	bool getIsShowDailyMissionBtn(){ return mIsShowDailyMissionBtn; };

	void setIsAfterAwardOpenAgain(bool isOpen){ mIsAfterAwardOpenAgain = isOpen; };
	bool getIsAfterAwardOpenAgain(){ return mIsAfterAwardOpenAgain; };

	void setIsShowDailyMissionProgress(bool isShow){ mIsShowDailyMissionProgress = isShow; };
	bool getIsShowDailyMissionProgress(){return mIsShowDailyMissionProgress; };

	void setIsOpenDailyMissionFirst(bool isShowFirst){ mIsOpenDailyMissionFirst = isShowFirst; };
	bool getIsOpenDailyMissionFirst(){ return mIsOpenDailyMissionFirst; };

	void setIsOpenDailyMissionDialog(bool isOpen){ mIsOpenDailyMissionDialog = isOpen; };
	bool getIsOpenDailyMissionDialog(){ return mIsOpenDailyMissionDialog; };

	void resetDailyMissionData();

	bool getIsGoneNewerProtect();
protected:	 
	std::string mOpendID;
	CC_SYNTHESIZE(bool, mIsHost, IsHost);							//是否是pvp主机
	CC_SYNTHESIZE(unsigned int, mWaitNum, WaitNum);
	CC_SYNTHESIZE(bool, mLvlUpgrade, LvlUpgrade);					//玩家升级，需要播放升级动画
	CC_SYNTHESIZE(int, mPlayerUpgradeState, PlayerUpgradeState);	//玩家播放升级动画限制状态
	CC_SYNTHESIZE(bool, mIsShopBuy, IsShopBuy);
				

	CC_SYNTHESIZE(int, mWinnerType, WinnerType);					//lua获取王者之路的页签 0,1,2
	CC_SYNTHESIZE(bool, mIsOpenCrazyWeek, IsOpenCrazyWeek);			//lua获取狂欢周开启

	//CC_SYNTHESIZE(int, mPlayerWinCount, PlayerWinCount);			//玩家胜利场次
	//CC_SYNTHESIZE(int, mPlayerConWinCount, PlayerConWinCount);		//玩家连胜场次

	bool mIsNewSavaData;											//是否是新存档数据标记

	typedef std::map<int, int> LostPointMap;
	//流失点存储
	LostPointMap m_LostPointDataMap;

	//活动公告id存储
	std::vector<int> mActNoticeList;

	typedef std::map<int, int> PropItemMap;
	PropItemMap mPropItemMap;
	
	typedef std::map<int, CVTimeSpan> VipTime;
	VipTime mVipTime;

	//如果是require字段一定要初始化!!!
	fgame::UserData mPlayerData;

	//获得的徽章List
	std::vector<int> mBadgeGetVec;
	std::map<unsigned int, BadgeItem> mObtainedBadges;

	bool mSendGuideRqst;

	CC_SYNTHESIZE(std::string, mHeadIconUrl, HeadIconUrl);//sdk头像URL
private: 
	void	    onLuaGetGlyphStuff(lua_State* tolua_S, int argc);
	bool		loadDataFromFile(std::string path);
	void 		savePlayerData();
	bool		saveTempPlayerData(std::string path);


	void		loadPropItem();
	void		savePropItem();
	

	void		loadLostPointData();
	void		saveLostPointData();

	void		loadActNoticeData();
	void		saveActNoticeData();
 
	bool		isInitCostData();
	void		onGetSaveDataFinish(cocos2d::Ref* ref);

private:
	UserProxy mUserProxy;

	std::map<int, int > mGlyphStuffList;

	std::map<int ,fgame::CheckPtData> mCheckPtDataMap;		//已通关关卡信息
	
	int mOpenCheckPtId;										//开放的最大关卡

	std::map<int, int> mGuideGateDataMap;					//引导关关卡信息
	fgame::CSGetNewBieListResp mGetNewBieListResp;			//已完成的新手引导
	int mMaxOpenGateID;
	bool mIsSkipSuccess;     //记录跳关成功

	typedef std::map<int, int> PropMap;
	PropMap mPropMap;

	unsigned int     mUin;
	unsigned int	 mNewUin;
	int mNowPassPt;
	int mNowPassStar;
	int mNowPassTime;
	int mOpenPt;
	bool mIsWin;
		 
	CC_SYNTHESIZE(unsigned int, mCurInfoUin, CurInfoUin);	//玩家当前请求的个人信息的uin
	CC_SYNTHESIZE(int, mCurInfoType, CurInfoType);			//玩家当前请求的个人信息的type

	fgame::CSSycPlayerInfoResp	m_stPersonalInfo;			//个人信息
	int				getScore(){ return m_stPersonalInfo.score(); };			//玩家排行榜积分
	std::string					m_szPersonalString;			//个人信息序列化串

	DailyMissionTips mDailyMissionTipList;
	bool mIsShowDailyMissionBtn;
	bool mIsAfterAwardOpenAgain;
	bool mIsShowDailyMissionProgress;
	bool mIsOpenDailyMissionFirst;
	bool mIsOpenDailyMissionDialog;//12点刷新
	CC_SYNTHESIZE(bool, mIsShowDailyMissionHot, IsShowDailyMissionHot);

	CC_SYNTHESIZE(std::string, mChannelPlayerName, ChannelPlayerName);	//玩家渠道用户名
};

#define sgUserObj()		Singleton<UserObj>::Get()
