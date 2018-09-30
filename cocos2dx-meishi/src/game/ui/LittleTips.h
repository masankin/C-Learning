#pragma once

#include "cocos2d.h"
#include "ui/UIText.h"
#include "ui/UIImageView.h"
#include "cocostudio/CCArmature.h"
USING_NS_CC;
using namespace ui;
using namespace cocostudio;

#define TIPS_UP_CONTROL_ACOUT 21 //小提示的关卡上限

class LittleTips : public Node
{
public:

	LittleTips();
	~LittleTips();

	enum EDT{//泡泡方向
		EDT_INVALID,
		EDT_LEFT,
		EDT_RIGHT
	};

	static LittleTips* create(EDT texType, bool isChange = true);//固定地方切换提示
	static LittleTips* create(EDT texType, std::string textStr, bool isPerson, Point panelPos, bool isChange = false);//提示固定

	bool init();
	bool init(EDT dType, bool isChange);
	bool init(EDT texType, std::string textStr, bool isPerson, Point panelPos, bool isChange);

	void change(float t);
	void showTips();

private:
	Armature* mTips;
	ImageView* mTipsPanel;
	Text* mTipsText;
	int mTipsDirection;
	int ms;
	int preIndex;
};
