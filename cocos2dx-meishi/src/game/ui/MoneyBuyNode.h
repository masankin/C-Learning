#pragma once

#include "Constant.h"
#include "MessageManager.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
#include "ui/UIHelper.h"
#include "cocos2d.h"

USING_NS_CC;
using namespace ui;


class MoneyBuyNode : public Node, public MessageHandler
{
public:
	MoneyBuyNode();
	~MoneyBuyNode();

	static MoneyBuyNode*	create();

	virtual void onReceiveMassage(GameMessage * message);

	virtual bool init();

	void onMenuClicked(cocos2d::Ref*);

	//滚数字效果
	void numberEffect(int before, int number);
	void numberEffect(float dt);

protected:
	ui::Text *mDiamond;

	int		m_iCurrentNumber;
	int		m_iChangeNumber;
	int		m_iTargetNumber;
	int		m_iNumberSpeed;
	bool	m_bIsPlayNumberEffect;

	CC_SYNTHESIZE(bool, m_bIsPlayAnimation, IsPlayAnimation)
};
