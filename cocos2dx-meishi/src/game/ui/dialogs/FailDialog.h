#pragma once

#include  "game/ui//system/ActionWindow.h"
#include "GamePrecedure.h"
#include "Message.h"

class StarLvNode;
class FailDialog :public ActionWindow,public MessageHandler
{
public:
	FailDialog();;
	~FailDialog();;

	virtual bool init();

	virtual void onShow();
	virtual void onReceiveMassage(GameMessage * message);

	CREATE_FUNC(FailDialog);

protected:
	virtual void onMenuItemAction(std::string, cocos2d::Ref*);

	virtual void aniCallback(cocostudio::Armature * pAr, cocostudio::MovementEventType type, const std::string& aniName);
	virtual void onFrameEvent(cocostudio::Bone *bone, const std::string& evt, int originFrameIndex, int currentFrameIndex);

	void onCallBackBuyPower(Ref*);
	void onCallBackBuyDiamond(Ref*);

	StarLvNode* mStarLvNode;
	cocostudio::Armature*  mArmatureGuang;
	cocostudio::Armature*  mBibleAni;
	int mCurLevelId;
};
