/*
* 뜩AI 엄꿉固놂잚
* 맏得踏狗코AI엄꿉돨밗잿
*/
#pragma once

#include "Singleton.h"
#include "BattleInfo.h"
#include "AIState.h"

class StateFactory;
class PolicyManager
{
public:
	PolicyManager();
	~PolicyManager();

	bool init();

	void	registerAIPolicy(const Policy*);
	void	unregisterAIPolicy(const Policy*);

	StateFactory* getStateFactory(eStateTag etag);
	void	registerStateFactory(eStateTag etag, StateFactory* );
	void	unregisterStateFactory(eStateTag etag);

	ActionFactory* getActionFactory(eActionTag etag);
	void	registerActionFactory(eActionTag etag, ActionFactory*);
	void	unregisterActionFactory(eActionTag etag);

	bool	doBuildAIState(vector<BaseState*>, int);
	bool	doAction(vector<BaseState*>, int);
protected:

	bool checkStates(vector<BaseState*>);

	StateFactoryList	mStateFactoryList;
	ActionFactoryList	mActionFactoryList;

};

#define sgPolicyMgr()	Singleton<PolicyManager>::Get()
