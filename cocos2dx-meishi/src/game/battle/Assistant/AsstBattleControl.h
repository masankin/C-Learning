#ifndef _ASST_BATTLE_MANAGER_H
#define _ASST_BATTLE_MANAGER_H

#include "cocos2d.h"
#include "Constant.h"

class Assistant;

class AsstBattleControl
{
public:
	typedef std::map<eCampType, Assistant*> CampAssistantList;

public:
	AsstBattleControl();
	~AsstBattleControl();

	void run();
	void clean();
	void update(float dt);

	bool registAssistant(Assistant* asst);
	void unregistAssistant(Assistant* asst);
	Assistant* getAssistant(eCampType campType);

	void castActiveSkill(eCampType campType, unsigned int skillId, std::vector<int>& randomList);

protected:
	void onRequestActiveSkill(cocos2d::EventCustom* customData);

	CampAssistantList mCampAsstList;

	std::vector<cocos2d::EventListener*> mListenerList;
};

#endif
