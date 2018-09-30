#include "RankingDialog.h"
#include "GamePrecedure.h"
#include "ranking/RankingControl.h"
#include "ranking/RankingProxy.h"
#include "ui/dialogs/GenerlDialog.h"
#include "game/net/UserObj.h"
#include "SoundManager.h"
#include "ui/PageFunction.h"
#include "data/TableManager.h"
#include "utils/Comm.h"
#include "ExRichText.h"

USING_NS_CC;


void RankingDialog::SelectLabel(ImageView* image, bool selected)
{
	if (nullptr == image)
		return;

	if (selected)
	{
		image->loadTexture("ui/ranking/ranking_ButtonLabelHightlight.png", ui::TextureResType::PLIST);
		image->setContentSize(image->getVirtualRendererSize());

		Sprite* icon = nullptr;
		icon = dynamic_cast<Sprite*>(image->getChildByName("IconH"));
		if (icon)
			icon->setVisible(true);

		icon = dynamic_cast<Sprite*>(image->getChildByName("IconN"));
		if (icon)
			icon->setVisible(false);

	}
	else
	{
		image->loadTexture("ui/ranking/ranking_ButtonLabelNormal.png", ui::TextureResType::PLIST);
		image->setContentSize(image->getVirtualRendererSize());

		Sprite* icon = nullptr;
		icon = dynamic_cast<Sprite*>(image->getChildByName("IconH"));
		if (icon)
			icon->setVisible(false);

		icon = dynamic_cast<Sprite*>(image->getChildByName("IconN"));
		if (icon)
			icon->setVisible(true);
	}

}


std::string RankingDialog::GetGroupName(eRankingGroupType groupType)
{
	switch (groupType)
	{
	case RANKINGGROUP_DEFAULT:
		return "";
	case RANKINGGROUP_BRONZE:
		return sgTextHelper(TEXT_RANKING_GROUP_1);
	case RANKINGGROUP_SILVER:
		return sgTextHelper(TEXT_RANKING_GROUP_2);
	case RANKINGGROUP_GOLD:
		return sgTextHelper(TEXT_RANKING_GROUP_3);
	case RANKINGGROUP_DIAMOND:
		return sgTextHelper(TEXT_RANKING_GROUP_4);
	case RANKINGGROUP_KING:
		return "";
	default:
		return "";
	}
}


RankingDialog::RankingDialog() : mCurrentGroup(eRankingGroupType::RANKINGGROUP_DEFAULT), mTimeRemainingModel(E_NORMAL)
, mRankListView(nullptr)
, mSelectedGroupBtn(nullptr)
, mPlayerInfoNode(nullptr)
, mPortraitFrame(nullptr)
, mBoxAni(nullptr)
, mTargetWeek(7)
, mTargetTime("22:00")
/*
, mPlayerFrame(nullptr)
, mPvpResultFrame(nullptr)
, mPlayerRowFrame(nullptr)
, mPlayerRow(nullptr)*/
{
}


RankingDialog::~RankingDialog()
{
	sgRankingControl()->setView(nullptr);

	mRankListView = nullptr;
	mSelectedGroupBtn = nullptr;
	mPlayerInfoNode = nullptr;
	mPortraitFrame = nullptr;

	/*mPlayerFrame = nullptr;
	mPvpResultFrame = nullptr;
	mPlayerRowFrame = nullptr;
	mPlayerRow = nullptr;*/

	for (auto &e : mRowNodeRecycleList)
		e->release();
	mRowNodeRecycleList.clear();
}


void RankingDialog::onMenuItemAction(std::string cName, cocos2d::Ref* sender)
{
	if (cName == "onListBtn")
	{
		auto nodeSender = dynamic_cast<Node*>(sender);
		if (nodeSender)
		{
			auto dataObj = dynamic_cast<cocostudio::ObjectExtensionData*>(nodeSender->getUserObject());
			if (dataObj)
			{
				auto groupType = static_cast<eRankingGroupType>(atoi(dataObj->getCustomProperty().c_str()));
				onSelectList(sender, groupType);
			}
		}
	}
	else if (cName == "onReturn")
	{
		if (true == sgRankingControl()->getbLockDialogClose())
		{
			return;
		}

		this->setShow(false);
		sgPageFunction()->enableOptionButton(true);
		sgMessageManager()->sendMessage(EMT_SELECTWORD_SCHEDULE, "");
		return;
	}
	else if (cName == "onPlayAgain")
	{
		this->setShow(false);

		sgPageFunction()->gotoFunction(MSG_FUC_WIN_PVP);
		sgPageFunction()->enableOptionButton(false);
	}
	else if (cName == "onInfo")
	{
		RankingInfoDialog* pRet = dynamic_cast<RankingInfoDialog*>(Window::Create("RankingInfoNode.csb"));
		pRet->setShow(true);
	}
	else if (cName == "onAward")
	{	
		if (0 == sgRankingControl()->getRank())
		{
			showTextTip(sgTextHelper(TEXT_NO_RANK));
			return;
		}

		if (true == sgRankingControl()->getStatus())
		{
			//发送领奖协议
			sgRankingControl()->requestRankWeekAward();
		} 
		else
		{
			//打开奖品UI
			//this->setShow(false);
			//sgPageFunction()->enableOptionButton(true);

			//sgRankingControl()->setbLockDialogClose(true);
			sgPageFunction()->gotoFunction(MSG_FUC_LUA_WIN, "RankingAwardDialog.csb", 1);
		}
	}
}



bool RankingDialog::init()
{
	if (!ActionWindow::init())
	{
		return false;
	}

	mRoot = CSLoader::createNode("ui/RankingDialog.csb");
	if (nullptr == mRoot)
	{
		CCLOG("[error]RankingDialog::init : Create mRoot failed.");
		return false;
	}
	
	this->addChild(mRoot);

	onLocateClickCallback(mRoot);
	setPopType(E_POP_TYPE_NONE);
	
	// Init ui armature
	
	setAnimation("fla_PaiHangB");
	if (nullptr == mArmatureBg)
	{
		CCLOG("[error]RankingDialog::init : Init mArmatureBg failed.");
		return false;
	}


	sgRankingControl()->setView(this);


	// Close Button
	ui::Button* pClose = dynamic_cast<ui::Button*>(mRoot->getChildByName("BtnClose"));
	if (nullptr != pClose)
	{
		pClose->setPosition(Point::ZERO);
		changeBone(mArmatureBg, "PaiHangB_a11", pClose);
	}

	// Group Button
	ui::ImageView* pGroupBtn = nullptr;

	pGroupBtn = dynamic_cast<ui::ImageView*>(mRoot->getChildByName("BtnGroup1"));
	if (nullptr != pGroupBtn)
	{
		pGroupBtn->setPosition(Point::ZERO);
		changeBone(mArmatureBg, "PaiHangB_a2", pGroupBtn);
	}

	pGroupBtn = dynamic_cast<ui::ImageView*>(mRoot->getChildByName("BtnGroup2"));
	if (nullptr != pGroupBtn)
	{
		pGroupBtn->setPosition(Point::ZERO);
		changeBone(mArmatureBg, "PaiHangB_a3", pGroupBtn);
	}

	pGroupBtn = dynamic_cast<ui::ImageView*>(mRoot->getChildByName("BtnGroup3"));
	if (nullptr != pGroupBtn)
	{
		pGroupBtn->setPosition(Point::ZERO);
		changeBone(mArmatureBg, "PaiHangB_a4", pGroupBtn);
	}

	pGroupBtn = dynamic_cast<ui::ImageView*>(mRoot->getChildByName("BtnGroup4"));
	if (nullptr != pGroupBtn)
	{
		pGroupBtn->setPosition(Point::ZERO);
		changeBone(mArmatureBg, "PaiHangB_a5", pGroupBtn);
	}

	// ListFrame
	ui::ImageView* pListFrame = dynamic_cast<ui::ImageView*>(mRoot->getChildByName("RankingListFrame"));
	if (nullptr != pListFrame)
	{
		pListFrame->setPosition(Point(0, -20));
		changeBone(mArmatureBg, "PaiHangB_jiesuan", pListFrame);

		auto topMask = pListFrame->getChildByName("ListViewTopMask");
		if (topMask)
		{
			ui::Text* text = nullptr;

			text = dynamic_cast<ui::Text*>(topMask->getChildByName("TitleRanking"));
			if (text)
				text->setString(sgTextHelper(TEXT_RANKING_RANKING));

			text = dynamic_cast<ui::Text*>(topMask->getChildByName("TitleName"));
			if (text)
				text->setString(sgTextHelper(TEXT_RANKING_PLAYER));

			text = dynamic_cast<ui::Text*>(topMask->getChildByName("TitleWinRate"));
			if (text)
				text->setString(sgTextHelper(TEXT_RANKING_WINRATE));

			text = dynamic_cast<ui::Text*>(topMask->getChildByName("TitlePoint"));
			if (text)
				text->setString(sgTextHelper(TEXT_RANKING_POINT));
		}

		mRankListView = dynamic_cast<ui::ListView*>(pListFrame->getChildByName("RankListView"));
		if (mRankListView)
		{
			mRankListView->addEventListener((const ui::ScrollView::ccScrollViewCallback&)CC_CALLBACK_2(RankingDialog::onListViewScrollEvent, this));
		}
	}
	
	// Player info node
	mPlayerInfoNode = mRoot->getChildByName("PlayerInfoNode");
	if (nullptr != mPlayerInfoNode)
	{
		mPlayerInfoNode->setPosition(Point::ZERO);
		changeBone(mArmatureBg, "PaiHangB_ziti", mPlayerInfoNode);

		ui::Text* text = nullptr;

		text = dynamic_cast<ui::Text*>(mPlayerInfoNode->getChildByName("TextTitlePoint"));
		if (text)
			text->setString(sgTextHelper(TEXT_RANKING_POINT) + ":");

		text = dynamic_cast<ui::Text*>(mPlayerInfoNode->getChildByName("TextTitleWinRate"));
		if (text)
			text->setString(sgTextHelper(TEXT_RANKING_WINRATE) + ":");

		text = dynamic_cast<ui::Text*>(mPlayerInfoNode->getChildByName("TextTitleRoundCount"));
		if (text)
			text->setString(sgTextHelper(TEXT_RANKING_ROUNDCOUNT) + ":");

		text = dynamic_cast<ui::Text*>(mPlayerInfoNode->getChildByName("TextTitleScore"));
		if (text)
			text->setString(sgTextHelper(TEXT_RANKING_SCORE) + ":");

		mPanel_noRank = mPlayerInfoNode->getChildByName("Panel_noRank");
		mPanel_maxGrade = mPlayerInfoNode->getChildByName("Panel_maxGrade");
		mPanel_normal = mPlayerInfoNode->getChildByName("Panel_normal");
	}

	// ListFrame
	mPortraitFrame = dynamic_cast<ui::ImageView*>(mRoot->getChildByName("PortraitFrame"));
	if (nullptr != pGroupBtn)
	{
		mPortraitFrame->setPosition(Point::ZERO);
		changeBone(mArmatureBg, "PaiHangB_a7", mPortraitFrame);
	}

	mTextGetTime = dynamic_cast<ui::Text*>(mPlayerInfoNode->getChildByName("TextGetTime"));
	mTextGetTime->setString("");

	//添加宝箱动画
	mBtnAward = mPlayerInfoNode->getChildByName("BtnAward");

	mBoxAni = cocostudio::Armature::create("fla_XGBaoX");
	mBoxAni->getAnimation()->play("step3", -1, 1);
	mBoxAni->setScale(0.6);
	mBtnAward->addChild(mBoxAni, -1);
	mBoxAni->setPosition(Point(50, 5));

	mImage_timeBg = mPlayerInfoNode->getChildByName("Image_timeBg");

	return true;
}


void RankingDialog::aniCallback(cocostudio::Armature * pAr, cocostudio::MovementEventType type, const std::string& aniName)
{
	switch (type)
	{
	case cocostudio::START:
		break;
	case cocostudio::COMPLETE:
	{
		if (aniName == "born")
		{
			if (mRoot)
			{
				auto defaultBtn = dynamic_cast<ui::ImageView*>(mRoot->getChildByName("BtnGroup1"));
				if (defaultBtn)
					this->onSelectList(defaultBtn, eRankingGroupType::RANKINGGROUP_BRONZE);

				//this->updatePlayerInfo();
			}
		}
	}
	break;
	case cocostudio::LOOP_COMPLETE:
		break;
	default:
		break;
	}
}


void RankingDialog::onShow()
{
	ActionWindow::onShow();
	
	if (mPlayerInfoNode)
		mPlayerInfoNode->setVisible(false);

	if (mPortraitFrame)
		mPortraitFrame->setVisible(false);

	//请求排行奖励信息
	sgRankingControl()->requestRankWeekAwardInfo();

	mPanel_noRank->setVisible(false);
	mPanel_maxGrade->setVisible(false);
	mPanel_normal->setVisible(false);
}


void RankingDialog::onClose()
{
	Director::getInstance()->getScheduler()->unscheduleSelector(schedule_selector(RankingDialog::updateCountDown), 0);

	RankingDialog::SelectLabel(mSelectedGroupBtn, false);
	mSelectedGroupBtn = nullptr;
	mCurrentGroup = eRankingGroupType::RANKINGGROUP_DEFAULT;

	this->cleanRankingList();

	this->cleanResultList();

	ActionWindow::onClose();
}

void RankingDialog::onSelectList(cocos2d::Ref* pSender, eRankingGroupType groupID)
{
	auto newGroupBtn = dynamic_cast<ui::ImageView*>(pSender);
	if (newGroupBtn != nullptr && mSelectedGroupBtn == newGroupBtn)
		return;

	RankingDialog::SelectLabel(mSelectedGroupBtn, false);

	mSelectedGroupBtn = newGroupBtn;
	mCurrentGroup = groupID;
	
	RankingDialog::SelectLabel(mSelectedGroupBtn, true);

	this->cleanRankingList();

	sgRankingControl()->requestRankingList(groupID);
}


void RankingDialog::updateRankingList(eRankingGroupType groupID, const RankingControl::RankingList& rankingList)
{
	if (groupID != mCurrentGroup)
		return;

	//mPlayerRow = nullptr;

	if (rankingList.empty())
	{
		//GenerlDialog::show("Ranking", "Ranking is empty!");
		if (mRankListView)
			mRankListView->setBackGroundImage("ui/ranking/ranking_EmptyBG.png", cocos2d::ui::Widget::TextureResType::PLIST);
	}
	else
	{
		if (mRankListView)
			mRankListView->removeBackGroundImage();

		int rowIndex = 0;
		for (auto& e : rankingList)
		{
			auto rowNode = this->fetchRowNode();
			if (nullptr == rowNode)
				continue;

			this->initRowNode(rowNode, e.second, rowIndex);
			mRankListView->pushBackCustomItem(rowNode);
			++rowIndex;
		}

		//mRankListView->refreshView();
		//mRankListView->jumpToPercentVertical(0);
		mRankListView->forceDoLayout();
		mRankListView->jumpToTop();
	}

	this->updatePlayerInfo();
}

void RankingDialog::setRankAwardInfo(bool status)
{
	//宝箱状态
	mBoxAni->setVisible(true);

	if (true == status)
	{
		mBoxAni->getAnimation()->play("step1", -1, 1);
		mBtnAward->getChildByName("Text_watch")->setVisible(false);
		mBtnAward->getChildByName("Text_get")->setVisible(true);
	}
	else
	{
		mBoxAni->getAnimation()->play("step3", -1, 1);
 		mBtnAward->getChildByName("Text_watch")->setVisible(true);
		mBtnAward->getChildByName("Text_get")->setVisible(false);
	}

	//根据本地的积分计算段位，如果本地段位大于服务器发来的段位，就用本地段位
	if (sgUserObj()->getPersonalInfo()->pvpround() >= sgEnvSet("NEWER_PROTECT_LEVEL"))
	{
		const GradeData* gradeData = sgTableManager()->getGradeData(sgUserObj()->getScore());
		int id = sgRankingControl()->getGroupId();
		if (gradeData && gradeData->getGroup() > sgRankingControl()->getGroupId())
		{
			sgRankingControl()->setGroupId(gradeData->getGroup());
			sgRankingControl()->setRank(99999);	//设置一个非常靠后的排名
		}
	}

	//设置到下一阶段需要的积分
	while (true)
	{
		int nowGrade = sgRankingControl()->getGroupId();

		if (0 == sgRankingControl()->getRank())
		{
			mBtnAward->setVisible(false);
			mTextGetTime->setVisible(false);
			mImage_timeBg->setVisible(false);

			dynamic_cast<ui::Text*>(mPanel_noRank->getChildByName("TextBattleCount"))->setString(StringUtils::format("%d/%d", sgUserObj()->getPersonalInfo()->pvpround(), sgEnvSet("NEWER_PROTECT_LEVEL")));
			mPanel_noRank->setVisible(true);
			break;
		}

		mBtnAward->setVisible(true);
		mTextGetTime->setVisible(true);
		mImage_timeBg->setVisible(true);

		if (nowGrade >= 4)
		{
			//已达到最高排行榜奖励
			mPanel_maxGrade->setVisible(true);
			break;
		}

		string tmpText = sgTableManager()->getTextHelperByID(TEXT_RANK_NEED_SCORE)->text;
		const GradeData* gradeData = sgTableManager()->getGradeByGrade(nowGrade + 1);
		if (gradeData)
		{
			//当前段位 + 1 取积分下限 - 当前积分 等于所需积分
			int nextScore = gradeData->getMinPoint();
			int nowScore = sgUserObj()->getScore();
			int needScore = nextScore - nowScore;

			dynamic_cast<ui::Text*>(mPanel_normal->getChildByName("TextNeedtScore"))->setString(StringUtils::format("%d", needScore));
			mPanel_normal->setVisible(true);
		}

		break;
	}

	if (true == sgRankingControl()->getStatus())
	{	
		//可以领奖
		mTextGetTime->setString(StringUtils::format(sgTableManager()->getTextHelperByID(TEXT_TIME_FORMAT1)->text.c_str(), 0, 0, 0));
	}
	
	else if (0 != sgRankingControl()->getRank())
	{
		mTimeRemainingModel = E_NORMAL;

		CppCallLuaHelper::stuTimeRemaining timeRemaining = sgCppCallLuaHelper()->GetTimeRemaining(sgRankingControl()->getAwardTime(), mTargetWeek, mTargetTime, mTimeRemainingModel);

		if (true == timeRemaining.mbOverTime)
		{	
			mTimeRemainingModel = E_WITH_NEXT;
			timeRemaining = sgCppCallLuaHelper()->GetTimeRemaining(sgRankingControl()->getAwardTime(), mTargetWeek, mTargetTime, mTimeRemainingModel);
		}

		mTextGetTime->setString(timeRemaining.mFormat.c_str());
		
		//开启定时器
		Director::getInstance()->getScheduler()->schedule(schedule_selector(RankingDialog::updateCountDown), this, 1, false);
	}
}

void RankingDialog::updateCountDown(float dt)
{
	CppCallLuaHelper::stuTimeRemaining timeRemaining = sgCppCallLuaHelper()->GetTimeRemaining(sgRankingControl()->getAwardTime(), mTargetWeek, mTargetTime, mTimeRemainingModel);
	mTextGetTime->setString(timeRemaining.mFormat.c_str());

	if (timeRemaining.mFormat.c_str() == StringUtils::format(sgTableManager()->getTextHelperByID(TEXT_TIME_FORMAT1)->text.c_str(), 0, 0, 0))
	{
		sgRankingControl()->requestRankWeekAwardInfo();
		Director::getInstance()->getScheduler()->unschedule(schedule_selector(RankingDialog::updateCountDown), 0);
	}
}

ui::Widget* RankingDialog::fetchRowNode()
{
	ui::Widget* node = nullptr;
	if (mRowNodeRecycleList.empty())
	{
		auto root = CSLoader::createNode("ui/RankingRowNode.csb");

		auto frameNode = dynamic_cast<ImageView*>(root->getChildByName("Frame"));
		if (nullptr != frameNode)
			frameNode->removeFromParentAndCleanup(false);

		node = frameNode;
	}
	else
	{
		node = mRowNodeRecycleList.front();
		node->autorelease();
		mRowNodeRecycleList.pop_front();
	}

	return node;
}


void RankingDialog::recycleRowNode(cocos2d::ui::Widget* root)
{
	if (nullptr == root)
		return;

	root->retain();
	mRowNodeRecycleList.push_back(root);
}


void RankingDialog::initRowNode(cocos2d::ui::Widget* node, const RankingControl::PlayerData* info, int index)
{
	if (nullptr == info)
	{
		CCLOG("[error]RankingDialog::initRowNode : info is nullptr");
		return;
	}

	auto frameNode = dynamic_cast<ui::ImageView*>(node);
	if (nullptr == frameNode)
		return;

	frameNode->setTag((int)info->id);

	bool isTopThree = false;
	Color4B commonTextColor;
	std::string iconFilename;
	std::string frameFilename;

	switch (info->rankingId)
	{
	case 1:
		isTopThree = true;
		commonTextColor.a = 0xff;
		commonTextColor.r = 0xd3;
		commonTextColor.g = 0x1b;
		commonTextColor.b = 0x21;
		iconFilename = "ui/ranking/ranking_1.png";
		frameFilename = "ui/ranking/ranking_RowBG-1.png";
		break;
	case 2:
		isTopThree = true;
		commonTextColor.a = 0xff;
		commonTextColor.r = 0x10;
		commonTextColor.g = 0x64;
		commonTextColor.b = 0xcb;
		iconFilename = "ui/ranking/ranking_2.png";
		frameFilename = "ui/ranking/ranking_RowBG-2.png";
		break;
	case 3:
		isTopThree = true;
		commonTextColor.a = 0xff;
		commonTextColor.r = 0xcb;
		commonTextColor.g = 0x82;
		commonTextColor.b = 0x10;
		iconFilename = "ui/ranking/ranking_3.png";
		frameFilename = "ui/ranking/ranking_RowBG-3.png";
		break;
	default:
		isTopThree = false;
		commonTextColor.a = 0xff;
		commonTextColor.r = 0x8d;
		commonTextColor.g = 0x58;
		commonTextColor.b = 0x39;
		iconFilename.clear();
		frameFilename = "ui/ranking/ranking_RowBG-Normal.png";
		break;
	}

	auto pGradeData = sgTableManager()->getGradeData(info->point);

	auto groupIcon = dynamic_cast<Sprite*>(frameNode->getChildByName("GroupIcon"));
	if (groupIcon)
	{
		if (nullptr != pGradeData)
		{
			groupIcon->setSpriteFrame(StringUtils::format("ui/Icon2/ranking_Emblem-%02d.png", pGradeData->getID()));
			groupIcon->setVisible(true);
		}
		else
		{
			groupIcon->setVisible(false);
		}
	}

	if (info->id == sgUserObj()->getUin())
	{
		frameNode->loadTexture("ui/ranking/ranking_RowBG-Self.png", ui::Widget::TextureResType::PLIST);
	}
	else
	{
		frameNode->loadTexture(frameFilename, ui::Widget::TextureResType::PLIST);
	}

	auto rankingIcon = dynamic_cast<Sprite*>(frameNode->getChildByName("RankingIcon"));
	if (rankingIcon)
	{
		if (iconFilename.empty())
		{
			rankingIcon->setVisible(false);
		}
		else
		{
			rankingIcon->setSpriteFrame(iconFilename);
			rankingIcon->setVisible(true);
		}
	}

	auto textRankingId = dynamic_cast<ui::Text*>(frameNode->getChildByName("TextRankingId"));
	if (textRankingId)
	{
		if (isTopThree)
		{
			textRankingId->setTextColor(Color4B(0xff, 0xff, 0xff, 0xff));
			auto vr = dynamic_cast<Label*>(textRankingId->getVirtualRenderer());
			if (vr)
			{
				vr->enableOutline(Color4B(0xb5, 0x63, 0x1a, 0xff), 2);
			}
		}
		else
		{
			textRankingId->setTextColor(Color4B(0x8d, 0x53, 0x39, 0xff));
			auto vr = dynamic_cast<Label*>(textRankingId->getVirtualRenderer());
			if (vr)
			{
				vr->disableEffect(LabelEffect::OUTLINE);
			}

		}

		/*if (info->rankingId < 10)
			textRankingId->setFontSize(30);
			else if (info->rankingId < 100)
			textRankingId->setFontSize(20);
			else if (info->rankingId < 1000)
			textRankingId->setFontSize(14);
			else
			textRankingId->setFontSize(10);*/

		textRankingId->setString(StringUtils::format("%d", info->rankingId));
	}

	auto name = dynamic_cast<ui::Text*>(frameNode->getChildByName("TextName"));
	if (name)
	{
#ifdef MGAME_PLATFORM_MPLUS
		name->setFontName(DEFAULT_FONT);
#endif
		name->setString(info->name);
		name->setTextColor(commonTextColor);
	}

	auto winRate = dynamic_cast<ui::Text*>(frameNode->getChildByName("TextWinRate"));
	if (winRate)
	{
		winRate->setString(StringUtils::format("%.2f%%", info->winRate));
		winRate->setTextColor(commonTextColor);
	}

	auto point = dynamic_cast<ui::Text*>(frameNode->getChildByName("TextScore"));
	if (point)
	{
		point->setString(StringUtils::format("%d", info->point));
		point->setTextColor(commonTextColor);
	}

	//VIP
	auto vipNode = dynamic_cast<Node*>(frameNode->getChildByName("vipNode"));
	if (vipNode)
	{
#ifdef MGAME_PLATFORM_MPLUS
		if (info->vipLevel>0 && info->vipLevel <= 3)
		{
			vipNode->removeAllChildren();
			vipNode->setVisible(true);
			Sprite * left = Sprite::createWithSpriteFrameName(StringUtils::format("ui/general/general_vip%d.png", info->vipLevel));
			Sprite * right = Sprite::createWithSpriteFrameName(StringUtils::format("ui/general/general_vip%d.png", info->vipLevel));
			left->setPosition(0, 0);
			right->setFlippedX(true);
			right->setPosition(name->getContentSize().width + 45, 0);
			vipNode->addChild(left);
			vipNode->addChild(right);
		}
		else
			vipNode->setVisible(false);
#else
		vipNode->setVisible(false);
#endif
			
	}
}

void RankingDialog::cleanRankingList()
{
	if (nullptr == mRankListView)
		return;

	auto itemList = mRankListView->getItems();
	for (auto & item : itemList)
	{
		this->recycleRowNode(item);
		mRankListView->removeChild(item, false);
	}
}


void RankingDialog::updatePlayerInfo()
{
	int playerPoint = sgUserObj()->getPersonalInfo()->score();//sgUserObj()->getPvpPoint();
	int groupId = sgUserObj()->getRankGroup();
	int ranking = sgUserObj()->getPersonalInfo()->rank();//sgUserObj()->getRanking();

	if (mPlayerInfoNode)
	{
		mPlayerInfoNode->setVisible(true);

		auto textPlayerName = dynamic_cast<Text*>(mPlayerInfoNode->getChildByName("TextName"));
		if (textPlayerName)
		{
#ifdef MGAME_PLATFORM_MPLUS
			textPlayerName->setColor(Color3B(126, 65, 24));
			textPlayerName->setFontName(DEFAULT_FONT);
#endif
			textPlayerName->setString(sgUserObj()->getPlayerName());
		}

		auto groupIcon = dynamic_cast<Sprite*>(mPlayerInfoNode->getChildByName("GroupIcon"));
		auto emblemFile = StringUtils::format("ui/Icon2/ranking_Emblem-00.png");
		if (groupIcon)
		{		
			auto pGradeData = sgTableManager()->getGradeData(playerPoint);
			if (pGradeData)
			{
				emblemFile = StringUtils::format("ui/Icon2/ranking_Emblem-%02d.png", pGradeData->getID());			
			}
		}
		groupIcon->setSpriteFrame(emblemFile);

		auto textGroup = dynamic_cast<Text*>(mPlayerInfoNode->getChildByName("TextGroup"));
		if (textGroup)
		{
			//if (sgUserObj()->getRoundCount() < 20)
			if (sgUserObj()->getPersonalInfo()->pvpround() < sgEnvSet("NEWER_PROTECT_LEVEL"))
			{
				textGroup->setString(sgTextHelper(TEXT_PVP_NO_RANK));
			}
			else
			{
				textGroup->setString(RankingDialog::GetGroupName((eRankingGroupType)groupId));
				//textGroup->setString(pGradeData->getName());
			}
		}

		auto textRanking = dynamic_cast<Text*>(mPlayerInfoNode->getChildByName("TextRanking"));
		if (textRanking)
		{
			//textRanking->setString(StringUtils::format("%d", ranking));
			if (ranking != 0)
			{
				textRanking->setVisible(true);
				textRanking->setString(StringUtils::format("%d", ranking));
			}
			else
			{
				textRanking->setVisible(false);
			}

			auto vr = dynamic_cast<Label*>(textRanking->getVirtualRenderer());
			if (vr)
			{
				vr->setAdditionalKerning(-2.0f);
			}

		}

		auto rankingArrow = dynamic_cast<ImageView*>(mPlayerInfoNode->getChildByName("Arrow"));
		if (rankingArrow)
		{
			rankingArrow->setVisible(false);

			int prevRanking = sgRankingControl()->getPlayerPrevRanking();
			int curRanking = sgRankingControl()->getPlayerCurrentRanking();
			int rankingDiff = prevRanking - curRanking;
			if (prevRanking > 0 && curRanking > 0)
			{
				if (rankingDiff > 0)
				{
					rankingArrow->setVisible(true);
					rankingArrow->loadTexture("ui/ranking/ranking_ArrowUp.png", ui::Widget::TextureResType::PLIST);
				}
				else if (rankingDiff < 0)
				{
					rankingArrow->setVisible(true);
					rankingArrow->loadTexture("ui/ranking/ranking_ArrowDown.png", ui::Widget::TextureResType::PLIST);
				}

			}
			else if (prevRanking == 0 && curRanking > 0)
			{
				rankingArrow->setVisible(true);
				rankingArrow->loadTexture("ui/ranking/ranking_ArrowUp.png", ui::Widget::TextureResType::PLIST);
			}

		}

		auto textPoint = dynamic_cast<Text*>(mPlayerInfoNode->getChildByName("TextPoint"));
		if (textPoint)
		{
			//int roundNum = 20 - sgUserObj()->getRoundCount();
			//if (sgUserObj()->getRoundCount() <  20)
			if (sgUserObj()->getPersonalInfo()->pvpround() < sgEnvSet("NEWER_PROTECT_LEVEL"))
			{
				textPoint->setText("----");
			}
			else
			{
				textPoint->setText(StringUtils::format("%d", sgUserObj()->getPersonalInfo()->score()));// sgUserObj()->getPvpPoint()));
			}

			//textPoint->setString(StringUtils::format("%d", sgUserObj()->getPvpPoint()));
		}

		auto textWinRate = dynamic_cast<Text*>(mPlayerInfoNode->getChildByName("TextWinRate"));
		if (textWinRate)
		{
			auto winRate = sgUserObj()->getPersonalInfo()->winrate();//sgUserObj()->getWinRate();
			textWinRate->setString(StringUtils::format("%0.2f%%", winRate));
			auto vr = dynamic_cast<Label*>(textWinRate->getVirtualRenderer());
			if (vr)
			{
				if (winRate >= 50.0f)
				{
					vr->setTextColor(Color4B(0x36, 0xff, 0x00, 0xff));
					vr->enableOutline(Color4B(0x11, 0x74, 0x0a, 0xff), 2);
				}
				else
				{
					vr->setTextColor(Color4B(0xff, 0x00, 0x00, 0xff));
					vr->enableOutline(Color4B(0x74, 0x0f, 0x0a, 0xff), 2);
				}
			}
		}

		auto textRoundCount = dynamic_cast<Text*>(mPlayerInfoNode->getChildByName("TextRoundCount"));
		if (textRoundCount)
		{
			textRoundCount->setString(StringUtils::format("%d", sgUserObj()->getPersonalInfo()->pvpround()));//sgUserObj()->getRoundCount()));
		}

		auto textScore = dynamic_cast<Text*>(mPlayerInfoNode->getChildByName("TextScore"));
		if (textScore)
		{
			textScore->setString(StringUtils::format("%d", sgUserObj()->getPersonalInfo()->honorcount()));//sgUserObj()->getHonorCount()));
		}
	}

	if (mPortraitFrame)
	{
		mPortraitFrame->setVisible(true);

		auto frameFile = StringUtils::format("ui/ranking/ranking_PortraitBG-%d.png", groupId > 0 ? groupId : 1);
		mPortraitFrame->loadTexture(frameFile, ui::Widget::TextureResType::PLIST);

		auto playerProtrait = dynamic_cast<Sprite*>(mPortraitFrame->getChildByName("Portrait"));
		if (playerProtrait)
		{
#ifdef MGAME_PLATFORM_MPLUS
			//FriendSystem::setPlayerIconSpr(playerProtrait, sgUserObj()->getPlayerIconID(), sgMplusControl()->getThirdPartyIcon());
			FriendSystem::setPlayerIconSpr(playerProtrait, sgUserObj()->getPersonalInfo()->iconid(), sgMplusControl()->getThirdPartyIcon());
#else
			FriendSystem::setPlayerIconSpr(playerProtrait, sgUserObj()->getPersonalInfo()->iconid());
#endif
		}

	}
}


void RankingDialog::cleanResultList()
{
/*
	if (nullptr == mPvpResultFrame)
		return;

	auto resultListView = dynamic_cast<ui::ListView*>(mPvpResultFrame->getChildByName("ResultListView"));
	if (nullptr == resultListView)
		return;

	resultListView->removeAllItems();*/
}


void RankingDialog::updatePvpResultFrame()
{
/*
	if (nullptr == mPvpResultFrame)
		return;

	auto resultListView = dynamic_cast<ui::ListView*>(mPvpResultFrame->getChildByName("ResultListView"));
	if (nullptr == resultListView)
		return;


	int roundIndex = 0;
	auto& roundResultList = sgRankingControl()->getRoundResultList();
	for (auto listItr = roundResultList.begin(); listItr != roundResultList.end(); ++listItr)
	{
		auto& pRoundRtlData = *listItr;
		if (nullptr == pRoundRtlData)
			continue;

		uint64 userUin = sgUserObj()->getUin();

		std::string strNameSet;
		int point = 0;
		int score = 0;
		auto & playerList = pRoundRtlData->resultList;
		for (auto playerItr = playerList.begin(); playerItr != playerList.end(); ++playerItr)
		{
			auto pPlayerInfo = *playerItr;
			if (nullptr == pPlayerInfo)
				continue;

			if (pPlayerInfo->uin != userUin)
			{
				strNameSet += pPlayerInfo->name;
				strNameSet += ",";
			}
			else
			{
				point = pPlayerInfo->addPoint;
				score = pPlayerInfo->addScore;
			}
		}

		if (!strNameSet.empty())
			strNameSet[strNameSet.size() - 1] = 0;

		auto pRichText = ui::RichText::create();
		if (nullptr == pRichText)
			continue;

		std::string strFormat;
		std::string strScore;
		if (0 != score)
		{
			strFormat = sgTextHelper(TEXT_RANKING_RESULT_GAINSCORE);
			strScore = StringUtils::format(strFormat.c_str(), score);
		}

		std::string strResult;
		std::string strPoint;
		if (pRoundRtlData->winUin == userUin)
		{
			strFormat = sgTextHelper(TEXT_RANKING_RESULT_WIN);
			strResult = StringUtils::format(strFormat.c_str(), strNameSet.c_str());

			if (0 != point)
			{
				strFormat = sgTextHelper(TEXT_RANKING_RESULT_GAINPOINT);
				strPoint = StringUtils::format(strFormat.c_str(), abs(point));
			}
		}
		else
		{
			strFormat = sgTextHelper(TEXT_RANKING_RESULT_LOST);
			strResult = StringUtils::format(strFormat.c_str(), strNameSet.c_str());

			if (0 != point)
			{
				strFormat = sgTextHelper(TEXT_RANKING_RESULT_LOSTPOINT);
				strPoint = StringUtils::format(strFormat.c_str(), abs(point));
			}
		}


		std::string strText = strResult;
		if (!strScore.empty())
		{
			strText += ",";
			strText += strScore;
		}

		if (!strPoint.empty())
		{
			strText += ",";
			strText += strPoint;
		}

		auto pText = ui::RichElementText::create(roundIndex * 10 + 1
			, Color3B(0x87, 0x48, 0x33)
			, 255
			, strText
			, "fonts/font.TTF"
			, 26);

		if (pText)
			pRichText->pushBackElement(pText);



		std::string iconFilename;
		if (point > 0)
			iconFilename = "ui/ranking/ranking_ArrowUp.png";
		else if (point < 0)
			iconFilename = "ui/ranking/ranking_ArrowDown.png";

		if (!iconFilename.empty())
		{
			auto pNode = ui::RichElementCustomNode::create(roundIndex * 10 + 2
				, Color3B(0xff, 0xff, 0xff)
				, 255
				, Sprite::createWithSpriteFrameName(iconFilename));

			if (pNode)
				pRichText->pushBackElement(pNode);
		}

		auto richTextSize = resultListView->getContentSize();
		//auto lenght = getUTF8StringLength((const UTF8*)strText.c_str());
		auto lengh = StringUtils::getCharacterCountInUTF8String(strText);
		if (strText.size() >= 41)
			richTextSize.height = 64.0f;
		else
			richTextSize.height = 32.0f;
		pRichText->ignoreContentAdaptWithSize(false);
		pRichText->setContentSize(richTextSize);

		resultListView->insertCustomItem(pRichText, 0);

		++roundIndex;
	}*/
}


void RankingDialog::onListViewScrollEvent(cocos2d::Ref* sender, ui::ScrollView::EventType eventType)
{
	auto listView = dynamic_cast<ui::ListView*>(sender);
	if (nullptr == listView)
		return;

	switch (eventType)
	{
	case cocos2d::ui::ScrollView::EventType::SCROLL_TO_TOP:
		break;
	case cocos2d::ui::ScrollView::EventType::SCROLL_TO_BOTTOM:
		break;
	case cocos2d::ui::ScrollView::EventType::SCROLL_TO_LEFT:
		break;
	case cocos2d::ui::ScrollView::EventType::SCROLL_TO_RIGHT:
		break;
	case cocos2d::ui::ScrollView::EventType::SCROLLING:
		/*if (mPlayerRow)
		{
			auto rect = mPlayerRow->getBoundingBox();
			CCLOG("[evix] x=%f, y=%f", mPlayerRow->getPosition().x, mPlayerRow->getPosition().y);
			auto layout = listView->getInnerContainer();
			if (layout)
			{
				CCLOG("[evix] x=%f, y=%f", layout->getPosition().x, layout->getPosition().y);
			}
		}*/
		break;
	case cocos2d::ui::ScrollView::EventType::BOUNCE_TOP:
		break;
	case cocos2d::ui::ScrollView::EventType::BOUNCE_BOTTOM:
		break;
	case cocos2d::ui::ScrollView::EventType::BOUNCE_LEFT:
		break;
	case cocos2d::ui::ScrollView::EventType::BOUNCE_RIGHT:
		break;
	default:
		break;
	}
}


/************************************************************************/
/*                                                                      */
/************************************************************************/
RankingInfoDialog::RankingInfoDialog()
{

}


RankingInfoDialog::~RankingInfoDialog()
{

}


bool RankingInfoDialog::init()
{
	bool ret = false;
	do
	{
		mRoot = CSLoader::getInstance()->createNode("ui/RankingInfoNode.csb");
		mRoot->setAnchorPoint(Vec2(0.5, 0.5));
		CC_BREAK_IF(!mRoot);
		onLocateClickCallback(mRoot);
		this->addChild(mRoot);

		ret = true;
	} while (0);


	return ret;
}


void RankingInfoDialog::onMenuItemAction(std::string cbName, cocos2d::Ref* sender)
{
	if (cbName == "onBack")
	{
		this->setShow(false);
	}
}


void RankingInfoDialog::onShow()
{
	ActionWindow::onShow();
}
