#include "ProDialog.h"
#include "prop/PropItem.h"
#include "game/prop/PropsControl.h"
#include "game/data/TableManager.h"
#include "EnvSet.h"
#include "game/net/UserObj.h"
void ProDialog::onShow()
{
	initPage();
}

bool ProDialog::init()
{
	bool ret = false;
	do
	{
		mRoot = CSLoader::getInstance()->createNode("ui/ProDialog.csb");
		mRoot->setAnchorPoint(Vec2(0.5,0.5));
		CC_BREAK_IF(!mRoot);
		this->addChild(mRoot);	
		
		Button *btn_back = dynamic_cast<Button*>(mRoot->getChildByName("btn_back"));
		btn_back->addClickEventListener(CC_CALLBACK_0(ProDialog::backCallBack, this));


		//initPage();

		ret = true;
	} while (0);


	return ret;
}

void ProDialog::backCallBack()
{
	this->setShow(false	);
}

void ProDialog::initPage()
{
	
	auto curPro = sgPropsControl()->getCurPropType();
	int propId = sgPropsControl()->getCurPropId();

	if (propId == -1) return;

	auto proInfo = sgTableManager()->getPropInfoById(propId);

	mTextName = dynamic_cast<Text*>(mRoot->getChildByName("Text_Name")); 
	mTextName->setText(proInfo->name);

	mTextNum = dynamic_cast<Text*>(mRoot->getChildByName("Text_num"));
	mTextNum->setText(CCString::createWithFormat("+%d", sgUserObj()->getProp(proInfo->id))->getCString());

	mTextExplain1 = dynamic_cast<Text*>(mRoot->getChildByName("Text_2"));
	mTextExplain1->setText(proInfo->explain2);

	mTextExplain2 = dynamic_cast<Text*>(mRoot->getChildByName("Text_3"));
	mTextExplain2->setText(proInfo->explain1);


	mIcon = dynamic_cast<Node*>(mRoot->getChildByName("node_icon"));
	mIcon->removeAllChildren();
	auto tempIcon = Sprite::createWithSpriteFrameName(proInfo->icon);
	tempIcon->setAnchorPoint(Point(0.5, 0.5));
	tempIcon->setScale(0.5);
	tempIcon->setPosition(Point::ZERO);
	mIcon->addChild(tempIcon);
}
