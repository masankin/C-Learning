#include "InfoPanel.h"
#include "cocostudio/ActionTimeline/CSLoader.h"

#include "EnvSet.h"
#include "game/net/UserObj.h"
#include "sync/SyncControl.h"
#include "data/TableManager.h"
#include "ui/PageFunction.h"
#include "ui/dialogs/GenerlDialog.h"
#include "game/friend/FriendControl.h"
#include "core/SoundManager.h"
#include "utils/Comm.h"
#include "core/WordChecker.h"
#include "utils/HeadIconUtils.h"

InfoPanel::InfoPanel():
mUin(0), mType(E_TYPE_NORMAL)
{
	sgMessageManager()->regisiterMessageHandler(EMT_CHANGE_PLAYER_ICON, this);
	sgMessageManager()->regisiterMessageHandler(EMT_CHANGE_SIGN, this);
	sgMessageManager()->regisiterMessageHandler(EMT_CHANGE_EXTRA_VISIBLE, this);
	sgMessageManager()->regisiterMessageHandler(EMT_GET_EXTRA, this);
}

InfoPanel::~InfoPanel()
{
	sgMessageManager()->removeMessageHandler(this);
	Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(this);
}

void InfoPanel::onMenuItemAction(std::string name, cocos2d::Ref* sender)
{
	if (name == "onBack")
	{
		this->setShow(false);
		sgMessageManager()->sendMessage(EMT_SELECTWORD_SCHEDULE, "");
		return;
	}
	else if (name == "onRename")
	{
		PageFunction::Get()->gotoFunction(MSG_FUC_NAMEING_DIALOG);
		this->setShow(false);
	}
	else if (name == "onApply")
	{
		if (sgFriendControl()->isFriend(mUin))
		{
			GenerlDialog::show(sgTextHelper(TEXT_COMM_TIPS), sgTextHelper(TEXT_FRIEND_IS_FRIEND));
		}
		else
		{
			sgFriendControl()->getProxy()->sendAddFriendRequest(mUin);
			auto pButton = ((ui::Button*)(sender));
			pButton->setEnabled(false);
			pButton->setBright(false);
			((Sprite*)(pButton->getChildByName("Sprite_54")))->setSpriteFrame("ui/Friend/btn_g_1.png");
			sgMessageManager()->sendMessage(EMT_FRIEND_UPDATE, Value(E_FRIEND_ADD_REQUEST_DONE).asString());
		}
	}
	else if (name == "onDelete")
	{
		GenerlDialog::show(sgTextHelper(TEXT_COMM_TIPS), sgTextHelper(TEXT_FRIEND_DELETE), CC_CALLBACK_1(InfoPanel::deleteFriendCallback, this));
	}
	else if (name == "onHead")
	{
		PageFunction::Get()->gotoFunction(MSG_FUC_SET_HEAD);
	}
	else if (name == "onSign")
	{
		signIsVisible(false);
		mSignText->attachWithIME();
		mSignText->setTouchEnabled(true);
	}
	else if (name == "onSignOk")
	{
		std::string chackStr = "";
		int ret = checkCharacter(mSignText->getString(), chackStr, 15);
		if (ret != 1)
		{
			if (chackStr.length() > 0)
			{
				showTextTip(StringUtils::format(sgTextHelper(EPI_SIGN_ERROR_TWO).c_str(), chackStr.c_str()));
			}
			else
			{
				showTextTip(sgTextHelper(EPI_SIGN_ERROR_ONE),Point::ZERO,Size(648,80));
			}
			return;
		}

		std::string sName = WordChecker::Get()->replaceSensitiveWord(mSignText->getString(), "*", 1);
		if (mSignText->getString().compare(sName)) {
			showTextTip(sgTextHelper(EPI_SIGN_ERROR_ONE), Point::ZERO, Size(648, 80));
			return;
		}

		sgSyncControl()->getSyncProxy()->sendChangeSignRqst(mSignText->getString());
	}
	else if (name == "onSignFail")
	{
 		signIsVisible(true);
		mSignText->setString(mPreSignStr);
		mSignText->detachWithIME();
		mSignText->setTouchEnabled(false);
	}
	else if (name == "onHonor")
	{
		sgPageFunction()->gotoFunction(MSG_FUC_BADGE);
		this->setShow(false);
	}
	else if (name == "onExtra")
	{
		sgSyncControl()->getSyncProxy()->sendExtraInfoRqst(mUin);
	}
	else if (name == "onExtraBack")
	{	
		mExtraBtn->setEnabled(false);
		if (mArmatureBg)
			mArmatureBg->getAnimation()->play("end2", -1, 0);
		mPanelExtra->setVisible(false);
		mLockNode->setVisible(false);
	}
	else if (name == "onUnVisible")
	{
		showTextTip(sgTextHelper(EPI_UNVISIBLE));
	}
	else if (name == "onLock")
	{
		sgSyncControl()->getSyncProxy()->sendChangeInforVisibleRqst(true);
	}
	else if (name == "onUnLock")
	{
		sgSyncControl()->getSyncProxy()->sendChangeInforVisibleRqst(false);
	}
}

void InfoPanel::onReceiveMassage(GameMessage * message)
{
	if (message->getMessageType() == EMT_CHANGE_PLAYER_ICON)
	{
		MessageChangePlayerIcon* pMes = dynamic_cast<MessageChangePlayerIcon*>(message);
#ifdef MGAME_PLATFORM_MPLUS
		FriendSystem::setPlayerIconSpr(mHeadIcon, pMes->getIconId(), sgMplusControl()->getThirdPartyIcon());
#else
		sgHeadIconUtils()->changeIcon(mHeadIcon, pMes->getIconId());
#endif
	}
	else if (message->getMessageType() == EMT_CHANGE_SIGN)
	{
		signIsVisible(true);
		mPreSignStr = mSignText->getString();
		mSignText->detachWithIME();
		mSignText->setTouchEnabled(false);
	}
	else if (message->getMessageType() == EMT_CHANGE_EXTRA_VISIBLE)
	{
		MessageChangeExtraVisible* pMes = dynamic_cast<MessageChangeExtraVisible*>(message);
		bool tag = pMes->getIsVisible();
		setLockVisible(tag);
		if (tag)
			showTextTip(sgTextHelper(EPI_EXTRA_UNLOCK));
		else
			showTextTip(sgTextHelper(EPI_EXTRA_LOCK));
	}
	else if (message->getMessageType() == EMT_GET_EXTRA)
	{
		MessageGetExtra* pMes = dynamic_cast<MessageGetExtra*>(message);
		updatePanelExtra(pMes);
	}
}

void InfoPanel::windowAniCallback(cocostudio::Armature * pAr, cocostudio::MovementEventType type, const std::string& aniName)
{
	if (type == cocostudio::COMPLETE && aniName == "born2")
	{
		cocostudio::MovementData* pMovementData = pAr->getAnimation()->getAnimationData()->getMovement("stand2");
		if (pMovementData != NULL)
		{
			pAr->getAnimation()->play("stand2",-1,0);
		}
	}

	if (type == cocostudio::COMPLETE && aniName == "stand2")
	{
		mExtraBtn->setEnabled(true);
		mExtraBtn->setCallbackName("onExtraBack");
	}

	if (type == cocostudio::COMPLETE && aniName == "end2")
	{
		mExtraBtn->setEnabled(true);
		mExtraBtn->setCallbackName("onExtra");
	}

	ActionWindow::windowAniCallback(pAr, type, aniName);
}

void InfoPanel::onFrameEvent(cocostudio::Bone *bone, const std::string& evt, int originFrameIndex, int currentFrameIndex)
{
	if (strcmp(evt.c_str(), "atk") == 0)
	{
		mPanelExtra->setVisible(true);
		bool btn_enable = (mType == E_TYPE_PERSONAL) ? true : false;
		mLockNode->setVisible(btn_enable);
	}
}


void InfoPanel::deleteFriendCallback(cocos2d::Ref* sender)
{
	GenerlDialog::hide();
	sgFriendControl()->deleteFriendItem(mUin);
	this->setShow(false);
}

InfoPanel* InfoPanel::create()
{
	InfoPanel* pret = new InfoPanel();
	if (pret && pret->initUI())
	{
		pret->autorelease();
		return pret;
	}
	CC_SAFE_DELETE(pret);
	return NULL;
}

bool InfoPanel::initUI()
{
	bool ret = false;
	do
	{
		cocostudio::ArmatureDataManager::getInstance()->addArmatureFileInfo("fla_GeRenXinXi.xml");

		mRoot = CSLoader::createNode("ui/InfoDialog.csb");
		CC_BREAK_IF(!mRoot);

		mRoot->setAnchorPoint(Vec2(0.5, 0.5));

		setPopType(E_POP_TYPE_NONE);
		setAnimation("fla_GeRenXinXi");
		this->addChild(mRoot);
		onLocateClickCallback(mRoot);

		mPanelExtra = dynamic_cast<Node*>(mRoot->getChildByName("Panel_extra"));
		changeBone(mArmatureBg, "GeRenXinXi_DangAn", mPanelExtra);

		mPanelIcon = dynamic_cast<Node*>(mRoot->getChildByName("Panel_icon"));
		changeBone(mArmatureBg, "GeRenXinXi_TouXiang", mPanelIcon);

		mPanelInfo = dynamic_cast<Node*>(mRoot->getChildByName("Panel_info"));
		changeBone(mArmatureBg, "GeRenXinXi_Neirong", mPanelInfo);

		Button* closeBtn = dynamic_cast<Button*>(mRoot->getChildByName("Button_60"));
		changeBone(mArmatureBg, "GeRenXinXi_GuanBi", closeBtn);

		mExtraBtn = dynamic_cast<Widget*>(mRoot->getChildByName("Button_extend"));
		changeBone(mArmatureBg, "guge_AnNiu", mExtraBtn);

		mLockNode = dynamic_cast<Node*>(mRoot->getChildByName("LockNode"));
		mLockBtn = dynamic_cast<Button*>(mLockNode->getChildByName("Button_lock"));
		mUnLockBtn = dynamic_cast<Button*>(mLockNode->getChildByName("Button_unlock"));
		changeBone(mArmatureBg, "guge_Suo", mLockNode);

		auto panelDown = dynamic_cast<Node*>(mRoot->getChildByName("Panel_down"));
		changeBone(mArmatureBg, "GeRenXinXi_DengJi", panelDown);
		mSignBtn = dynamic_cast<Button*>(panelDown->getChildByName("Button_54_0_0"));
		mSignOkBtn = dynamic_cast<Button*>(panelDown->getChildByName("Button_54_0"));
		mSignFailBtn = dynamic_cast<Button*>(panelDown->getChildByName("Button_54"));
		mSignBg = dynamic_cast<Sprite*>(panelDown->getChildByName("Sprite_52"));
		mSignText = dynamic_cast<TextField*>(panelDown->getChildByName("TextField_sign"));
		mSignText->addEventListenerTextField(this, textfieldeventselector(InfoPanel::TextFieldCallback));
		mHeadIcon = dynamic_cast<Sprite*>(mPanelIcon->getChildByName("Player_Icon"));
		ret = true;

	} while (0);


	return ret;
}

void InfoPanel::init(const fgame::CSQueryPlayerInfoResp &resp)
{
	mUin = resp.uin();
	mType = (eInfoPanelType)sgUserObj()->getCurInfoType();
	mSignText->setTouchEnabled(false);
	mPanelExtra->setVisible(false);
	mLockNode->setVisible(false);
	updatePanelIcon(resp);
	updatePanelInfo(resp);
	updateOtherInfo(resp);
}

void InfoPanel::updatePanelIcon(const fgame::CSQueryPlayerInfoResp &resp)
{
	Sprite* vipBg = dynamic_cast<Sprite*>(mPanelIcon->getChildByName("info_vip0_22"));
	if (resp.has_viplevel())
	{
		int vipLv = resp.viplevel();
		vipBg->setSpriteFrame(StringUtils::format("ui/InfoDialog/info_vip%d.png", vipLv));
	}
	else
	{
		vipBg->setSpriteFrame("ui/InfoDialog/info_vip0.png");
	}
	
	if (mType == E_TYPE_PERSONAL)
	{
#ifdef MGAME_PLATFORM_MPLUS
		FriendSystem::setPlayerIconSpr(mHeadIcon, resp.icon(), sgMplusControl()->getThirdPartyIcon());
#else
		FriendSystem::setPlayerIconSpr(mHeadIcon, resp.icon());
#endif
	}
	else
	{
		sgHeadIconUtils()->changeIcon(mHeadIcon, resp.icon());
	}

	bool tag = (mType == E_TYPE_PERSONAL) ? true : false;

	ImageView* headImg = dynamic_cast<ui::ImageView*>(mPanelIcon->getChildByName("Player_Icon_Bg"));
	headImg->setEnabled(tag);

	Button* iconBtn = dynamic_cast<Button*>(mPanelIcon->getChildByName("Button_1_0"));
	iconBtn->setVisible(tag);

	Button* honorBtn = dynamic_cast<Button*>(mPanelIcon->getChildByName("Button_22"));
	int title = resp.title();
	bool titleTag = true;
	if (title == 0)
	{
		titleTag = false;
		title = 1;
		honorBtn->setEnabled(false);
	}
	else
	{
		honorBtn->setEnabled(true && tag);
	}
	auto titleStr = GetPlayerBadgeIconNameById(title, titleTag);
	honorBtn->loadTextures(titleStr, titleStr,"",ui::Widget::TextureResType::PLIST);
}

void InfoPanel::updatePanelInfo(const fgame::CSQueryPlayerInfoResp &resp)
{
	Button* nameBtn = dynamic_cast<Button*>(mPanelInfo->getChildByName("Button_1"));
	nameBtn->setVisible((mType == E_TYPE_PERSONAL) ? true : false);

	Text* playerName = dynamic_cast<Text*>(mPanelInfo->getChildByName("Text_Name"));

#ifdef  MGAME_PLATFORM_MPLUS
	if (playerName) {
		playerName->setColor(Color3B(126, 65, 24));
		playerName->setFontName(DEFAULT_FONT);
	}
#endif

	playerName->setString(resp.name());

	auto pBtn = dynamic_cast<Button*>(mPanelInfo->getChildByName("Button_Area"));
	NameArea* pArea = sgTableManager()->getNameArea(resp.area());
	if (pArea){
		pBtn->setTitleText(pArea->mName);
		pBtn->loadTextureNormal(StringUtils::format("ui/guide/Area_%d.png", resp.area() > AREA_NAME_COUNT ? (resp.area() - AREA_NAME_COUNT) : resp.area()), TextureResType::PLIST);
	}


	Text* rankInfo1 = dynamic_cast<Text*>(mPanelInfo->getChildByName("Rank_Info_1"));
	Sprite* rankInfo1_1 = dynamic_cast<Sprite*>(mPanelInfo->getChildByName("Rank_Info_1_1"));

	auto pGradeData = sgTableManager()->getGradeData(resp.score());
	if (pGradeData)
	{
		auto emblemFile = StringUtils::format("ui/Icon2/ranking_Emblem-%02d.png", pGradeData->getID());
		rankInfo1_1->setSpriteFrame(emblemFile);
		rankInfo1_1->setScale(0.5);
		int rankGroup = pGradeData->getGroup();
		if (resp.rank() > 0 && rankGroup > 0)
		{
			int group = TEXT_RANK_GROUP_1 - 1 + rankGroup;
			rankInfo1->setText(__String::createWithFormat(sgTextHelper(group).c_str(), resp.rank())->getCString());
		}
		else
		{
			rankInfo1->setText(sgTextHelper(TEXT_PVP_NO_RANK));
		}
	}
	else
	{
		rankInfo1->setText(sgTextHelper(TEXT_PVP_NO_RANK));
		rankInfo1_1->setVisible(false);
	}
	rankInfo1->ignoreContentAdaptWithSize(false);
	rankInfo1_1->setPositionX(rankInfo1->getPositionX()-rankInfo1->getContentSize().width - 10.0f);

	Text* rankInfo2 = dynamic_cast<Text*>(mPanelInfo->getChildByName("Rank_Info_2"));
	std::string scoreStr = "----";
	if (resp.score() > 0)
		scoreStr = StringUtils::format("%d", resp.score());
	rankInfo2->setString(scoreStr);

	Text* rankInfo3 = dynamic_cast<Text*>(mPanelInfo->getChildByName("Rank_Info_3"));
	rankInfo3->setString(StringUtils::format("%d", resp.glyphslevel()));

	Text* rankInfo4 = dynamic_cast<Text*>(mPanelInfo->getChildByName("Rank_Info_4"));
	auto checkPoint = sgTableManager()->getCheckPointByID(resp.maxcheckpoint());
	std::string pointStr = "1-1";
	if (checkPoint!=NULL)
		pointStr = StringUtils::format("%d", checkPoint->owner) + "-" + checkPoint->levelName;
	rankInfo4->setString(pointStr);

	ImageView* lvbg = dynamic_cast<ImageView*>(mPanelInfo->getChildByName("Rank_Info_LvBg"));
	ImageView* rankInfo5 = dynamic_cast<ImageView*>(lvbg->getChildByName("Rank_Info_5"));
	int level = resp.level();
	if (level == 0)
		level = 1;
	rankInfo5->loadTexture(StringUtils::format("ui/general/Achieve_dj_%d.png",level),TextureResType::PLIST);
	const AchievePointAward* pAwardPoint = sgTableManager()->getAchievePointAwardById(level);
	lvbg->loadTexture(pAwardPoint->icon, TextureResType::PLIST);
}

void InfoPanel::updatePanelExtra(MessageGetExtra* pMes)
{
	//mPanelExtra->setVisible(true);
	//mLockNode->setVisible(true);

	Text* extra1 = dynamic_cast<Text*>(mPanelExtra->getChildByName("Rank_Extra_1"));
	extra1->setString(StringUtils::format(sgTextHelper(EPI_CHANG).c_str(), pMes->getTotalPvpCount())); 

	Text* extra2 = dynamic_cast<Text*>(mPanelExtra->getChildByName("Rank_Extra_2"));
	extra2->setString(StringUtils::format(sgTextHelper(EPI_CHANG).c_str(), pMes->getTotalPveCount()));

	Text* extra3 = dynamic_cast<Text*>(mPanelExtra->getChildByName("Rank_Extra_3"));
	extra3->setString(StringUtils::format(sgTextHelper(EPI_CHANG).c_str(), pMes->getMaxPvpWinCount()));

	Text* extra4 = dynamic_cast<Text*>(mPanelExtra->getChildByName("Rank_Extra_4"));
	std::string extraStr = sgTextHelper(TEXT_PVP_NO_RANK);
	if (pMes->getMaxPvpPoint() > 0)
	{
		auto extraData = sgTableManager()->getGradeData(pMes->getMaxPvpPoint());
		if (extraData != NULL)
			extraStr = sgTextHelper(extraData->getGroup()+13000);
	}
	extra4->setText(extraStr);

	Text* extra5 = dynamic_cast<Text*>(mPanelExtra->getChildByName("Rank_Extra_5"));
	extra5->setString(StringUtils::format(sgTextHelper(EPI_CI).c_str(), pMes->getTotalPvpUseItem()));

	Text* extra6 = dynamic_cast<Text*>(mPanelExtra->getChildByName("Rank_Extra_6"));
	extra6->setString(StringUtils::format(sgTextHelper(EPI_CI).c_str(), pMes->getTotalPveUseItem()));

	bool btn_visible = (mExtraIsVisible == 1) ? true : false;
	setLockVisible(btn_visible);		

	bool btn_enable = (mType == E_TYPE_PERSONAL) ? true : false;
	mLockBtn->setEnabled(btn_enable);
	mUnLockBtn->setEnabled(btn_enable);

	mExtraBtn->setEnabled(false);
	if (mArmatureBg)
		mArmatureBg->getAnimation()->play("born2", -1, 0);
}

void InfoPanel::updateOtherInfo(const fgame::CSQueryPlayerInfoResp &resp)
{
	showLevel(resp.explevel(), resp.exp());

	mSignText->setString(resp.signature());
	mPreSignStr = mSignText->getString();

	auto panelDown = dynamic_cast<Node*>(mRoot->getChildByName("Panel_down"));
	Button* applyBtn = dynamic_cast<Button*>(panelDown->getChildByName("BtnApply"));
	Button* deletedBtn = dynamic_cast<Button*>(panelDown->getChildByName("BtnDelete"));
	if (mType == E_TYPE_FRIEND_MINE)
	{
		applyBtn->setVisible(false);
		deletedBtn->setVisible(true);
	}
	else
	{
		if ((sgFriendControl()->isFriend(mUin)) || (mType == E_TYPE_PERSONAL) || mUin == sgUserObj()->getUin())
		{
			applyBtn->setVisible(false);
		}		
		else
		{
			applyBtn->setVisible(true);
			applyBtn->setEnabled(true);
			applyBtn->setBright(true);
			((Sprite*)(applyBtn->getChildByName("Sprite_54")))->setSpriteFrame("ui/general/applyFriend.png");
		}

		deletedBtn->setVisible(false);
	}

	signIsVisible(true);

	bool tag = (mType == E_TYPE_PERSONAL) ? true : false;
	mSignBtn->setVisible(tag);

	mExtraIsVisible = resp.visible();
	if (!tag && (mExtraIsVisible == 0))
	{
		mExtraBtn->setCallbackName("onUnVisible");
	}
	else
	{
		mExtraBtn->setEnabled(true);
		mExtraBtn->setCallbackName("onExtra");
	}
}

void InfoPanel::signIsVisible(bool isVisible)
{
	mSignBtn->setVisible(isVisible);
	mSignOkBtn->setVisible(!isVisible);
	mSignFailBtn->setVisible(!isVisible);
	mSignBg->setVisible(!isVisible);
}

void InfoPanel::setLockVisible(bool isVisible)
{
	mExtraIsVisible = isVisible;
	mUnLockBtn->setVisible(isVisible);
	mLockBtn->setVisible(!isVisible);
}

void InfoPanel::showLevel(int level, int curEx)
{
	auto panelDown = dynamic_cast<Node*>(mRoot->getChildByName("Panel_down"));

	Text* playerLv = dynamic_cast<Text*>(panelDown->getChildByName("text_lv"));
	playerLv->setString(StringUtils::format("LV.%d", level));

	Text* playerLvEx = dynamic_cast<Text*>(panelDown->getChildByName("text_ex"));
	playerLvEx->setVisible((mType == E_TYPE_PERSONAL)?true:false);
	int maxEx = sgTableManager()->getLevelExp(level);
	playerLvEx->setString(StringUtils::format("%d/%d", curEx, maxEx));

	LoadingBar* lvBar = dynamic_cast<ui::LoadingBar*>(panelDown->getChildByName("bar_level"));
	lvBar->setPercent((float)curEx / (float)maxEx*100.0f);
}

void InfoPanel::TextFieldCallback(Ref* pSender, TextFiledEventType type)
{
	if (type == TextFiledEventType::TEXTFIELD_EVENT_INSERT_TEXT)
	{

	}
}

//void InfoPanel::onClose()
//{
//	ActionWindow::onClose();
//	CCTextureCache::getInstance()->removeTextureForKey("ui/Achieve/Achieve.png");
//	SpriteFrameCache::getInstance()->removeSpriteFramesFromFile("ui/Achieve/Achieve.plist");
//}
//
//void InfoPanel::onShow()
//{
//	ActionWindow::onShow();
//	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("ui/Achieve/Achieve.plist");
//}
