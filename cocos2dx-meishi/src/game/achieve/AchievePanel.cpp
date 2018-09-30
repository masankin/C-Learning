#include "AchievePanel.h"
#include "GamePrecedure.h"
#include "AchievePointLayout.h"
#include "AchieveStarLayout.h"
#include "AchieveAddUpLayout.h"
#include "AchieveLastLayout.h"
#include "ui/dialogs/GiftAwardDialog.h"
#include "data/TableManager.h"
#include "AchieveControl.h"
#include "sync/SyncControl.h"
#include "cocos-ext.h"
#include "game/net/UserObj.h"
#include "SoundManager.h"
#include "ui/PageFunction.h"

USING_NS_CC_EXT;
#define CONTENT_LAYOUT 10
#define RM_CELL_TAG 1001
#define BUTTON_TIP_TAG 100

AchievePanel::AchievePanel(void)
	:m_achieveType(Achieve_PVE), m_contentNode(NULL), mScrolSlider(NULL), mloadTableView(NULL), mMoveY(0), pGetPointArm(NULL)
{
}


AchievePanel::~AchievePanel(void)
{
	sgAchieveControl()->setAchievePanel(NULL);
	for (auto it = mAchieveLayoutMap.begin(); it!= mAchieveLayoutMap.end(); it++)
	{
		if ((*it).second)
		{
			CC_SAFE_RELEASE_NULL((*it).second);
		}
	}
	mAchieveLayoutMap.clear();
}


bool AchievePanel::init()
{
	if (!ActionWindow::init())
	{
		return false;
	}
	cocostudio::ArmatureDataManager::getInstance()->addArmatureFileInfo("fla_chengjiu.xml");
	mRoot = CSLoader::createNode("ui/AchieveNode.csb");
	//mRoot->setAnchorPoint(Vec2(0.5, 0.5));

	setAnimation("fla_chengjiu");	
	
	mPointIcoSp = dynamic_cast<Sprite*>(mRoot->getChildByName("Achieve_djzz_2"));
	mPointIcoSp->setVisible(false);
	mAchievePointCount = dynamic_cast<Text*>(mPointIcoSp->getChildByName("Text_2_1"));
	mPointCountLoad = dynamic_cast<LoadingBar*>(mPointIcoSp->getChildByName("LoadingBar_2"));
	mPointAwardBtn = dynamic_cast<Layout*>(mRoot->getChildByName("Panel_1"));

	mAchievePointPrent = dynamic_cast<Text*>(mPointIcoSp->getChildByName("Text_2_0_0"));
	mAchievePointPrent->setString("1/1");
	//mScrolSlider = dynamic_cast<Slider*>(mRoot->getChildByName("Slider_1"));

	for (int i = 0; i<4; i++)
	{
		ImageView* pAchieveBtn = dynamic_cast<ImageView*>(mRoot->getChildByName(__String::createWithFormat("achieve_type_%d",i)->getCString()));
		m_AchieveBtnVec.push_back(pAchieveBtn);
		changeBone(mArmatureBg, StringUtils::format("guge_chengjiu_bq%d",i+1), pAchieveBtn);

	}

	m_contentNode = mRoot->getChildByName("Panel_content");
	onSelectLayout(m_achieveType);
	mPointIcoSp->getChildByName("Text_2_0")->setVisible(false);
	addChild(mRoot);

	changeBone(mArmatureBg, "guge_chengjiu_close", mRoot->getChildByName("BtnBack"));
	changeBone(mArmatureBg, "guge_chengjiu_xz", mRoot->getChildByName("lvbg"));
	changeBone(mArmatureBg, "guge_chengjiu_cj2", m_contentNode);
	m_contentNode->setScale(0.0f);
	m_contentNode->runAction(ScaleTo::create(0.3f, 1.0));

	pBtnJewel = cocostudio::Armature::create("fla_chengjiubx");
	mPointAwardBtn->addChild(pBtnJewel);
	pBtnJewel->setPosition(ccp(55, 55));
	changeBone(mArmatureBg, "guge_chengjiu_bx", mPointAwardBtn);
	pBtnJewel->getAnimation()->play("born");
	pBtnJewel->getAnimation()->setMovementEventCallFunc(this, movementEvent_selector(AchievePanel::jewelCallback));
	
	cocostudio::Armature* pArm = pBtnJewel->getBone("guge_chengjiubx11")->getChildArmature();
	pArm->setBlendFunc(cocos2d::BlendFunc::COMADDITIVE);
	pArm->getAnimation()->play("born");

	setPopType(E_POP_TYPE_NONE);
	onLocateClickCallback(mRoot);
	sgAchieveControl()->setAchievePanel(this);
	return true;
}


void AchievePanel::onMenuItemAction(std::string cName, cocos2d::Ref* sender)
{
	if (cName == "onBack")
	{
		m_contentNode->runAction(Spawn::createWithTwoActions(ScaleTo::create(0.15f, 0.0), FadeOut::create(0.2f)));
		mPointIcoSp->setVisible(false);
		this->setShow(false);
		sgPageFunction()->enableOptionButton(true);
		sgMessageManager()->sendMessage(EMT_SELECTWORD_SCHEDULE, "");
		return;
	}
	if (!sgAchieveControl()->isCanTouch())
	{
		return;
	}
	
	if (cName == "onListBtn")
	{
		std::string lSenderName = dynamic_cast<ImageView*>(sender)->getName();
		std::string lname = lSenderName.substr(13,1);
		int lnameint = std::atoi(lname.c_str());
		onSelectLayout(lnameint);
	}

	else if (cName == "onAward")
	{
		CCLOG("[trace] sgAchieveControl()->sendReceiveAchievementRewardReq %d", sgAchieveControl()->getAchieveLv());
		sgAchieveControl()->sendReceiveAchievementRewardReq(sgAchieveControl()->getAchieveLv());
		//mPointAwardBtn->setTouchEnabled(false);
	}
}

void AchievePanel::onSelectLayout(int type)
{
	SelectLabel(m_AchieveBtnVec[m_achieveType], false);
	m_achieveType = (AchieveType) type;
	SelectLabel(m_AchieveBtnVec[m_achieveType], true);

	
	AchieveLayout* pAchieve = getAchieveLayout(m_achieveType);
	if (!pAchieve)
	{
		pAchieve = createAchieveLayout(m_achieveType);

	}
	if (pAchieve)
	{
		m_contentNode->removeAllChildren();
		m_contentNode->addChild(pAchieve,1,CONTENT_LAYOUT);
		pAchieve->updateInfo();
	}
	updateRedTip();
}

void AchievePanel::SelectLabel(ImageView* label, bool selected)
{
	if (nullptr == label)
		return;

	if (selected)
	{
		label->loadTexture("ui/Achieve/Achieve_btn_1.png",cocos2d::ui::Widget::TextureResType::PLIST);
		label->setContentSize(Size(153,102));
		auto title = dynamic_cast<Sprite*>(label->getChildByName("Title"));
		if (title)
		{
			title->setSpriteFrame(StringUtils::format("ui/Achieve/Achieve_%d_1.png",(int)m_achieveType));
		}
	}
	else
	{
		label->loadTexture("ui/Achieve/Achieve_btn_0.png",cocos2d::ui::Widget::TextureResType::PLIST);
		label->setContentSize(Size(138,100));
		auto title = dynamic_cast<Sprite*>(label->getChildByName("Title"));
		if (title)
		{
			title->setSpriteFrame(StringUtils::format("ui/Achieve/Achieve_%d_0.png",(int)m_achieveType));
		}
	}
}

void AchievePanel::updateUI(const fgame::CSGetAchievementResp& resp)
{
	if (mloadTableView&&mloadTableView->getContainer()->getChildByTag(RM_CELL_TAG))
	{
		mloadTableView->getContainer()->removeChildByTag(RM_CELL_TAG);
		
	}
	fgame::CSGetAchievementResp pResp = resp;
	updateContent(pResp);

	if (m_contentNode&& m_contentNode->getChildrenCount() == 1)
	{
		dynamic_cast<AchieveLayout*>(m_contentNode->getChildByTag(CONTENT_LAYOUT))->updateInfo();
	}
	if (mloadTableView)
	{
		mloadTableView->setContentOffset(Vec2(mMoveY,0));
		mloadTableView = NULL;
	}

	sgAchieveControl()->setCanTouch(true);
	updateRedTip();
}

void AchievePanel::updateContent(fgame::CSGetAchievementResp& resp)
{
	int pointCount = resp.achievementpointcount();
	mAchievePointCount->setString(__String::createWithFormat("%d",pointCount)->getCString());

	int level = resp.achievementlevel();
	const AchievePointAward* pAwardPoint = sgTableManager()->getAchievePointAwardById(level);
	if (!pAwardPoint)
	{
		level = level -1;
		pAwardPoint = sgTableManager()->getAchievePointAwardById(level);
	}
	if (pAwardPoint)
	{
		if (pAwardPoint->achievePoint <= pointCount)
		{
			//옵鹿쥴혤
			sgMessageManager()->sendMessage(EMT_HONGDIAN_TIP,StringUtils::format("%d 1",MSG_FUC_ACHIEVE));
			mPointCountLoad->setPercent(100);
			mPointAwardBtn->setTouchEnabled(true);
			pBtnJewel->getAnimation()->play("end",-1,1);
			pBtnJewel->getBone("guge_chengjiubx11")->getChildArmature()->getAnimation()->play("end");

		}else
		{
			mPointAwardBtn->setTouchEnabled(false);
			float pPoint1 =  pointCount *1.0/ (pAwardPoint->achievePoint *1.0);
			mPointCountLoad->setPercent(pPoint1*100);
			//if (pBtnJewel->getAnimation()->getCurrentMovementID().compare("stand"))
			{
				pBtnJewel->getAnimation()->play("born");
				pBtnJewel->getBone("guge_chengjiubx11")->getChildArmature()->getAnimation()->play("born");
			}		
		}
		int pTablePoint = pAwardPoint->achievePoint;
		ImageView* lvbg = (dynamic_cast<ImageView*>(mRoot->getChildByName("lvbg")));
		dynamic_cast<ImageView*>(lvbg->getChildByName("Image_5"))->loadTexture(StringUtils::format("ui/general/Achieve_dj_%d.png", level), cocos2d::ui::Widget::TextureResType::PLIST);
		lvbg->loadTexture(pAwardPoint->icon, cocos2d::ui::Widget::TextureResType::PLIST);
		int pGetAchievementPoint = pointCount > pTablePoint? pTablePoint: pointCount;
		mAchievePointPrent->setString(__String::createWithFormat("%d/%d",pGetAchievementPoint,pTablePoint)->getCString());
	}
}

AchieveLayout* AchievePanel::createAchieveLayout(AchieveType pType)
{
	AchieveLayout* pRet = NULL;

	switch (pType)
	{
	case Achieve_PVE:
		pRet = AchievePointLayout::create();
		break;
	case Achieve_OTHER:
		pRet = AchieveStarLayout::create();
		break;
	case Achieve_PVP:
		pRet = AchieveAddUpLayout::create();
		break;
	case  Achieve_Complete:
		pRet = AchieveLastLayout::create();
		break;
	default:
		break;
	}
	mAchieveLayoutMap.insert(std::make_pair(pType,pRet));
	CC_SAFE_RETAIN(pRet);
	return pRet;
}
AchieveLayout* AchievePanel::getAchieveLayout(AchieveType type)
{
	auto pRet = mAchieveLayoutMap.find(type);
	if (pRet != mAchieveLayoutMap.end())
	{
		return pRet->second;
	}
	return NULL;
}

void AchievePanel::setShow(bool b)
{
	ActionWindow::setShow(b);

	if (b == true)
	{
		if (sgTableManager()->isAchieveMentAwardComplent(A_M_PVP,true))
		{
			onSelectLayout(Achieve_Complete);
		}else
		{
			onSelectLayout(Achieve_PVE);
		}
		if (sgAchieveControl()->isSendQueryAchievementStatus())
		{
			sgAchieveControl()->sendQueryAchievementStatus();
		}
		else
		{
			updateUI(sgAchieveControl()->getAchievementData());
		}
		
	}
}

void AchievePanel::showAward(int achieveId, bool pgeted)
{
	int pTextId;
	if (pgeted)
	{
		pTextId = TEXT_ACHIEVE_GETPOINT;
		if (pGetPointArm == NULL)
		{
			pGetPointArm = cocostudio::Armature::create("fla_chengjiuhd");
			this->addChild(pGetPointArm,1000);
		}
		
		pGetPointArm->getAnimation()->play("play");
		cocostudio::Armature* pBoneArm = pGetPointArm->getBone("guge_chengjiu_a1")->getChildArmature();
		pBoneArm->setBlendFunc(cocos2d::BlendFunc::COMADDITIVE);
		pBoneArm->getAnimation()->play("play");
	}else
	{
		pTextId = achieveId;
	}

	const TextHelper* pTextHelper = sgTableManager()->getTextHelperByID(pTextId);
	std::string pString;
	if (pgeted)
	{
		const AchievementAward* pAward = sgTableManager()->getAchievementById(achieveId);
		pString = __String::createWithFormat(pTextHelper->text.c_str(),pAward->Award)->getCString();
	}else
		pString = pTextHelper->text;

	showTextTip(pString);
}

void AchievePanel::scrolViewSlider(int percent)
{
	//if (mScrolSlider)
	//{
		//this->mScrolSlider->setPercent(percent);
	//}
}

void AchievePanel::showAwardByLv(int achieveLv)
{
	const AchievePointAward* pPointAward = sgTableManager()->getAchievePointAwardById(achieveLv);
	if (pPointAward)
	{
		GiftAwardDialog::showDialogByGiftId(pPointAward->giftId);
		pBtnJewel->getAnimation()->play("stand");
		pBtnJewel->getBone("guge_chengjiubx11")->getChildArmature()->getAnimation()->play("stand");
	}
}

void AchievePanel::removeTableViewCell(TableViewCell* pCell)
{
	AchieveLayout* pLayer = dynamic_cast<AchieveLayout*>(m_contentNode->getChildByTag(CONTENT_LAYOUT));
	int pidx = pCell->getIdx();
	TableView* pTableView = dynamic_cast<TableView*>( pCell->getParent()->getParent());
	mloadTableView = pTableView;
	mMoveY = mloadTableView->getContentOffset().x;
	//if (m_achieveType == Achieve_Complete)
	//{
	//	mMoveY = mloadTableView->getContentOffset().y+130;
	//}
	
	float px =pCell->getPositionX();
	pCell->removeFromParent();
	mRmCellIdx = pidx;

	if (pLayer->numberOfCellsInTableView(pTableView) != pidx+1)
	{
		for (int i = pidx+1; i<pLayer->numberOfCellsInTableView(pTableView);  i++ )
		{
			TableViewCell* pcell2 = pLayer->tableCellAtIndex2(pTableView,i);
			if (pcell2->getParent() == NULL)
			{
				pTableView->getContainer()->addChild(pcell2,1,RM_CELL_TAG);
				pcell2->setPosition(Vec2(px + TABLE_WIDTH/3,0));
				pcell2->runAction(Sequence::create(MoveBy::create(0.5,Vec2(-TABLE_WIDTH/3,0)),NULL));
				break;
			}else
			{
				px = pcell2->getPositionX();
				pcell2->runAction(Sequence::create(MoveBy::create(0.5,Vec2(-TABLE_WIDTH/3,0)),NULL));
			}
			
		}
		
		scheduleOnce(schedule_selector(AchievePanel::scheduleLoad),0.6);
	}else
	{
		sgAchieveControl()->sendQueryAchievementStatus();
	}
	
}

void AchievePanel::scheduleLoad(float dt)
{

	mloadTableView->removeCellAtIndex(mRmCellIdx);
	sgAchieveControl()->setAchieveGeted();
}


void AchievePanel::showTextTip(const std::string pStr)
{
	ActionWindow::showTextTip(pStr);
}

void AchievePanel::updateRedTip()
{
	for (int i = 0; i<m_AchieveBtnVec.size(); i++)
	{
		AchieveModule pModule = A_M_PVE;
		bool pIsLast = false;
		switch (i)
		{
		case Achieve_Complete:
			pIsLast = true;
			break;
		case Achieve_PVE:
			pModule = A_M_PVE;
			break;
		case Achieve_OTHER:
			pModule = A_M_OTHER;
			break;
		case Achieve_PVP:
			pModule = A_M_PVP;
			break;
		default:
			break;
		}
		if (sgTableManager()->isAchieveMentAwardComplent(pModule, pIsLast))
		{
			if (m_AchieveBtnVec[i]->getChildByTag(BUTTON_TIP_TAG) == nullptr)
			{

				cocostudio::Armature* pTipArm = cocostudio::Armature::create("fla_HongDian");
				m_AchieveBtnVec[i]->addChild(pTipArm, 1, BUTTON_TIP_TAG);
				pTipArm->setPosition(m_AchieveBtnVec[i]->getContentSize().width*0.25, m_AchieveBtnVec[i]->getContentSize().height - 20);
				pTipArm->getAnimation()->play("play");
			}
		}else
		{
			if (m_AchieveBtnVec[i]->getChildByTag(BUTTON_TIP_TAG) )
			{
				m_AchieveBtnVec[i]->removeChildByTag(BUTTON_TIP_TAG);
			}
		}
	}
}

void AchievePanel::aniCallback(cocostudio::Armature * pAr, cocostudio::MovementEventType type, const std::string& aniName)
{
	if (type == cocostudio::COMPLETE && aniName == "born")
	{
		mPointIcoSp->setVisible(true);
	}
}

void AchievePanel::jewelCallback(cocostudio::Armature * pAr, cocostudio::MovementEventType type, const std::string& aniName)
{
	if (type == cocostudio::COMPLETE && aniName == "stand")
	{
		pAr->getAnimation()->play("end",-1,-1);
		pAr->getBone("guge_chengjiubx11")->getChildArmature()->getAnimation()->play("end");
	}
	else if (type == cocostudio::COMPLETE && aniName == "end")
	{
		pAr->getAnimation()->play("born");
		pAr->getBone("guge_chengjiubx11")->getChildArmature()->getAnimation()->play("born");
	}
}
