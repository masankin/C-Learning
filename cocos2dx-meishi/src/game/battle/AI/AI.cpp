#include "AI.h"
#include "MessageManager.h"
#include "battle/BattleRecordsManager.h"

AI::AI() : mPauseAI(false)
{
	sgMessageManager()->regisiterMessageHandler(EMT_CHECKPOINT_END, this);
}

AI::~AI()
{
	sgMessageManager()->removeMessageHandler(this);
}

bool AI::init(eCampType etype)
{

	mCampType = etype;

	mPolicy.setAITarget(this);




	return true;
}

AI* AI::create(eCampType etype)
{
	AI* pRet = new AI();
	if (pRet && pRet->init(etype))
	{
		return pRet;
	}
	return NULL;
}

void AI::update(float deta)
{
	if (mPauseAI || true == sgBattleRecordsManager()->getIsPlayback())
		return;

	mPolicy.update(deta);
}

void AI::onReceiveMassage(GameMessage * message)
{

	if (message->getMessageType() == EMT_CHECKPOINT_END)
	{
		mPauseAI = true;
	}

}
