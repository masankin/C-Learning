#include "ActInform.h"

#include "game/ui/PageFunction.h"
#include "game/ui/system/Window.h"
#include "act/ActControl.h"
#include "act/ActCrazyWeekControl.h"
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

#include "core/CVTime.h"
#include "utils/Comm.h"
#include "core/MessageManager.h"

#define TBVIEW_WIDTH 232
#define TBVIEW_HEIGH 452
#define CELL_HEIGH   115
void ActInform::onMenuItemAction(std::string cName, cocos2d::Ref* sender)
{  
	if (cName == "onClose")
	{
		this->setShow(false);
		sgMessageManager()->sendMessage(EMT_SELECTWORD_SCHEDULE, "");
		if (sgUserObj()->getIsShowDailyMissionBtn() && sgUserObj()->getIsOpenDailyMissionFirst() && !sgUserObj()->getIsOpenDailyMissionDialog())
			sgPageFunction()->gotoFunction(MSG_FUC_LUA_WIN, "MissionDialog.csb", 1);

		return;
	 }
	else if (cName == "onInform")
	{
		mActInformType = ACTINFORM_INFORM;
		updateBtnUI();
		
	}
	else if (cName == "onAct")
	{
		mActInformType = ACTINFORM_ACT;
		updateBtnUI();
		if (!mbFirstTouchAct)
		{
			updateBtnTips();
			mbFirstTouchAct = true;
		}
	}
	else if (cName == "onGoto")
	{
		auto btn = dynamic_cast<ui::Button*>(sender);
		if (btn)
		{
			ActInformData* pData = sgTableManager()->getActInformData(btn->getTag());
			if (!pData) return;
			vector<string> vPageId = splitStr(pData->getPageId(), "#");
			if (vPageId.size() <= 0) return;
			int iAUIPage = atoi(vPageId[0].c_str());
			
			bool bShowPage = false;
			if (vPageId.size() == 1)
			{
				bShowPage = checkIsActOpen(iAUIPage);
				if (!bShowPage)
				{
					GenerlDialog::show(sgTextHelper(TEXT_COMM_TIPS), sgTextHelper(TEXT_ACTTIPS_HOPE));
				}
				else
				{
					sgPageFunction()->gotoFunction(ePageType(iAUIPage));
					this->setShow(false);
				}
			}
			else if (vPageId.size() == 2)
			{
				int iBUIPage = atoi(vPageId[1].c_str());
				bShowPage = checkIsActOpen(iAUIPage, iBUIPage);
				if (!bShowPage)
				{
					GenerlDialog::show(sgTextHelper(TEXT_COMM_TIPS), sgTextHelper(TEXT_ACTTIPS_HOPE));
				}
				else
				{
					sgPageFunction()->gotoFunction(ePageType(iAUIPage));
					if (iAUIPage == MSG_FUC_WIN_SHOP)
					{
						sgMessageManager()->sendMessage(EMT_SHOP_SHOW_BUYITEM, vPageId[1].c_str());
					}
					else
					{
						sgMessageManager()->sendMessage(EMT_GOTO_ACTLAB, StringUtils::format("%d %d", iAUIPage, iBUIPage));
					}
					this->setShow(false);
				}
				
			}
		}
	}
}

bool ActInform::checkIsActOpen(int iPageA, int iPageB)
{
	bool ret = true;
	switch (iPageA)
	{
	case MSG_FUC_NEWER_GIFT:
	{
		ret = sgActControl()->getIsNewerGiftOpened();
		break;
	}
	case MSG_FUC_ACT:
	{
		auto itr = sgActControl()->getActTypeEff()->find(iPageB);
		if (itr == sgActControl()->getActTypeEff()->end())
			ret = false;
		break;
	}
	case MSG_FUC_WINNER:
	{
		if (sgWinnerControl()->getWinnerActList(iPageB) == NULL)
			ret = false;
		break;
	}
	case MSG_FUC_FIRST_CHARGE:
	{
		if (!sgActControl()->getIsShowFirstCharge())
			ret = false;
		break;
	}
	case MSG_FUC_CRAZYWEEK:
	{
		if (!sgActCrazyWeekControl()->getIsCrazyWeekOpen())
			ret = false;
		break;
	}
	default:
		break;
	}

	return ret;
}
void ActInform::onEnter()
{
	ActionWindow::onEnter();
}

void ActInform::onExit()
{
	ActionWindow::onExit();
	sgUserObj()->saveToFile();
}


void ActInform::initView()
{
	mActItemTableView = TableView::create(this, Size(TBVIEW_WIDTH, TBVIEW_HEIGH));
	mActItemTableView->setDirection(cocos2d::extension::ScrollView::Direction::VERTICAL);
	mActItemTableView->setDelegate(this);
	mActItemTableView->setVerticalFillOrder(TableView::VerticalFillOrder::TOP_DOWN);
	mPanelViewActItem->addChild(mActItemTableView);

	mNoticeItemTableView = TableView::create(this, Size(TBVIEW_WIDTH, TBVIEW_HEIGH));
	mNoticeItemTableView->setDirection(cocos2d::extension::ScrollView::Direction::VERTICAL);
	mNoticeItemTableView->setDelegate(this);
	mNoticeItemTableView->setVerticalFillOrder(TableView::VerticalFillOrder::TOP_DOWN);
	mPanelViewNoticeItem->addChild(mNoticeItemTableView);

	mEnableClickMusic = false;

	if (mActItemTableView)
	{
		mActInformType = ACTINFORM_ACT;
		mActItemTableView->reloadData();
		updateBtnTips();
		tableCellTouched(mActItemTableView, mActItemTableView->cellAtIndex(0));
	}

	if (mNoticeItemTableView)
	{
		mActInformType = ACTINFORM_INFORM;
		mNoticeItemTableView->reloadData();
		tableCellTouched(mNoticeItemTableView, mNoticeItemTableView->cellAtIndex(0));
	}

	mEnableClickMusic = true;
}


void ActInform::onShow()
{
	ActionWindow::onShow();
	mScrollText->setVisible(false);
}
void ActInform::onClose()
{
	mScrollText->setOpacity(0);
	ActionWindow::onClose();	
}

void ActInform::windowAniCallback(cocostudio::Armature * pAr, cocostudio::MovementEventType type, const std::string& aniName)
{
	ActionWindow::windowAniCallback(pAr, type, aniName);
	if (pAr == mArmatureBg)
	{
		if (mActInformType == ACTINFORM_INFORM)
		{
			if (aniName == "born")
			{
				if (type == COMPLETE)
				{
					mScrollText->setVisible(true);
					mScrollText->setOpacity(0);
					mScrollText->runAction(FadeIn::create(0.2f));
				}
			}
			//else if (aniName == "end")
			//{
				//if (type == COMPLETE)
				//{
					//mScrollText->runAction(FadeOut::create(0.2f));
				//}
			//}
		}
		
	}
}


void ActInform::onFrameEvent(cocostudio::Bone *bone, const std::string& evt, int originFrameIndex, int currentFrameIndex)
{
	if (evt == "atk") {
		if (mArnDengGuang)
			mArnDengGuang->getAnimation()->play("play");
	}
}


bool ActInform::init()
{
	bool ret = false;
	do
	{

		mRoot = CSLoader::getInstance()->createNode("ui/ActInform.csb");
		CC_BREAK_IF(!mRoot);
		this->addChild(mRoot);
		setAnimation("fla_GongGao");
		mInformList = sgTableManager()->getActInformListByType(ACTINFORM_INFORM);
		mActList = sgTableManager()->getActInformListByType(ACTINFORM_ACT);
		mNodeCom = mRoot->getChildByName("NodeCom");

		mBtnClose = dynamic_cast<ui::Button*>(mRoot->getChildByName("ButtonClose"));
		mPanelAct = dynamic_cast<ui::Layout*>(mRoot->getChildByName("PanelAct")); 
		mPanelInform = dynamic_cast<ui::Layout*>(mRoot->getChildByName("PanelInform"));
		mfPanelActY = mPanelAct->getPositionY();
		mfPanelInformY = mPanelInform->getPositionY();
		mImageAct = dynamic_cast<ImageView*>(mPanelAct->getChildByName("ImageAct"));
		mImageInform = dynamic_cast<ImageView*>(mPanelInform->getChildByName("ImageInform"));
		mBtnGoto = dynamic_cast<ui::Button*>(mRoot->getChildByName("ButtonGoto"));

		mNodeItem = mRoot->getChildByName("NodeItem");
		//auto nodeScroll = mRoot->getChildByName("NodeScroll");
		mScrollText = dynamic_cast<ui::ScrollView*>(mRoot->getChildByName("ScrollViewNotice"));
		mPanelViewActItem = dynamic_cast<ui::Layout*>(mNodeItem->getChildByName("PanelViewItem"));
		mPanelViewNoticeItem = dynamic_cast<ui::Layout*>(mNodeItem->getChildByName("PanelViewItem1"));

		mNodeAct = mRoot->getChildByName("NodeAct");
		mTextTitle = dynamic_cast<ui::Text*>(mNodeCom->getChildByName("TextTitle"));
		mSpriteGrade = dynamic_cast<Sprite*>(mNodeCom->getChildByName("SpriteGrade"));

		mTextActTime = dynamic_cast<ui::Text*>(mNodeAct->getChildByName("TextActTime"));
		mTextActDetail = dynamic_cast<ui::Text*>(mNodeAct->getChildByName("TextActDetail"));
		mStringLabel = StringLabel::create();
		mStringLabel->setContentSize(Size(500, 10));
		mStringLabel->setTextSize(24);
		mStringLabel->setAnchorPoint(Vec2(0, 1));
		mStringLabel->setFontName(mTextTitle->getFontName());
		mScrollText->addChild(mStringLabel);


		changeBone(mArmatureBg, "GongGao_q1", mBtnClose);
		changeBone(mArmatureBg, "GongGao_AnNiu", mNodeItem);
		changeBone(mArmatureBg, "GongGao_HuoDong", mPanelAct);
		changeBone(mArmatureBg, "GongGao_GongGao", mPanelInform);
		changeBone(mArmatureBg, "GongGao_NeiRong", mNodeAct);
		//changeBone(mArmatureBg, "GongGao_gonggaoNeiRong", mNodeAct);
		changeBone(mArmatureBg, "BiaoTi", mNodeCom);
		changeBone(mArmatureBg, "GongGao_q4", mBtnGoto);
		setPopType(E_POP_TYPE_NONE);
		onLocateClickCallback(mRoot);
		initView();
		mActInformType = ACTINFORM_INFORM;
		mArnDengGuang = cocostudio::Armature::create("fla_GongGao2");
		mArnDengGuang->setBlendFunc(BlendFunc::COMADDITIVE);
		this->addChild(mArnDengGuang);
		onMenuItemAction("onInform", nullptr);
		updateBtnTips();
		mbFirstTouchAct = false;
		ret = true;
	} while (0);

	sgNoticeControl()->setActInformDialog(this);
	return ret;
}


void ActInform::updateBtnUI()
{
	auto spriteActTitle = dynamic_cast<Sprite*>(mPanelAct->getChildByName("SpriteTitle"));
	auto spriteInformTitle = dynamic_cast<Sprite*>(mPanelInform->getChildByName("SpriteTitle"));
	if (spriteActTitle && spriteInformTitle)
	{
		if (mActInformType == ACTINFORM_ACT)
		{
			spriteActTitle->setSpriteFrame("ui/ActInform/ActInform_activivty1.png");
			spriteInformTitle->setSpriteFrame("ui/ActInform/ActInform_notice0.png");
			mImageAct->loadTexture("ui/ActInform/ActInform_selectbg1.png", ui::Widget::TextureResType::PLIST);
			mImageInform->loadTexture("ui/ActInform/ActInform_selectbg0.png", ui::Widget::TextureResType::PLIST);
			mNodeAct->setVisible(true);
			mScrollText->setVisible(false);
			mBtnGoto->setVisible(true);
			mPanelViewActItem->setVisible(true);
			mPanelViewNoticeItem->setVisible(false);
			if (mPrevActInformType != mActInformType && mPrevActInformType != -1)
			{
				if (mPrevActSelectedId != 0)
					switchTilte(mPrevActSelectedId);
			}
			mPanelAct->setPositionY(mfPanelActY+12);
			mPanelInform->setPositionY(mfPanelInformY);
		}
		else if (mActInformType == ACTINFORM_INFORM)
		{
			spriteInformTitle->setSpriteFrame("ui/ActInform/ActInform_notice1.png");
			spriteActTitle->setSpriteFrame("ui/ActInform/ActInform_activivty0.png");
			mImageAct->loadTexture("ui/ActInform/ActInform_selectbg0.png", ui::Widget::TextureResType::PLIST);
			mImageInform->loadTexture("ui/ActInform/ActInform_selectbg1.png", ui::Widget::TextureResType::PLIST);
			mNodeAct->setVisible(false);
			mScrollText->setVisible(true);
			mBtnGoto->setVisible(false);
			mPanelViewActItem->setVisible(false);
			mPanelViewNoticeItem->setVisible(true);
			if (mPrevActInformType != mActInformType && mPrevActInformType != -1)
			{
				if (mPrevInformSelectedId != 0)
					switchTilte(mPrevInformSelectedId);
			}
			mPanelAct->setPositionY(mfPanelActY);
			mPanelInform->setPositionY(mfPanelInformY+12);
		}
		mPrevActInformType = mActInformType;
	}
	
}

void ActInform::updateBtnTips()
{
	bool bRet = false;
	ActInformList mList;
	if (mActInformType == ACTINFORM_ACT)
	{
		mList = mActList;
	}
	else if (mActInformType == ACTINFORM_INFORM)
	{
		mList = mInformList;
	}
	ActInformList::iterator iter = mList.begin();
	for (; iter != mList.end(); iter++)
	{
		bool ret = sgUserObj()->checkIsHaveNoticeData((*iter)->mID);
		if (!ret)
		{
			bRet = true;
			break;
		}

	}
	if (mActInformType == ACTINFORM_ACT)
	{
		mPanelAct->getChildByName("SpriteRedDot")->setVisible(bRet);
	}
	else if (mActInformType == ACTINFORM_INFORM)
	{
		mPanelInform->getChildByName("SpriteRedDot")->setVisible(bRet);
	}
}

void ActInform::switchTilte(int iActId)
{
	if (iActId != 0)
	{
		ActInformData* pData = sgTableManager()->getActInformData(iActId);
		if (!pData) return;
		mTextTitle->setText(pData->getActName());
		int iShowType = pData->mShowType2;
		if (iShowType > 0)
		{
			mSpriteGrade->setSpriteFrame(StringUtils::format("ui/ActInform/ActInform_Type%d.png", iShowType));
			mSpriteGrade->setVisible(true);
		}
		else
		{
			mSpriteGrade->setVisible(false);
		}
	}
}

void ActInform::setContent(std::string stHead,std::string stContent)
{
	mTextTitle->setText(stHead);
	PowerString ps;  
	ps.init(stContent.c_str());
	mStringLabel->setPowerString(&ps,true);
	Size _size = Size(mScrollText->getInnerContainerSize().width,mStringLabel->getRealyHight() > 260 ? mStringLabel->getRealyHight() : 260);
	mStringLabel->setPosition(Vec2(20,_size.height));
	mScrollText->setInnerContainerSize(_size);

}

ActInform::ActInform() : mScrollText(0), mStringLabel(0), mShowTimes(0), mEnableClickMusic(true), mPrevActInformType(-1)
{
}

ActInform::~ActInform()
{
	sgNoticeControl()->setActInformDialog(NULL);
}


void ActInform::updateContentUI(int iSubId)
{
	ActInformData* pData = sgTableManager()->getActInformData(iSubId);
	if (!pData) return;
	if (mActInformType == ACTINFORM_ACT)
	{
		mBtnGoto->setTag(iSubId);
		mPrevActSelectedId = iSubId;
		mScrollText->setVisible(false);
		mNodeAct->setVisible(true);
		mTextTitle->setText(pData->getActName());
		if (pData->mTControlType == 0)
			mTextActTime->setText(sgTextHelper(TEXT_ACT_OPEN_FOREVER));
		else
		{
			std::string openTime = pData->getOpenTime();
			std::string closeTime = pData->getCloseTime();
			std::vector<std::string > tTempStrVec = splitStr(openTime, "#");
			std::string str[3] = { "" };
			if (tTempStrVec.size() == 2)
			{
				openTime = tTempStrVec.at(1).substr(0, tTempStrVec.at(1).length());
				str[0] = openTime.substr(0, 4) + ".";
				str[1] = openTime.substr(4, 2) + ".";
				str[2] = openTime.substr(6, 2);
				openTime = str[0] + str[1] + str[2];
			}

			tTempStrVec = splitStr(closeTime, "#");
			if (tTempStrVec.size() == 2)
			{
				closeTime = tTempStrVec.at(1).substr(0, tTempStrVec.at(1).length());
				str[0] = closeTime.substr(0, 4) + ".";
				str[1] = closeTime.substr(4, 2) + ".";
				str[2] = closeTime.substr(6, 2);
				closeTime = str[0] + str[1] + str[2];
			}

			mTextActTime->setText(__String::createWithFormat(sgTextHelper(TEXT_ACT_OPEN_TIME).c_str(), openTime.c_str(), closeTime.c_str())->getCString());
		}
		mTextActDetail->setText(pData->getContent());
	}
	else if (mActInformType == ACTINFORM_INFORM)
	{
		mPrevInformSelectedId = iSubId;
		mScrollText->setVisible(true);
		mNodeAct->setVisible(false);
		setContent(pData->getActName(), pData->getContent());
	}

	int iShowType = pData->mShowType2;
	if (iShowType > 0)
	{
		mSpriteGrade->setSpriteFrame(StringUtils::format("ui/ActInform/ActInform_Type%d.png", iShowType));
		mSpriteGrade->setVisible(true);
	}
	else
	{
		mSpriteGrade->setVisible(false);
	}

}


TableViewCell* ActInform::tableCellAtIndex(TableView *table, ssize_t idx)
{
	TableViewCell* cell = table->cellAtIndex(idx);
	if (!cell) {
		cell = ActInformCell::create();
	}
	ActInformData* pData = nullptr;

	if (mActInformType == ACTINFORM_ACT)
	{
		pData = mActList.at(idx);
	}
	else if (mActInformType == ACTINFORM_INFORM)
	{
		pData = mInformList.at(idx);
	}
	
	if (!pData) return nullptr;
	(dynamic_cast<ActInformCell*>(cell))->updateCell(pData);
	cell->setTag(pData->mID);
	return cell;
}

ssize_t ActInform::numberOfCellsInTableView(TableView *table)
{
	ActInformList pActInformList;
	if (mActInformType == ACTINFORM_ACT)
	{
		pActInformList = mActList;
	}
	else if (mActInformType == ACTINFORM_INFORM)
	{
		pActInformList = mInformList;
	}
	if (pActInformList.size() > 0)
		return pActInformList.size();
	else
	{
		return 0;
	}
}

Size ActInform::cellSizeForTable(TableView *table)
{
	return Size(TBVIEW_WIDTH, CELL_HEIGH);
}

// tableView Delegate
void ActInform::tableCellTouched(TableView* table, TableViewCell* cell)
{
	if (mEnableClickMusic)
	{
		sgSoundManager()->playMusicEffect(SOUND_CLICK_COMMON);
	}
	ActInformList pActInformList;
	if (mActInformType == ACTINFORM_ACT)
	{
		pActInformList = mActList;
	}
	else if (mActInformType == ACTINFORM_INFORM)
	{
		pActInformList = mInformList;
	}
	for (int idx = 0; idx < pActInformList.size(); idx++)
	{
		TableViewCell* cell = table->cellAtIndex(idx);
		if (cell)
		{
			ActInformData* pData = pActInformList.at(idx);
			if (!pData) return;
			(dynamic_cast<ActInformCell*>(cell))->updateCell(pData);
		}
	}
	if (!cell) return;
	sgUserObj()->addActNoticeData(cell->getTag());
	if (sgUserObj()->checkIsHaveAllNoticeData())
	{
		sgMessageManager()->sendMessage(EMT_HONGDIAN_TIP, StringUtils::format("%d 0", MSG_FUC_ACTINFORM));
	}
	if (mEnableClickMusic)
		updateBtnTips();
	(dynamic_cast<ActInformCell*>(cell))->onTouched();

	updateContentUI(cell->getTag());
	
}

ActInformCell::ActInformCell() :mRoot(nullptr)
{

}

ActInformCell::~ActInformCell()
{

}

bool ActInformCell::init()
{
	bool ret = false;
	do {
		mRoot = CSLoader::getInstance()->createNode("ui/ActInformItem.csb");
		this->addChild(mRoot);
		mRoot->setPosition(Vec2(TBVIEW_WIDTH/2, CELL_HEIGH/2));
		initUI();
		ret = true;
	} while (0);

	return ret;
}

void ActInformCell::initUI()
{
	mSpriteItem = dynamic_cast<Sprite*>(mRoot->getChildByName("SpriteItem"));

	mTextName = dynamic_cast<ui::Text*>(mRoot->getChildByName("TextName"));

	mTextName1 = dynamic_cast<ui::Text*>(mRoot->getChildByName("TextName1"));

	mSpriteRedDot = dynamic_cast<Sprite*>(mRoot->getChildByName("SpriteRedDot"));

}

void ActInformCell::updateCell(ActInformData* pData)
{
	mSpriteItem->setSpriteFrame("ui/ActInform/ActInform_item0.png");
	mTextName->setText(pData->getActName());
	mTextName1->setText(pData->getActName());
	mTextName->setVisible(true);
	mTextName1->setVisible(false);

	if (sgUserObj()->checkIsHaveNoticeData(pData->mID))
	{
		mSpriteRedDot->setVisible(false);
	}
	else
	{
		mSpriteRedDot->setVisible(true);
	}
}

void ActInformCell::onTouched()
{
	mSpriteItem->setSpriteFrame("ui/ActInform/ActInform_item1.png");
	mTextName->setVisible(false);
	mTextName1->setVisible(true);
	mSpriteRedDot->setVisible(false);
}
