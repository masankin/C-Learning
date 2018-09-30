#pragma once

#include "Constant.h"

#include "cocos2d.h"
USING_NS_CC;

class ArrowWidget : public Node
{
public:
	ArrowWidget(){};
	~ArrowWidget(){};

	static ArrowWidget*	create();

	bool initUI();

	void stat2end(Vec2 star, Vec2 end, bool isFaceBuild = false);
	void reset();

	//Size minSize;
	
protected:
	virtual void onMenuItemAction(std::string, cocos2d::Ref*);
	
	Node* mRoot;
	ui::ImageView* mArrowHead;
	ui::ImageView* mArrowEnd;

	cocostudio::Armature* mBattleArrowTou;
	//cocostudio::Armature* mBattleArrowWei;
	//cocostudio::Bone* mArrowEndBone;
};

//DEFINE_PAGE_READER(ArrowWidget);
