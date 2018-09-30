#pragma once

#include "Constant.h"

#include "cocos2d.h"
USING_NS_CC;

class MyFont : public Node
{
public:
	MyFont(){};
	~MyFont(){};

	static MyFont*	create(std::string str);
	static MyFont*	create();

	bool init(std::string str);
	bool init();
	void setText(std::string str);
	void setScale(float scale);
	void setShadeColor(cocos2d::Color3B color);
	void setTextColor(cocos2d::Color3B color);

	std::string& getText();

protected:
	
	ui::TextBMFont* mShade;
	ui::TextBMFont* mFont;

	Node* mRoot;

	std::string mString;
};

//DEFINE_PAGE_READER(ArrowWidget);
