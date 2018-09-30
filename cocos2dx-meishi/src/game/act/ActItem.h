#pragma once

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "ActLayout.h"

using namespace cocos2d;

class ActItem :public ui::Layout
{
public:
	ActItem(ACTTYPE);
	~ActItem(void);
	static ActItem* createByType(ACTTYPE);

	virtual bool init();
	void initWithNode(Node* mNode);

	void updateUI();
	void setSelect(bool);
	ACTTYPE getActType(){return mType;}
private:
	ACTTYPE mType;
	Node* m_itemNode;

	ui::ImageView* mActImage;
	Sprite* mTitle;
};

