#pragma once

//玩家信息通信类

#include "BaseProxy.h"

class UserProxy : public BaseProxy
{
public:
	UserProxy(void);
	~UserProxy(void);

	virtual void DoRsp(NetPack& stRsp);
	virtual bool fault(const fgame::CSErrorMsg &err);

	//关卡信息
	void sendUpdateCheckPoint(int id, unsigned star,int passTime,bool isWin, int openCheckPt = 0);//通关信息
	void sendGetGift(int id, int type);//关卡礼包领取信息 1通关礼包 2满星礼包
	void sendGetNewBieReward();			//新手训练营完成礼包
	void sendGetCheckPtData();
	void sendEnterPoint(int pointId);

	//新手引导信息
	void sendGetNewBieList();
	void sendFinishGuideID(int id, bool isComplete);
	void sendFinishGuideIDList(std::vector<int > idList, bool isComplete);
	
	 
	void sendGuideDiamonRqst(int count); 

	void sendGuideGlyphStuffRqst();

private:
};

