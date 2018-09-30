#include "SrvDownNoticeDialog.h"
#include "game/ui//PageFunction.h"
#include "game/ui//system/Window.h"
#include "utils/Comm.h"
#include "SoundManager.h"

void SrvDownNoticeDialog::onMenuItemAction(std::string cName, cocos2d::Ref* sender)
{  
	if (cName == "onClose")
	{
		Director::sharedDirector()->getNotificationNode()->onExit();
		Director::sharedDirector()->end();
		return;
	 }
}

bool SrvDownNoticeDialog::init()
{
	bool ret = false;
	do
	{
		mRoot = CSLoader::getInstance()->createNode("ui/SrvDownNoticeDialog.csb");
		CC_BREAK_IF(!mRoot);
		this->addChild(mRoot);
		mTextTitle =  dynamic_cast<ui::Text*>(mRoot->getChildByName("TextTitle"));
		mTextOpenTime = dynamic_cast<ui::Text*>(mRoot->getChildByName("TextOpenTime"));
		onLocateClickCallback(mRoot);

		ret = true;
	} while (0);


	return ret;
}

void SrvDownNoticeDialog::setContent(std::string stContent)
{
	std::vector<std::string > strVec = splitStr(stContent, "|");
	if (strVec.size() >= 2)
	{
		mTextTitle->setText(strVec[0]);
		mTextOpenTime->setText(strVec[1]);
	}

}

SrvDownNoticeDialog::SrvDownNoticeDialog() :mTextTitle(0), mTextOpenTime(0)
{

}

SrvDownNoticeDialog::~SrvDownNoticeDialog()
{

}

