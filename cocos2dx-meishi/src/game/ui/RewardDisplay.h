#pragma once

#include "Constant.h"

#include "cocos2d.h"
USING_NS_CC;

class RewardDisplay : public Node
{
public:
	enum eRewardType
	{
		NONE = -1,
		MISSION = 0,
		STAR = 1
	};

public:
	RewardDisplay();
	~RewardDisplay();

	bool init(int id, eRewardType type);
	static RewardDisplay* create(int id, eRewardType type);

public:
	void updateUI(int id, RewardDisplay::eRewardType type);

protected:
	void onClose(Ref*);

private:
	Node* mBaseTitle;
	Node* mPerfectTitle;
	std::vector<ui::Text*> mNumList;
	std::vector<Sprite*> mIconList;

	int mGiftId;
	eRewardType mRewardType;
};

//DEFINE_PAGE_READER(ArrowWidget);
