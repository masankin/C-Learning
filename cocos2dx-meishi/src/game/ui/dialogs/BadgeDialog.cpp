#include "BadgeDialog.h"
#include "net/UserObj.h"
#include "sync/SyncControl.h"
#include "SoundManager.h"
#include "ui/PageFunction.h"
#include "data/TableManager.h"
#include "utils/Comm.h"
#include "CVTime.h"

#include "act/ActControl.h"
#include "winner/WinnerControl.h"

USING_NS_CC;

void BadgeDialog::SelectBadgeBtn(ImageView* image, bool selected)
{
	if (nullptr == image)
		return;

	if (selected)
	{
		image->loadTexture("ui/Badge/Badge_FrameSelect.png", ui::TextureResType::PLIST);
	}
	else
	{
		image->loadTexture("ui/Badge/Badge_FrameNormal.png", ui::TextureResType::PLIST);
	}

}


BadgeDialog::BadgeDialog() : mCurrentBadge(0)
, mTotalCount(0)
, mCurrentPage(0)
, mBadgeInfoNode(nullptr)
, mPageNumText(nullptr)
, mSelectedBadgeBtn(nullptr)
{
	mBadgeBtns.fill(nullptr);

	sgMessageManager()->regisiterMessageHandler(EMT_UPDATE_PLAYER_BADGES, this);
	sgMessageManager()->regisiterMessageHandler(EMT_CHANGE_PLAYER_BADGE, this);
}


BadgeDialog::~BadgeDialog()
{
	sgMessageManager()->removeMessageHandler(this);

	mCurrentBadge = 0;
	mTotalCount = 0;
	mCurrentPage = 0;

	mBadgeInfoNode = nullptr;
	mPageNumText = nullptr;
	mSelectedBadgeBtn = nullptr;

	for (auto & pBtn : mBadgeBtns)
	{
		if (pBtn)
			pBtn->release();
	}
}


void BadgeDialog::onMenuItemAction(std::string cName, cocos2d::Ref* sender)
{
	if (cName == "onSelect")
	{
		auto imgSender = dynamic_cast<ImageView*>(sender);
		if (imgSender)
		{
			this->onSelectBadge(imgSender);
		}
	}
	else if (cName == "onClose")
	{
		this->setShow(false);
		//sgPageFunction()->enableOptionButton(true);
		return;
	}
	else if (cName == "onLeft")
	{
		this->onTurnPage(mCurrentPage - 1);
	}
	else if (cName == "onRight")
	{
		this->onTurnPage(mCurrentPage + 1);
	}
	else if (cName == "onMultiFunc")
	{
		if (mCurrentBadge > 0)
		{
			if (sgUserObj()->isHaveBadge(mCurrentBadge))
			{
				this->onDress(mCurrentBadge);
			}
			else
			{
				this->onGet(mCurrentBadge);
			}
		}
	}
}


void BadgeDialog::onReceiveMassage(GameMessage * pBasicMsg)
{
	if (pBasicMsg->getMessageType() == EMT_UPDATE_PLAYER_BADGES)
	{
		this->refresh();
	}
	else if (pBasicMsg->getMessageType() == EMT_CHANGE_PLAYER_BADGE)
	{
		MessageChangePlayerBadge* pMsg = dynamic_cast<MessageChangePlayerBadge*>(pBasicMsg);
		//updateCurBadge(pMes->getBadgeId());
		if (pMsg)
			this->dressBadge(pMsg->getBadgeId());
	}
}


bool BadgeDialog::init()
{
	if (!ActionWindow::init())
	{
		return false;
	}

	mRoot = CSLoader::createNode("ui/BadgeDialog.csb");
	if (nullptr == mRoot)
	{
		CCLOG("[error]BadgeDialog::init : Create mRoot failed.");
		return false;
	}
	
	this->addChild(mRoot);

	for (unsigned int i = 0; i < BadgeDialog::CountPerPage; ++i)
	{
		auto pNode = CSLoader::createNode("ui/BadgeFrameNode.csb");
		if (pNode)
		{
			auto pFrame = dynamic_cast<ImageView*>(pNode->getChildByName("BadgeFrame"));
			if (pFrame)
			{
				pFrame->retain();
				mBadgeBtns[i] = pFrame;

				pFrame->removeFromParentAndCleanup(false);
				mRoot->addChild(pFrame);
			}
		}
	}

	Vector<Node*> children = mRoot->getChildren();
	for (auto pChild : children)
	{
		pChild->setPosition(Point::ZERO);
	}


	onLocateClickCallback(mRoot);
	setPopType(E_POP_TYPE_NONE);
	
	// Init ui armature
	cocostudio::ArmatureDataManager::getInstance()->addArmatureFileInfo("fla_Xunzhan.xml");
	setAnimation("fla_Xunzhan");
	if (nullptr == mArmatureBg)
	{
		CCLOG("[error]BadgeDialog::init : Init mArmatureBg failed.");
		return false;
	}

	// Close Button
	ui::Button* pCloseBtn = dynamic_cast<ui::Button*>(mRoot->getChildByName("BtnClose"));
	if (nullptr != pCloseBtn)
	{
		changeBone(mArmatureBg, "Xunzhan_guanbiaa", pCloseBtn);
	}

	// Left Button
	ui::Button* pLeftBtn = dynamic_cast<ui::Button*>(mRoot->getChildByName("BtnLeft"));
	if (nullptr != pLeftBtn)
	{
		changeBone(mArmatureBg, "Xunzhan_zhixiangb", pLeftBtn);
	}

	// Right Button
	ui::Button* pRightBtn = dynamic_cast<ui::Button*>(mRoot->getChildByName("BtnRight"));
	if (nullptr != pRightBtn)
	{
		changeBone(mArmatureBg, "Xunzhan_zhixang1", pRightBtn);
	}

	// PageText
	mPageNumText = dynamic_cast<ui::Text*>(mRoot->getChildByName("TextPage"));
	if (nullptr != mPageNumText)
	{
		changeBone(mArmatureBg, "Xunzhan_yeqian", mPageNumText);
	}

	// InfoFrame
	mBadgeInfoNode = mRoot->getChildByName("InfoFrame");
	if (nullptr != mBadgeInfoNode)
	{
		changeBone(mArmatureBg, "Xunzhan_xiangqing", mBadgeInfoNode);

		auto pTextIntroduce = dynamic_cast<ui::Text*>(mBadgeInfoNode->getChildByName("TextIntroduce"));
		if (pTextIntroduce)
		{
			auto pVr = dynamic_cast<Label*>(pTextIntroduce->getVirtualRenderer());
			if (pVr)
			{
				pVr->setMaxLineWidth(298.0f);
				pVr->setLineHeight(33.0f);
				//pVr->setDimensions(298.0f, 66.0f);
			}
		}
	}

	// Group Button
	ui::ImageView* pFrame = nullptr;

	pFrame = mBadgeBtns[0];
	if (nullptr != pFrame)
	{
		changeBone(mArmatureBg, "Xunzhan_ahezi", pFrame);
	}

	pFrame = mBadgeBtns[1];
	if (nullptr != pFrame)
	{
		changeBone(mArmatureBg, "Xunzhan_bhezi", pFrame);
	}

	pFrame = mBadgeBtns[2];
	if (nullptr != pFrame)
	{
		changeBone(mArmatureBg, "Xunzhan_chezi", pFrame);
	}

	pFrame = mBadgeBtns[3];
	if (nullptr != pFrame)
	{
		changeBone(mArmatureBg, "Xunzhan_dhezi", pFrame);
	}

	pFrame = mBadgeBtns[4];
	if (nullptr != pFrame)
	{
		changeBone(mArmatureBg, "Xunzhan_ehezi", pFrame);
	}

	pFrame = mBadgeBtns[5];
	if (nullptr != pFrame)
	{
		changeBone(mArmatureBg, "Xunzhan_fhezi", pFrame);
	}


	auto pBadgeTable = sgTableManager()->getBadgeDataTable();
	if (pBadgeTable)
	{
		auto& cache = pBadgeTable->getCache();
		mTotalCount = cache.size();
	}

	return true;
}


void BadgeDialog::aniCallback(cocostudio::Armature * pAr, cocostudio::MovementEventType type, const std::string& aniName)
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


void BadgeDialog::onShow()
{
	ActionWindow::onShow();
	
	if (mBadgeInfoNode)
		mBadgeInfoNode->setVisible(false);

	mCurrentPage = 0;
	mCurrentBadge = 0;

	this->onTurnPage(mCurrentBadge, true);
	this->onSelectBadge(mBadgeBtns[0], false);
}


void BadgeDialog::onClose()
{
	SelectBadgeBtn(mSelectedBadgeBtn, false);
	mCurrentBadge = 0;
	mCurrentPage = 0;

	ActionWindow::onClose();
}


int BadgeDialog::getPageSize()
{
	int size = mTotalCount / BadgeDialog::CountPerPage;
	if (mTotalCount % BadgeDialog::CountPerPage > 0)
		size += 1;

	return size;
}


void BadgeDialog::onTurnPage(int page, bool force /* = false */)
{
	auto totalPageCount = this->getPageSize();
	if (page >= totalPageCount)
		page = totalPageCount - 1;

	if (page < 0)
		page = 0;

	if (!force && page == mCurrentPage)
		return;

	mSelectedBadgeBtn = nullptr;

	auto pBadgeTable = sgTableManager()->getBadgeDataTable();
	if (pBadgeTable)
	{
		auto& cache = pBadgeTable->getCache();

		int begIndex = page * BadgeDialog::CountPerPage;
		int endIndex = (page + 1) * BadgeDialog::CountPerPage;

		for (int index = begIndex; index < endIndex; ++index)
		{
			if (index < cache.size())
				this->updateBadgeFrame(cache[index], index % BadgeDialog::CountPerPage);
			else
				this->updateBadgeFrame(nullptr, index % BadgeDialog::CountPerPage);
		}
	}

	mCurrentPage = page;

	if (mPageNumText)
	{
		if (mTotalCount > 0)
			mPageNumText->setText(StringUtils::format("%d/%d", mCurrentPage + 1, this->getPageSize()));
		else
			mPageNumText->setText("-/-");
	}
}


void BadgeDialog::updateBadgeFrame(const BadgeData* pData, int index)
{
	if (index < 0 || index >= BadgeDialog::CountPerPage)
	{
		CCLOG("[error]BadgeDialog::updateBadgeFrame : Out of range[%d]", index);
		return;
	}

	auto pFrame = mBadgeBtns[index];
	if (pFrame)
	{
		if (nullptr != pData)
		{
			pFrame->setVisible(true);
			pFrame->setEnabled(true);
			if (pData->getId() == mCurrentBadge)
			{
				BadgeDialog::SelectBadgeBtn(pFrame, true);
				mSelectedBadgeBtn = pFrame;
			}
			else
			{
				BadgeDialog::SelectBadgeBtn(pFrame, false);
			}
			pFrame->setTag(pData->getId());

			bool bHave = sgUserObj()->isHaveBadge(pData->getId());
			bool bActive = pData->getActive() != 0;

			auto pIconDress = pFrame->getChildByName("IconDress");
			if (pIconDress)
			{
				if (bActive)
				{
					//if (sgUserObj()->getPlayerTitle() == pData->getId())
					if (sgUserObj()->getPersonalInfo()->playertitle() == pData->getId())
					{
						pIconDress->setVisible(true);
					}
					else
					{
						pIconDress->setVisible(false);
					}
				}
				else
				{
					pIconDress->setVisible(false);
				}
			}

			
			auto pIconBadge = dynamic_cast<Sprite*>(pFrame->getChildByName("IconBadge"));
			if (pIconBadge)
			{
				if (bActive)
					pIconBadge->setSpriteFrame(GetPlayerBadgeIconNameById(pData->getId(), bHave));
				else
					pIconBadge->setSpriteFrame("ui/Icon2/BadgeIcon00.png");
			}

			auto pTextName = dynamic_cast<ui::Text*>(pFrame->getChildByName("TextTitle"));
			if (pTextName)
			{
				//if (bActive)
				//	pTextName->setText(pData->getName());
				//else
				//	pTextName->setText(sgTextHelper(TEXT_BADGE_NOTACTIVE));
				pTextName->setText(pData->getName());
			}
		}
		else
		{
			pFrame->setVisible(false);
			pFrame->setEnabled(false);
		}
	}
}


void BadgeDialog::onSelectBadge(cocos2d::ui::ImageView* sender, bool showSwitchAction)
{
	if (nullptr == sender)
	{
		CCLOG("[error]BadgeDialog::onSelectBadge : sender is null");
		return;
	}

	if (nullptr != mSelectedBadgeBtn)
	{
		BadgeDialog::SelectBadgeBtn(mSelectedBadgeBtn, false);
	}

	mSelectedBadgeBtn = sender;

	BadgeDialog::SelectBadgeBtn(mSelectedBadgeBtn, true);

	if (mCurrentBadge != mSelectedBadgeBtn->getTag())
	{
		mCurrentBadge = mSelectedBadgeBtn->getTag();
		this->updateBadgeInfoFrame(mCurrentBadge);
	}

	if (showSwitchAction) {
		doInfoFrameSwitchAction();
	}
}


void BadgeDialog::updateBadgeInfoFrame(unsigned int id)
{
	if (nullptr == mBadgeInfoNode)
	{
		CCLOG("[error]BadgeDialog::updateBadgeInfoFrame : Info Frame is null");
		return;
	}

	auto pData = sgTableManager()->getBadgeData(id);
	if (nullptr == pData)
	{
		CCLOG("[error]BadgeDialog::updateBadgeInfoFrame : Data is null");
		return;
	}

	mBadgeInfoNode->setVisible(true);

	bool bActive = pData->getActive() != 0;
	if (bActive)
	{
		auto pPlayerBadge = sgUserObj()->getBadgeItem(id);

		auto pIcon = dynamic_cast<ui::ImageView*>(mBadgeInfoNode->getChildByName("IconPhoto"));
		if (pIcon)
		{
			if (nullptr != pPlayerBadge)
				pIcon->loadTexture(GetPlayerBadgeIconNameById(pData->getId(), true), ui::Widget::TextureResType::PLIST);
			else
				pIcon->loadTexture(GetPlayerBadgeIconNameById(pData->getId(), false), ui::Widget::TextureResType::PLIST);
		}

		auto pTextName = dynamic_cast<ui::Text*>(mBadgeInfoNode->getChildByName("TextName"));
		if (pTextName)
		{
			pTextName->setVisible(true);
			pTextName->setText(pData->getName());
		}

		auto pTextAlias = dynamic_cast<ui::Text*>(mBadgeInfoNode->getChildByName("TextAlias"));
		if (pTextAlias)
		{
			pTextAlias->setVisible(true);
			pTextAlias->setText(pData->getAlias());
		}

		auto pTextTime = dynamic_cast<ui::Text*>(mBadgeInfoNode->getChildByName("TextTime"));
		if (pTextTime)
		{
			pTextTime->setVisible(true);
			if (nullptr != pPlayerBadge)
			{
				CVTime cvTime(pPlayerBadge->getTime());
				pTextTime->setText(StringUtils::format(sgTextHelper(TEXT_BADGE_OBTAINTIME).c_str(), cvTime.GetYear(), cvTime.GetMonth(), cvTime.GetDay()));
			}
			else
			{
				pTextTime->setText(sgTextHelper(TEXT_BADGE_NOTOBTAIN));
			}
		}

		auto pTextIntroduce = dynamic_cast<ui::Text*>(mBadgeInfoNode->getChildByName("TextIntroduce"));
		if (pTextIntroduce)
		{
			pTextIntroduce->setVisible(true);
			pTextIntroduce->setText(pData->getIntroduce());
		}

		auto pBtnMultiFunc = dynamic_cast<ui::Button*>(mBadgeInfoNode->getChildByName("BtnMultiFunc"));
		if (pBtnMultiFunc)
		{
			//if (sgUserObj()->getPlayerTitle() == pData->getId())
			if (sgUserObj()->getPersonalInfo()->playertitle() == pData->getId() || pData->getJumpPoint().compare("0") == 0)
				pBtnMultiFunc->setVisible(false);
			else
				pBtnMultiFunc->setVisible(true);

			//pBtnMultiFunc->setVisible(true);

			auto pTitle = dynamic_cast<Sprite*>(pBtnMultiFunc->getChildByName("Title"));
			if (pTitle)
			{
				if (nullptr != pPlayerBadge)
					pTitle->setSpriteFrame("ui/Badge/Badge_TitleDress.png");
				else
					pTitle->setSpriteFrame("ui/Badge/Badge_TitleGet.png");
			}
		}

		auto pIconCompleted = dynamic_cast<Sprite*>(mBadgeInfoNode->getChildByName("IconCompleted"));
		if (pIconCompleted)
		{
			if (nullptr != pPlayerBadge)
				pIconCompleted->setVisible(true);
			else
				pIconCompleted->setVisible(false);
		}
	}
	else
	{
		auto pIcon = dynamic_cast<ui::ImageView*>(mBadgeInfoNode->getChildByName("IconPhoto"));
		if (pIcon)
		{
			pIcon->loadTexture("ui/Icon2/BadgeIcon00.png", ui::Widget::TextureResType::PLIST);
		}

		auto pTextName = dynamic_cast<ui::Text*>(mBadgeInfoNode->getChildByName("TextName"));
		if (pTextName)
		{
			pTextName->setVisible(true);
			pTextName->setText(pData->getName());
		}

		auto pTextAlias = dynamic_cast<ui::Text*>(mBadgeInfoNode->getChildByName("TextAlias"));
		if (pTextAlias)
		{
			pTextAlias->setVisible(true);
			pTextAlias->setText(pData->getAlias());
		}

		auto pTextTime = dynamic_cast<ui::Text*>(mBadgeInfoNode->getChildByName("TextTime"));
		if (pTextTime)
		{
			pTextTime->setVisible(true);
			pTextTime->setText(sgTextHelper(TEXT_BADGE_NOTACTIVE));
		}

		auto pTextIntroduce = dynamic_cast<ui::Text*>(mBadgeInfoNode->getChildByName("TextIntroduce"));
		if (pTextIntroduce)
		{
			pTextIntroduce->setVisible(false);
			pTextIntroduce->setText("");
		}

		auto pBtnMultiFunc = dynamic_cast<ui::Button*>(mBadgeInfoNode->getChildByName("BtnMultiFunc"));
		if (pBtnMultiFunc)
		{
			pBtnMultiFunc->setVisible(false);
		}

		auto pIconCompleted = dynamic_cast<Sprite*>(mBadgeInfoNode->getChildByName("IconCompleted"));
		if (pIconCompleted)
			pIconCompleted->setVisible(false);
	}
}


void BadgeDialog::onDress(int id)
{
	//if (sgUserObj()->getPlayerTitle() != id)
	if (sgUserObj()->getPersonalInfo()->playertitle() != id)
		sgSyncControl()->getSyncProxy()->sendChangePlayerTitle(id);
}


void BadgeDialog::onGet(int id)
{
	auto pData = sgTableManager()->getBadgeData(id);
	if (nullptr == pData)
	{
		CCLOG("[error]BadgeDialog::onGet : Could not found id[%d]", id);
		return;
	}

	vector<string> vParamList = std::move(splitStr(pData->getJumpPoint(), "#"));
	if (vParamList.size() <= 0)
		return;

	int pageId = atoi(vParamList[0].c_str());
	int param = -1;
	if (vParamList.size() > 1)
		param = atoi(vParamList[1].c_str());

	switch (pageId)
	{
		case MSG_FUC_NEWER_GIFT:
		{
			if (sgActControl()->getIsNewerGiftOpened())
			{
				sgPageFunction()->gotoFunction(ePageType(pageId));
				this->setShow(false);
			}
			else
			{
				GenerlDialog::show(sgTextHelper(TEXT_COMM_TIPS), sgTextHelper(TEXT_ACTTIPS_HOPE));
			}
			
		}
		break;

		case MSG_FUC_ACT:
		{
			auto pEffectList = sgActControl()->getActTypeEff();
			if (pEffectList)
			{
				if (pEffectList->find(param) != pEffectList->end())
				{
					sgPageFunction()->gotoFunction(ePageType(pageId));
					sgMessageManager()->sendMessage(EMT_GOTO_ACTLAB, StringUtils::format("%d %d", pageId, param));
					this->setShow(false);
				}
				else
				{
					GenerlDialog::show(sgTextHelper(TEXT_COMM_TIPS), sgTextHelper(TEXT_ACTTIPS_HOPE));
				}
			}
			
		}
		break;

		case MSG_FUC_WINNER:
		{
			if (sgWinnerControl()->getWinnerActList(param) != nullptr)
			{
				sgPageFunction()->gotoFunction(ePageType(pageId));
				sgMessageManager()->sendMessage(EMT_GOTO_ACTLAB, StringUtils::format("%d %d", pageId, param));
				this->setShow(false);
			}
			else
			{
				GenerlDialog::show(sgTextHelper(TEXT_COMM_TIPS), sgTextHelper(TEXT_ACTTIPS_HOPE));
			}
			
		}
		break;

		case MSG_FUC_WIN_SHOP:
		{
			sgPageFunction()->gotoFunction(ePageType(pageId));
			sgMessageManager()->sendMessage(EMT_SHOP_SHOW_BUYITEM, vParamList[1].c_str());
			this->setShow(false);

		}
		break;

		default:
		{
			sgPageFunction()->gotoFunction(ePageType(pageId));
			this->setShow(false);
		}
		break;
	}
}


void BadgeDialog::dressBadge(unsigned int id)
{
	this->showTextTip(sgTextHelper(TEXT_BADGE_DRESSSUCCESS));

	for (unsigned int index = 0; index < BadgeDialog::CountPerPage; ++index)
	{
		auto pFrame = mBadgeBtns[index];
		if (nullptr != pFrame)
		{
			auto pIconDress = pFrame->getChildByName("IconDress");
			if (nullptr == pIconDress)
				continue;
			if (pFrame->getTag() == id)
			{
				pIconDress->setVisible(true);
			}
			else
			{
				pIconDress->setVisible(false);
			}
		}
	}

	if (mCurrentBadge == id)
	{
		if (mBadgeInfoNode)
		{
			auto pBtnMultiFunc = dynamic_cast<ui::Button*>(mBadgeInfoNode->getChildByName("BtnMultiFunc"));
			if (pBtnMultiFunc)
				pBtnMultiFunc->setVisible(false);
		}
	}
}


void BadgeDialog::refresh()
{
	this->onTurnPage(mCurrentPage, true);
}

void BadgeDialog::doInfoFrameSwitchAction()
{
	if (nullptr != mBadgeInfoNode) {
		mBadgeInfoNode->setAnchorPoint(Vec2(0.5f, 2.0f));
		auto node_size = mBadgeInfoNode->getContentSize();
		mBadgeInfoNode->setPosition(Vec2(0, node_size.height/2*3));
		mBadgeInfoNode->runAction(Sequence::create(RotateTo::create(0.1f, -15), RotateTo::create(0.1f, 0), NULL));
	}
}
