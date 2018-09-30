#ifndef _ASST_ACTIVE_CONTROLLER_H
#define _ASST_ACTIVE_CONTROLLER_H

#include "cocos2d.h"
#include "AsstComponent.h"

class Assistant;
class AsstActiveSkillData;
class AsstActiveSkill;
class LazyTimer;

class AsstActiveSkillComponent : public AsstComponent
{
public:
	enum class SkillEvent
	{
		kCDBeg,
		kCDEnd,

		kCastBeg,
		kCastIng,
		kCastEnd,
	};

	enum class SkillEffectEvent
	{
		kCastBegEffectOver,
		kCastEndEffectOver,
	};

	typedef std::function<void(SkillEvent, unsigned int skillId)> SkillEventCallback;

public:
	static AsstActiveSkillComponent* create();

	virtual void run() override;
	virtual void clean() override;

	bool addSkill(AsstActiveSkillData* pSkillBase);

	bool checkSkill(unsigned skillId) const;
	void coolSkill(unsigned skillId);

	void castSkill(unsigned int skillId, std::vector<int>& randomList);
	void castSkill(AsstActiveSkill* pSkill, std::vector<int>& randomList);

	void setSkillEventCallback(const SkillEventCallback& callback);

protected:
	AsstActiveSkillComponent();
	virtual ~AsstActiveSkillComponent();

	AsstActiveSkill* createSkill(AsstActiveSkillData* pData);

	virtual bool init() override;
	virtual void update(float dt) override;

	virtual void setOwner(Assistant *pOwner) override;
	
	AsstActiveSkill* getSkill(unsigned int skillId) const;

	void sendSkillEvent(SkillEvent eventType, unsigned int skillId);

	void coolSkill(AsstActiveSkill* pSkill);

	void requestSkill(unsigned int skillId);
	void requestSkill(AsstActiveSkill* pSkill);

	void stopSkillAllTimer(unsigned int skillId);
	void stopSkillAllTimer(AsstActiveSkill* pSkill);

	void onCoolTimeout(const std::string& key, unsigned int skillId);
	void onDelayIntervalTimeout(const std::string& key, unsigned int skillId);
	void onDelayTimeout(const std::string& key, unsigned int skillId);
	void onContinualTimeout(const std::string& key, unsigned int skillId);
	void onIntervalTimeout(const std::string& key, unsigned int skillId);

protected:
	typedef std::map<unsigned int, AsstActiveSkill*> SkillContainer;

	static const float DelayInterval;
	static const float DelayCastProb;

	SkillContainer mSkillContainer;
	
	LazyTimer* mTimer;

	SkillEventCallback mOnSkillEventCallback;
};

#endif
