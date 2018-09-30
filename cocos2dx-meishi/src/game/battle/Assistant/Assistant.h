
#ifndef _ASSISTANT_H
#define _ASSISTANT_H

#include "cocos2d.h"
#include "Constant.h"
#include "AssistantConstant.h"
#include "AsstComponent.h"
#include "AsstActiveSkillComponent.h"
#include "AssistantView.h"

class Camp;
class AsstComponent;
class AsstActiveSkill;
class AsstActiveSkillData;

class Assistant : public cocos2d::Ref 
{
public:
	typedef std::unordered_map<std::string, AsstComponent*> ComponentContainer;
	typedef std::vector<AsstActiveSkillData*> ActiveSkillList;

public:
	static Assistant* createWithLuaData(Camp* owner);

	unsigned int getId() const ;
	AssistantView* getView() const;
	eCampType getCampType() const;

	void run();
	void update(float dt);
	void clean();
	void stop();

	bool addComponent(AsstComponent* pComponent);
	bool removeComponent(const std::string& comName);

	void saveActiveSkill(unsigned int skillId, std::vector<int>& randomList);
	void castActiveSkill(unsigned int skillId, std::vector<int>& randomList);

protected:
	Assistant();
	virtual ~Assistant();

	bool initWithLuaData(Camp* pOwner);
	void onLuaGetBtlAsstComplete(lua_State* tolua_S, int argc);

	AsstComponent* getComponent(const std::string& comName) const;

	bool addActiveSkill(AsstActiveSkillData* pSkillData);
	void onActiveSkillEvent(AsstActiveSkillComponent::SkillEvent eventType, unsigned int skillId);
	void cleanActiveSkillCastQueue();

	void onAniEvent(AssistantView::AniEvent eventType);

protected:
	unsigned int mId;
	int mLevel;
	int	mQuality;
	bool mAlive;
	std::string mAniFile;

	Camp* mOwner;
	AssistantView* mView;
	ComponentContainer* mCompoentContainer;

	struct ActiveSkillCastInfo
	{
		unsigned int skillId;
		std::vector<int> randomList;
	};
	std::list<ActiveSkillCastInfo*> mASCastQueue;
};

#endif //_ASSISTANT_H
