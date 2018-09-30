#pragma once

#include "Constant.h"
#include "cocos2d.h"

USING_NS_CC;

class Solider;

class SoliderView : public Node
{
public:
	static SoliderView* create(Solider* owner);

	virtual void pause() override;
	virtual void resume() override;

	void setWorldPosition(const cocos2d::Point& worldPos);
	void setWorldPosition(float worldX, float worldY);
	Point getWorldPosition();

	void refresh();
	void face(bool val); // ture for left, false for false;
	void show(bool val);
	void setGray(bool val);

	void playDeadAni();
	void playUpgradeAni();
	//void animationCallback(cocostudio::Armature * pAr, cocostudio::MovementEventType type, const std::string& aniName);

protected:
	SoliderView();
	virtual ~SoliderView();

	virtual bool init(Solider* owner);
	void cleanup() override;

	std::string getFrameName(int type, eCampType camp, int level);
protected:
	static const int ActionTagMain = 10000;
	Solider * mOwner;

	Sprite* mSprite;
};
