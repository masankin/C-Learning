#include "LittleTips.h"
#include "GamePrecedure.h"
#include "data/TableManager.h"
#include "core/ResManager.h"

#define TIPS_COUNTS 10
#define TIPS_START_TAG 11001

LittleTips::LittleTips(){

}

LittleTips::~LittleTips(){

}

LittleTips* LittleTips::create(EDT dType, bool isChange)
{
	LittleTips *littleTips = new (std::nothrow) LittleTips;
	if (littleTips && littleTips->init(dType, isChange))
	{
		littleTips->autorelease();
		return littleTips;
	}
	CC_SAFE_DELETE(littleTips);
	return nullptr;
}

LittleTips* LittleTips::create(EDT texType, std::string textStr, bool isPerson, Point panelPos, bool isChange){
	LittleTips *littleTips = new (std::nothrow) LittleTips;
	if (littleTips && littleTips->init(texType, textStr, isPerson, panelPos, isChange))
	{
		littleTips->autorelease();
		return littleTips;
	}
	CC_SAFE_DELETE(littleTips);
	return nullptr;
}

bool LittleTips::init(){
	if (!Node::create())
		return false;

	ms = 1;
	preIndex = -1;
	mTipsPanel = ImageView::create("ui/battle/tipsPanel.png",TextureResType::PLIST);
	mTipsPanel->setScale9Enabled(true);
	mTipsPanel->ignoreContentAdaptWithSize(false);

	mTipsText = Text::create();
	mTipsText->setFontSize(22);
	mTipsText->setFontName("font.TTF");
	mTipsText->setTextColor(Color4B(133, 72, 15, 255));
	mTipsText->setString("");
	mTipsText->setPosition(Point(mTipsPanel->getContentSize().width*0.5, mTipsPanel->getContentSize().height*0.55));
	mTipsPanel->setVisible(false);
	mTipsPanel->addChild(mTipsText, 1000);
	mTipsPanel->setAnchorPoint(Point(0.0, 0.5));
	return true;
}

bool LittleTips::init(EDT dType, bool isChange){
	
	bool tag = init();	
	mTipsDirection = dType;
	addChild(mTipsPanel);

	auto visibleSize = Director::getInstance()->getVisibleSize();
	mTips = sgResManager()->getAramatur("fla_battle_xiaotishi");//cocostudio::Armature::create("fla_battle_xiaotishi");//侷奇抗耙洋
	mTips->setVisible(false);
	addChild(mTips, mTipsPanel->getZOrder() + 1);

	schedule(schedule_selector(LittleTips::change), 8.0f);

	return tag;
}

bool LittleTips::init(EDT texType, std::string textStr, bool isPerson, Point panelPos, bool isChange){
	auto tag = init();
	mTipsPanel->addChild(mTipsText, 1000);

	auto visibleSize = Director::getInstance()->getVisibleSize();
	Node* node = Node::create();
	node->setPosition(panelPos);
	addChild(node,1000);

	node->addChild(mTipsPanel);
	mTipsText->setContentSize(Size(0.638*mTipsPanel->getContentSize().width, 0.592*mTipsPanel->getContentSize().height));
	mTipsText->ignoreContentAdaptWithSize(false);
	mTipsText->setString(textStr);
	mTipsText->setTextHorizontalAlignment(TextHAlignment::CENTER);
	mTipsText->setTextVerticalAlignment(TextVAlignment::CENTER);
	mTipsPanel->loadTexture("dhk.png");
	mTipsPanel->setVisible(true);

	if (isPerson)
	{
		auto person = ImageView::create("npc2small.png");//納俱沾奈
		person->ignoreContentAdaptWithSize(false);
		person->setAnchorPoint(Point::ZERO);
		person->setPosition(Point::ZERO);
		node->addChild(person);
		mTipsPanel->setPosition(Point(person->getContentSize().width*0.7, person->getContentSize().height*0.9));
	}
	else
	{
		mTipsPanel->setPosition(Point(panelPos.x, panelPos.y + mTipsPanel->getContentSize().height/2));
	}

	if (texType == EDT::EDT_LEFT)
	{
		node->setScaleX(-1);
		mTipsText->setScaleX(-1);
	}
	return tag;
}

void LittleTips::change(float t){
	if (ms == 1)
	{
		showTips();
		return;
	}
	auto actionBy3D = RotateBy::create(0.5, Vec3(60, 0, 0));
	mTipsText->runAction(Sequence::create(actionBy3D, CallFuncN::create(CC_CALLBACK_0(LittleTips::showTips, this)), NULL));
}

void LittleTips::showTips(){
	auto visibleSize = Director::getInstance()->getVisibleSize();

	srand((unsigned)time(NULL));
	int index = rand() % TIPS_COUNTS ;
	while (index == preIndex)
	{
		index = rand() % TIPS_COUNTS;
	}
	preIndex = index;

	const TextHelper* pTextHelper = sgTableManager()->getTextHelperByID(TIPS_START_TAG + index);
	int length = 0;// pTextHelper->text.length();

	auto str = pTextHelper->text;
	for (int i = 0; i < str.length(); ++i) {
		if (str[i] <= '10' && str[i] >= '0')
		{
			length += 2;
		}
		else
		{
			length++;
		}
	}
	

	mTips->setVisible(true);
	mTips->getAnimation()->play("play");
	mTipsPanel->setVisible(true);
	mTipsPanel->setContentSize(Size(0.0065*visibleSize.width*length, 0.1*visibleSize.height));
	if (mTipsDirection == EDT::EDT_LEFT)
		mTipsPanel->setPosition(Point(0.015*visibleSize.width, mTipsPanel->getContentSize().height*0.5));
	else
		mTipsPanel->setPosition(Point(0.985*visibleSize.width - mTipsPanel->getContentSize().width, mTipsPanel->getContentSize().height*0.5));

	mTipsText->setPosition(Point(mTipsPanel->getContentSize().width*0.5, mTipsPanel->getContentSize().height*0.5));
	mTipsText->setString(str);
	mTips->setPosition(Point(mTipsPanel->getPositionX() + visibleSize.width*0.05, mTipsPanel->getPositionY() + mTipsPanel->getContentSize().height*0.6));

	if (ms != 1)
	{
		auto actionBy3D = RotateBy::create(0.5, Vec3(-60, 0, 0));
		mTipsText->runAction(actionBy3D);
	}
	ms++;
}




