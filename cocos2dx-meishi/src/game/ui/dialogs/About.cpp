#include "About.h"
#include "GamePrecedure.h"
#include "game/net/UserObj.h"
#include "channel/ChannelApi.h"
#include "EnvSet.h"
#include "SoundManager.h"

void About::onMenuItemAction(std::string cName, cocos2d::Ref* sender)
{
	if (cName == "onFailClose")
	{
		this->setShow(false);

		return;
	}
	else if (cName == "onMore")
	{
		//java 
		sgChannelApi()->callMoreGame();
	}
}

bool About::init()
{
	bool ret = false;
	do
	{
		mRoot = CSLoader::getInstance()->createNode("ui/About.csb");
		mRoot->setAnchorPoint(Vec2(0.5, 0.5));
		CC_BREAK_IF(!mRoot);
		mRoot->setPosition(Vec2(25,-35));
		this->addChild(mRoot);	
		auto btn = dynamic_cast<ui::Button*>(mRoot->getChildByName("Button_2"));
		
		//if (sgChannelApi()->getChannelId() == ChannelApi::eChannelID::ChannelID_DianXin)
		//	btn->setVisible(true);
		//else
		//	btn->setVisible(false);

		//mVersion = dynamic_cast<ui::Text*>(mRoot->getChildByName("Text_Version"));
		//mVersion->setText(VERSION_CODE);

		onLocateClickCallback(mRoot);

		ret = true;
	} while (0);


	return ret;
}

void About::onShow()
{
	
}

About::About() :mVersion(NULL)
{

}

About::~About()
{

}
