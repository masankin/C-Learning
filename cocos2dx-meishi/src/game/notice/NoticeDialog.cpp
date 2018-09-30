#include "NoticeDialog.h"
#include "game/ui//PageFunction.h"
#include "game/ui//system/Window.h"
#include "act/ActControl.h"
#include "MessageManager.h"
#include "SoundManager.h"
#include "achieve/AchieveControl.h"
#include "NoticeControl.h"
#include "friend/FriendControl.h"
#include "data/TableManager.h"
#include "winner/WinnerControl.h"
#include "data/MailData.h"
#include "mail/MailProxy.h"
#include "act/PowerGiftControl.h"

void NoticeDialog::onMenuItemAction(std::string cName, cocos2d::Ref* sender)
{  
	if (cName == "onClose")
	{
		this->setShow(false);
		return;
	 }
}

bool NoticeDialog::init()
{
	bool ret = false;
	do
	{
		mRoot = CSLoader::getInstance()->createNode("ui/NoticeDialog.csb");
		CC_BREAK_IF(!mRoot);
		this->addChild(mRoot);

		mRoot = dynamic_cast<Node*>(mRoot->getChildByName("uiAtlas_Notice"));
		mTextTitle =  dynamic_cast<ui::Text*>(mRoot->getChildByName("Text_Title"));
		mScrollText = dynamic_cast<ui::ScrollView*>(mRoot->getChildByName("ScrollView_1"));

		mStringLabel = StringLabel::create();
		mStringLabel->setContentSize(Size(650, 10));
		mStringLabel->setTextSize(25);
		mStringLabel->setAnchorPoint(Vec2(0, 1));
		mStringLabel->setFontName(mTextTitle->getFontName());
		mScrollText->addChild(mStringLabel);
		
		NoticeInfo* pInfo = dynamic_cast<NoticeInfo*>(sgNoticeControl()->getNoticeArray()->objectAtIndex(0));
		if (pInfo)
		{
			setContent(pInfo->getTitle(),pInfo->getNoticeContent());
			sgNoticeControl()->setNewNotice(false);
		}

		setPopType(E_POP_TYPE_BIG);
		onLocateClickCallback(mRoot);

		ret = true;
	} while (0);


	return ret;
}

void NoticeDialog::show(std::string stHead, std::string stContent)
{
	NoticeDialog* pRet = dynamic_cast<NoticeDialog*>(Window::Create("NoticeDialog.csb"));
	if (pRet && pRet->mTextTitle)
	{
		pRet->setContent(stHead,stContent);
		pRet->setShow(true);
	}
}


void NoticeDialog::setContent(std::string stHead,std::string stContent)
{
	mTextTitle->setText(stHead);

	PowerString ps;  
	ps.init(stContent.c_str());
	mStringLabel->setPowerString(&ps);
	Size _size = Size(mScrollText->getInnerContainerSize().width,mStringLabel->getRealyHight() > 370 ? mStringLabel->getRealyHight() : 370);
	mStringLabel->setPosition(Vec2(0,_size.height));
	mScrollText->setInnerContainerSize(_size);

}

NoticeDialog::NoticeDialog() :mTextTitle(0), mScrollText(0),mStringLabel(0),mShowTimes(0)
{

}

NoticeDialog::~NoticeDialog()
{

}

