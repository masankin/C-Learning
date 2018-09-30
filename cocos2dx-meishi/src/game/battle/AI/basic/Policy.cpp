#include "Policy.h"
#include "battle/BattleControl.h"
#include "battle/AI/AI.h"
#include "battle/Camp.h"
#include "battle/Build.h"

Policy::Policy()
{
	mPolicyInterval = 0.0f;
}

Policy::~Policy()
{

}

bool Policy::init()
{

	return true;
}

void Policy::update(float dt)
{
	mPolicyInterval += dt;
	if (mPolicyInterval >= 1)
	{
		

		const MapBulidList& buildlist = getTargetCamp()->getBuildList();
		MapBulidList::const_iterator it = buildlist.begin();
		for (; it != buildlist.end(); ++it)
		{
			it->second->doAIState(mPolicyInterval);
		}

		mPolicyInterval = 0.0f;
	}
}

eCampType Policy::getPolicyCamp()
{
	return mAITarget->getCampType();
}

Camp* Policy::getTargetCamp()
{
	eCampType etype = getPolicyCamp();
	return sgBattleControl()->getTargetCamp(etype);
}

void Policy::registerState(eStateTag eTag)
{
	StateList::iterator it = mStateList.find(eTag);
	if (it == mStateList.end())
	{
		//mStateList.insert(std::make_pair(eTag, ))
	}
}

void Policy::unregisterState(eStateTag eTag)
{

}

