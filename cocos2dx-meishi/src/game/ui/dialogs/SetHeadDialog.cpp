#include "SetHeadDialog.h"
#include "data/TableManager.h"
#include "sync/SyncControl.h"
#include "net/UserObj.h"
#include "utils/Comm.h"
#include "MessageManager.h"
#include "utils/HeadIconUtils.h"
#define ListMaxCount 5

#define VIP_HEAD_INDEX_BEGIN 16
#define VIP_MAX_LEVEL 3

SetHeadDialog::SetHeadDialog()
{
	sgMessageManager()->regisiterMessageHandler(EMT_CHANGE_PLAYER_ICON, this);
}

SetHeadDialog::~SetHeadDialog()
{
	sgMessageManager()->removeMessageHandler(this);
	mButtonList.clear();
}

void SetHeadDialog::onMenuItemAction(std::string name, cocos2d::Ref* sender)
{
	if (name == "onBack")
	{
		this->setShow(false);
	}
	else if (name == "onSelectHead")
	{
		setIconTouchEnable(false);
		CCLOG("[edward]======================= change head");
		int id = ((Node*)sender)->getTag();
		// vip
		if (id >= VIP_HEAD_INDEX_BEGIN && id <= VIP_HEAD_INDEX_BEGIN + VIP_MAX_LEVEL) {
			int needVipLevel = id - VIP_HEAD_INDEX_BEGIN + 1;
			int currVipLevel = sgUserObj()->getPersonalInfo()->viplevel();//sgUserObj()->getVipLevel();
			if (currVipLevel < needVipLevel) {
				//show tips 需要 needVipLevel
				if (needVipLevel > 0 && needVipLevel <= VIP_MAX_LEVEL) {
					showTextTip(sgTextHelper(tipsIndex[needVipLevel - 1]));
					setIconTouchEnable(true);
					return;
				}
			}
		}

#ifdef MGAME_PLATFORM_MPLUS
		sgSyncControl()->getSyncProxy()->sendChangePlayerIcon(id, sgMplusControl()->getThirdPartyIcon());		
#else
		sgSyncControl()->getSyncProxy()->sendChangePlayerIcon(id,"");
#endif

	}
}

void SetHeadDialog::onReceiveMassage(GameMessage * message)
{
	if (message->getMessageType() == EMT_CHANGE_PLAYER_ICON)
	{
		MessageChangePlayerIcon* pMes = dynamic_cast<MessageChangePlayerIcon*>(message);
		sgUserObj()->getPersonalInfo()->set_iconid(pMes->getIconId());//sgUserObj()->setPlayerIconID(pMes->getIconId());
		if (isShow())
		{
			showTextTip(sgTextHelper(TEXT_FRIEND_CHANGE_HEAD));
		}
		
		this->setShow(false);
	}
}

bool SetHeadDialog::init()
{
	if (!Window::init())
		return false;

	tipsIndex[0] = TEXT_VIP1_TIPS;
	tipsIndex[1] = TEXT_VIP2_TIPS;
	tipsIndex[2] = TEXT_VIP3_TIPS;

	mButtonList = std::vector<Widget*>();

	mRoot = CSLoader::createNode("ui/SetHeadDialog.csb");
	mRoot->setAnchorPoint(Vec2(0.5, 0.5));
	addChild(mRoot);

	auto pBgList = dynamic_cast<ListView*> (mRoot->getChildByName("ListView_bg"));

	auto pHeadList = dynamic_cast<ListView*> (mRoot->getChildByName("ListView_1"));
	pHeadList->removeFromParentAndCleanup(false);

	auto pHeadButton = dynamic_cast<Button*> (mRoot->getChildByName("Button_1"));
	pHeadButton->setPosition(Vec2::ZERO);
	pHeadButton->setEnabled(false);
	pHeadButton->setVisible(false);
	//pHeadButton->removeFromParentAndCleanup(false);

	int rowSize = ::ceil((float)HeadMaxCount / ListMaxCount);
	for (int i = 0; i < rowSize; i++)
	{
		auto pListView = pHeadList->clone();
		pListView->setTag(i);
		pBgList->addChild(pListView);
	}

	int num = HeadMaxCount - 3;
#ifdef MGAME_PLATFORM_MPLUS
	num = HeadMaxCount;
#endif
	
	for (int i = 0; i < num; i++)
	{
		auto pListView = dynamic_cast<ListView*> (pBgList->getChildByTag(i / ListMaxCount));
		auto pHeadCell = CSLoader::createNode("ui/HeadCell.csb");
		pHeadCell->setAnchorPoint(Vec2(0.5, 0.5));
		auto sprite = dynamic_cast<Sprite*>(pHeadCell->getChildByName("Sprite_1"));

#ifdef MGAME_PLATFORM_MPLUS

			if ((i + 1) == HeadMaxCount){
				if (!sgMplusControl()->getThirdPartyIcon().empty())
					sprite->setTexture(sgMplusControl()->getThirdPartyIcon());
				else
					break;
			}
			else{
				sprite->setSpriteFrame(FriendSystem::GetPlayerIconNameFromId(i + 1));
			}
#else
		if (i == 0)
		{
			sgHeadIconUtils()->changeIcon(sprite, sgUserObj()->getHeadIconUrl());
		}
		else
		{
			sgHeadIconUtils()->changeIcon(sprite, String::createWithFormat("%d", i + 1)->getCString());
		}
#endif

		auto panelCell = Layout::create();
		panelCell->setContentSize(pHeadButton->getContentSize());
		panelCell->setAnchorPoint(Vec2(0.5, 0.5));
		panelCell->addChild(pHeadCell);
		auto pButton = dynamic_cast<ui::Button*>(pHeadCell->getChildByName("HeadBtn"));
		pButton->setCallbackType(pButton->getCallbackType());
		pButton->setCallbackName(pButton->getCallbackName());
		pHeadCell->setPosition(pHeadButton->getContentSize() / 2);
		pButton->setTag(i + 1);
		mButtonList.push_back(pButton);
		pListView->pushBackCustomItem(panelCell);
		// vip head
		if ((i + 1) >= VIP_HEAD_INDEX_BEGIN) {
			int needVipLevel = i + 1 - VIP_HEAD_INDEX_BEGIN + 1;
			int currVipLevel = sgUserObj()->getPersonalInfo()->viplevel();//sgUserObj()->getVipLevel();
			if (currVipLevel < needVipLevel) {
				// 图标变灰，加锁
				if (needVipLevel > 0 && needVipLevel <= VIP_MAX_LEVEL) {
					sprite->setSpriteFrame(FriendSystem::GetPlayerGrayIconNameByVipLevel(needVipLevel));
					pButton->loadTextureNormal("ui/main/Head_bg_gray.png", TextureResType::PLIST);
					auto lockText = dynamic_cast<ui::Text*>(pHeadCell->getChildByName("LockLabel"));
					if (lockText) {
						lockText->ignoreContentAdaptWithSize(false);
						lockText->setTextAreaSize(Size(100, 60));
						lockText->setVisible(true);
						lockText->setText(sgTextHelper(tipsIndex[needVipLevel - 1]));
					}
				}
			}
		}
	}

	onLocateClickCallback(mRoot);
	return true;
}

void SetHeadDialog::onShow()
{
	Window::onShow();

	//highlight
}

void SetHeadDialog::onEnter()
{
	ActionWindow::onEnter();
	setIconTouchEnable(true);
}

void SetHeadDialog::setIconTouchEnable(bool isTouch)
{
	for (int i = 0; i < mButtonList.size(); i++)
	{
		mButtonList.at(i)->setTouchEnabled(isTouch);
	}
}
