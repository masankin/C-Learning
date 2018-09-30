#pragma once

#include "Singleton.h"
#include "game/BattleInfo.h"
#include "BaseProxy.h"
#include "game/Message.h"
#include "Constant.h"

class PvpProxy : public BaseProxy
{
public:

	PvpProxy();
	~PvpProxy();

	// override begin
	virtual void DoRsp(NetPack& stRsp);

	virtual bool fault(const fgame::CSErrorMsg &err);
	// override end

	//@type : ePvpMatchType
	void	sendMatchRqst(int type,int mode = 1,int score = 0);

	void	sendStartRqst(int type);

	void	sendPing();

	void	sendMap(const MapBulidList& builList);

	void	sendFight(int src,int des,int num,int fId);

	void	sendUpgrade(int buildId,int level);	

	void	sendEnd(int uid);

	void	sendLeave();

	void	sendStopMatch(int mode);

	void	sendUseItem(int id, const Vec2 pos, int buildId);

	void	sendAppearCreep(int id,int dataId);

	void	sendDeadCamp(eCampType type);

	void	sendRecoverItem(int uin, int itemId);

	void	sendAsstSkill(int campId, int skillId, int rollNum);

	//void	sendOb
protected:

	int mMsgNum;
	CC_SYNTHESIZE(int, mRoomId, RoomId);

};

