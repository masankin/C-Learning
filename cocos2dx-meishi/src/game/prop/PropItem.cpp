#include "PropItem.h"

#include "data/TableManager.h"
#include "EnvSet.h"
#include "game/prop/PropsControl.h"
#include "game/ui//PageFunction.h"
#include "game/net/UserObj.h"
#include "game/data/Tables.h"
#include "game/battle/BattleControl.h"
#include "game/pvp/PvpControl.h"
#include "game/guide/GuideManager.h"
#include "SoundManager.h"
#include "game/data/TableManager.h"

#define NAME_USE_ANI "use"
#define NAME_BUY_ANI "buy"

PropItem::PropItem() :
mIcon(0),
mProgress(0),
mRoot(0),
m_Id(0),
open(false),
mPlayUseTipAni(false),
mPlayBuyTipAni(false),
mIsTipsProp(false)
{

}

PropItem::~PropItem()
{

}

bool PropItem::init()
{
	bool ret = false;
	do
	{
		mRoot = CSLoader::getInstance()->createNode("PropItem.csb");

		CC_BREAK_IF(!Node::init());
		CC_BREAK_IF(!mRoot);

		Node* mCenter = mRoot->getChildByName("mCenter");
		mProgress = ProgressTimer::create(NULL);
		mProgress->setType(kCCProgressTimerTypeRadial);
		mProgress->setReverseProgress(true);
		mCenter->addChild(mProgress);

		mIcon = dynamic_cast<Node*>(mRoot->getChildByName("mIcon"));
		mCurOwnText = dynamic_cast<ui::Text*>(mRoot->getChildByName("Text_19"));

		mTouchLayout = dynamic_cast<ui::Layout*>(mRoot->getChildByName("Panel_1"));
		mTouchLayout->addClickEventListener(CC_CALLBACK_0(PropItem::onSelectProp, this));
		
		this->addChild(mRoot);

		 

		ret = true;
	} while (0);
	return ret;
}

void PropItem::updateUI(int id)
{ 
	mIcon->removeAllChildren();
	m_Id = id;

	const PropInfo* pProp = sgTableManager()->getPropInfoById(m_Id);
	if (!pProp || pProp->isPve == 2)
		return;

	std::string iconString = pProp->icon.substr(0, pProp->icon.find_last_of("."));
	std::string stIcon = StringUtils::format("%s_small.png", iconString.c_str());

	Sprite* pIcon = Sprite::createWithSpriteFrameName(stIcon);
	mIcon->addChild(pIcon,0,"icon");

	if (mProgress)
	{
		Sprite* pIcon = Sprite::createWithSpriteFrameName("ui/general/Battle_prop_bg2.png");
		mProgress->setSprite(pIcon);
		mProgress->setPercentage(0);
	}
	std::string frameBg = "ui/general/pvpSelect1.png";
	if (pProp->isPve == 1)
		frameBg = "ui/general/pveSelect1.png"; 
	dynamic_cast<ui::ImageView* >(mRoot->getChildByName("bg"))->loadTexture(frameBg, ui::TextureResType::PLIST);
	updatePropNum();
}

void PropItem::updatePropNum()
{
	const PropInfo* pProp = sgTableManager()->getPropInfoById(m_Id);
	if (!pProp || pProp->isPve == 2) return;

	int iNum = sgUserObj()->getProp(m_Id);
	mCurOwnText->setString(StringUtils::format("%d", iNum));

	if (iNum <= 0)
	{
		ui::Button* pAdd = Button::create("ui/general/LevelDetail_prop_add.png","","",TextureResType::PLIST);
		pAdd->addTouchEventListener(CC_CALLBACK_0(PropItem::onSelectProp, this));
		Vec2 pos(30,30);
		pAdd->setPosition(pos);

		Sprite* pIcon = dynamic_cast<Sprite*>(mIcon->getChildByName("icon"));
		if (pIcon)
		{
			std::string iconString = pProp->icon.substr(0, pProp->icon.find_last_of("."));
			std::string stIcon = StringUtils::format("%s_small_2.png", iconString.c_str());

			pIcon->setSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName(stIcon));
		}

		mIcon->addChild(pAdd,0);
		sgMessageManager()->sendMessage(EMT_PROP_USE_ALL, StringUtils::format("%d", m_Id));
	}
}

void PropItem::onSelectProp()
{
	if (!sgBattleControl()->isBorning())
		return;

	if (sgBattleControl()->getIsPvp() && sgPvpControl()->getIsDead())
	{
		sgPropsControl()->showNoticeText(sgTextHelper(TEXT_COMMTIPS_PROP_DATA));
		return;
	}

	int num = sgUserObj()->getProp(m_Id);
	if (num > 0)
	{
		if (!sgPropsControl()->checkIsCdTime(m_Id))
		{
			if (sgPropsControl()->setProp(m_Id))
				sgGuideManager()->unRegistGuideMessage();
		}
	}
	else
	{
		if (m_Id != 0)
		{
			//购买商品弹框
			ShopDetailed::show(m_Id);
		}

	}
	//ProEvent(open);
}

void PropItem::setCdPercentage(float percent)
{
	if (mProgress)
	{
		mProgress->setPercentage(percent);
	}
}

void PropItem::ProEvent(bool _b)
{
	ePropType m_propType = getItemType();
	int num = sgUserObj()->getProp(m_Id);
	if (!_b && num > 0)
	{
		sgPropsControl()->setProp(m_propType);
	}
	else
	{
		if (num <= 0)
		{
			const ItemBuyList list = sgTableManager()->getItemBuyList();
			ItemBuyList::const_iterator it = list.begin();
			for (; list.end() != it; it++)
			{
				const PropInfo* pPorp = sgTableManager()->getPropInfoById(it->second->PropId);
				if (pPorp != NULL && m_propType == pPorp->type && it->second->num == 10 && m_Id == it->second->PropId)
				{
					ShopDetailed::show(it->second->PropId);
				}
			}
		}

		//else{
		//	//弹出窗口
		//	if (m_propType == 0 || m_propType == PROP_TYPE_NULL)
		//	{
		//		return;
		//	}
		//	sgPropsControl()->setProp(m_Id);
		//	PageFunction::Get()->gotoFunction(MSG_FUC_WIN_PRO);
		//}

	}
}

ePropType PropItem::getItemType()
{
	const PropInfo* pInfo = sgTableManager()->getPropInfoById(m_Id);
	if (pInfo)
	{
		return (ePropType)pInfo->type;
	}
	return ePropType::PROP_TYPE_NULL;
}

void PropItem::update(Ref*)
{
	//setCdPercentage(0);
	updateUI(m_Id);
}

void PropItem::cleanData()
{
	if (m_Id != 0 && !sgBattleControl()->getIsPvp())
	{
		sgUserObj()->cleanPropItem(m_Id);
	}

	m_Id = 0;
	mPlayUseTipAni = false;
	mPlayBuyTipAni = false;
	mIsTipsProp = false;
	mIcon->removeAllChildren();
	mCurOwnText->setText("0");
	if (mProgress)
		mProgress->setPercentage(0);

}

void PropItem::playPropBuyTips()
{
	auto pArm = Armature::create("fla_DaoJuSYGM_ADDITIVE");
	pArm->setBlendFunc(BlendFunc::COMADDITIVE);
	pArm->setTag(0);
	pArm->getAnimation()->setMovementEventCallFunc([this](cocostudio::Armature *pAr, cocostudio::MovementEventType type, const std::string& id){
		if (MovementEventType::LOOP_COMPLETE == type)
		{
			int tag = pAr->getTag();
			pAr->setTag(++tag);
			if (pAr->getTag() == 5)
			{
				pAr->removeFromParent();
				this->mPlayBuyTipAni = false;
			}
		}
	});
	pArm->setPosition(30, 30);
	mIcon->addChild(pArm, 1, NAME_BUY_ANI);
	pArm->getAnimation()->play("play");
	mPlayBuyTipAni = true;
}

void PropItem::playPropUseTips()
{
	auto pArm = Armature::create("fla_DaoJuSYTS_ADDITIVE");
	pArm->setBlendFunc(BlendFunc::COMADDITIVE);
	pArm->setTag(0);
	pArm->getAnimation()->setMovementEventCallFunc([this](cocostudio::Armature *pAr, cocostudio::MovementEventType type, const std::string& id){
		if (MovementEventType::LOOP_COMPLETE == type)
		{
			int tag = pAr->getTag();
			pAr->setTag(++tag);
			if (pAr->getTag() == 5)
			{
				pAr->removeFromParent();
				this->mPlayUseTipAni = false;
			}
		}
	});
	mIcon->addChild(pArm, -1, NAME_USE_ANI);
	pArm->getAnimation()->play("play");
	mPlayUseTipAni = true;
}

void PropItem::stopPropTipsAni()
{
	if (mPlayUseTipAni)
	{
		mIcon->removeChildByName(NAME_USE_ANI);
		mPlayUseTipAni = false;
	}

	if (mPlayBuyTipAni)
	{
		mIcon->removeChildByName(NAME_BUY_ANI);
		mPlayBuyTipAni = false;
	}
}



/************************************************************************/
/* PropViewList                                                         */
/************************************************************************/

PropViewList::PropViewList() :mDesDialog(nullptr), mDescription(nullptr), mShowArrowDesc(true), mPropBg(nullptr)
{

}

PropViewList::~PropViewList()
{

}

bool PropViewList::init()
{
	bool ret = false;
	do
	{
		mGuideNode = NULL;

		mRoot = CSLoader::getInstance()->createNode("PropViewList.csb");

		CC_BREAK_IF(!Node::init());
		CC_BREAK_IF(!mRoot);

		mListView = dynamic_cast<ui::ListView*>(mRoot->getChildByName("mListView"));
		CC_BREAK_IF(!mListView);

		mDesDialog = mRoot->getChildByName("mDesDialog");
		mDescription = mRoot->getChildByName("mDescription");
		mPropBg = dynamic_cast<ui::ImageView*>(mRoot->getChildByName("LevelDetail_prop_bg_1"));

		//Size contentSize = mPropBg->getContentSize();
		//Vec2 pos = mPropBg->getPosition();
		//pos = Vec2(pos.x - (contentSize.width / 2), pos.y - (contentSize.height / 2));
		//mPropBg->setAnchorPoint(Vec2(0, 0));
		//mPropBg->setPosition(pos);

		//mRoot->setAnchorPoint(Vec2(1, 0.5));
		this->addChild(mRoot);

		mShow = false;

		ret = true;
	} while (0);

	return ret;
}

void PropViewList::initCell(Node* pSender, const PropInfo* info)
{
	if (!info)
		return;
	int num = sgUserObj()->getProp(info->id);

	ui::Widget* mTouch = dynamic_cast<ui::Widget*>(pSender);
	mTouch->setTag(info->id);
	//mTouch->addClickEventListener(CC_CALLBACK_1(LevelDialog::onClick, dynamic_cast<LevelDialog*>(Window::Create("LevelDialog.csb"))));
	LevelDialog* pLevelDialog = dynamic_cast<LevelDialog*>(sgPageFunction()->getWindowByPageName("LevelDialog.csb"));
	if (pLevelDialog)
	{
		mTouch->addTouchEventListener(pLevelDialog, toucheventselector(LevelDialog::onClick));
	}

	ui::Button* mBg = dynamic_cast<ui::Button*>(pSender->getChildByName("mBg")); 
	std::string normalStr = "ui/general/pvpSelect1.png";
	std::string pressStr = "ui/general/pvpSelect1.png";
	std::string disableStr = "ui/general/pvpSelect2.png";
	if (info->isPve == 1)
	{
		normalStr = "ui/general/pveSelect1.png";
		pressStr = "ui/general/pveSelect1.png";
		disableStr = "ui/general/pveSelect1.png"; 
	}
	mBg->loadTextureNormal(normalStr, ui::TextureResType::PLIST);
	mBg->loadTexturePressed(pressStr, ui::TextureResType::PLIST);
	mBg->loadTextureDisabled(disableStr, ui::TextureResType::PLIST);
	mBg->setSwallowTouches(false);
	auto pItem = sgPropsControl()->getPropItem(info->id);
	mBg->setBright(pItem == nullptr);

	std::string iconString = info->icon.substr(0, info->icon.find_last_of("."));
	std::string stIcon = StringUtils::format("%s_small.png", iconString.c_str());

	ui::Button* mIcon = dynamic_cast<ui::Button*>(pSender->getChildByName("mIcon"));
	SpriteFrame* pSF = SpriteFrameCache::getInstance()->getSpriteFrameByName(stIcon);
	if (pSF){
		Size size = pSF->getOriginalSize();
		mIcon->setContentSize(size);
	}
		
	mIcon->setScale(1.0);

	mIcon->loadTextureNormal(stIcon, TextureResType::PLIST);
	mIcon->loadTextureDisabled(StringUtils::format("%s_small_2.png", iconString.c_str()), TextureResType::PLIST);


	int propNum = sgUserObj()->getProp(info->id);
	Text* mPropNum = dynamic_cast<Text*>(pSender->getChildByName("mPropNum"));
	mPropNum->setText(StringUtils::format("x%d", propNum));

	Sprite* mAdd = dynamic_cast<Sprite*>(pSender->getChildByName("mAdd"));
	if (num == 0 && mIcon)
	{
		mAdd->setVisible(true); 
		mIcon->setBright(false);
	}
	else
	{
		mAdd->setVisible(false);
		mIcon->setBright(true);
	}
	//如果是兵力包，则作为新手引导节点
	if(info->id == 30001)
	{
		if(!mGuideNode)
		{
			mGuideNode = Node::create();
			addChild(mGuideNode);
			mGuideNode->setPosition(mBg->getPosition()); 
		}
	}

}

void PropViewList::onShow() /*const std::function<void()>& callFun = nullptr*/
{
	refreshData();
	this->setVisible(true);
	if (mShowArrowDesc)
	{
		hideOrShowDesc(true);
		scheduleOnce(schedule_selector(PropViewList::updateArrowDesc), 6);
		auto roll = RotateBy::create(0.5, -9.4);
		mDescription->stopAllActions();
		mDescription->runAction(RepeatForever::create(Sequence::create(RotateTo::create(0, 0),DelayTime::create(1), roll, roll->reverse(), NULL)));
	}
	mShow = true;
	//showBornAnimation(callFun);
}

void PropViewList::onHide()
{
	mShow = false;
	hideOrShowDesc(false);
	//showBackAnimation();
	if (mDesDialog)
	{
		mDesDialog->setVisible(false);
	}
}

void PropViewList::updateItem(cocos2d::Ref* pSender, int id)
{
	updateView(id);
}

Rect PropViewList::getRect()
{
	return mRoot->boundingBox();
}

void PropViewList::showBornAnimation(const std::function<void()>& callFun /* = nullptr */)
{
	this->setPosition(Vec2::ZERO);
	this->stopAllActions();
	
	auto pRun = MoveBy::create(0.3, Vec2(mRoot->getContentSize().width, 0));
	auto pAction = EaseElasticInOut::create(pRun);

	if (callFun)
	{
		this->runAction(Sequence::createWithTwoActions(pAction, CallFunc::create(callFun)));
	}
	else
	{
		this->runAction(pAction);
	}
}

void PropViewList::showBackAnimation()
{
	this->setPosition(Vec2(mRoot->getContentSize().width,0));
	this->stopAllActions();

	auto pRun = MoveBy::create(0.3, Vec2(-1 * mRoot->getContentSize().width, 0));
	auto pAction = EaseElasticInOut::create(pRun);
	this->runAction(pRun);
}

void PropViewList::refreshData()
{

	if (mCellList.size() > 0)
	{
		std::map<int, ui::Widget*>::iterator it = mCellList.begin();
		for (it = mCellList.begin(); it != mCellList.end();)// 赛选 出数量不为0
		{
			auto pInfo = sgTableManager()->getPropInfoById(it->first);

			if (pInfo)
			{
				initCell(it->second, pInfo);
			}
			it++;
		}

	}
	else
	{
		auto propList = sgTableManager()->getPropInfoList();

		std::vector<PropInfo*> v_Prop;
		for (PropList::iterator it = propList.begin(); it != propList.end(); it++)
		{
			if (it->second != nullptr && it->second->display == 1 && it->second->isPve == 1)
			{
				v_Prop.push_back(it->second);
			}
		}

		std::sort(std::begin(v_Prop), std::end(v_Prop), [](PropInfo* pInfo1, PropInfo* pInfo2){
			return pInfo1->priority < pInfo2->priority;
		});

		for (std::vector<PropInfo*>::iterator it = v_Prop.begin(); it != v_Prop.end(); it++)// 赛选 出数量不为0
		{
			PropInfo* pInfo = *it;
			if (pInfo)
			{
				int num = sgUserObj()->getProp(pInfo->id);
				Widget* cell = dynamic_cast<Widget*>(CSLoader::createNode("PropView.csb")->getChildByName("mTouch"));
				cell->removeFromParent();
				cell->setAnchorPoint(Vec2(0.5, 0.5));
				this->initCell(cell, pInfo);
				//ui::Button* pBgBtn = dynamic_cast<Button* >(CSLoader::createNode("PropView.csb")->getChildByName("mBg"));

				mCellList.insert(std::make_pair(pInfo->id, cell));
				mListView->pushBackCustomItem(cell);
			}
		}
	}


	if (mPropBg)
	{
		int size = 1250 - 80;
		int cellSize = size / 8;
		float offSet = MIN(size, mCellList.size() * cellSize);
		float difPos = offSet + 80 - mPropBg->getContentSize().width;
		mPropBg->setSize(Size(offSet + 80, mPropBg->getSize().height));
		ui::Helper::doLayout(mPropBg);
		mListView->setBounceEnabled(mCellList.size() > 8);
		mDescription->setPositionX(mDescription->getPositionX() + difPos);
	}

}

void PropViewList::updateView(int id)
{
	auto pView = getItemView(id);
	if (pView)
	{
		auto pInfo = sgTableManager()->getPropInfoById(id);

		if (pInfo)
		{
			initCell(pView, pInfo);
		}
	}

}

Vec2 PropViewList::getItemPosition(int id)
{
	auto pView = getItemView(id);
	if (pView)
	{
		Vec2 pos = pView->convertToWorldSpace(pView->getChildByName("mBg")->getPosition());
		return pos;
	}

	return Vec2::ZERO;
}

ui::Widget* PropViewList::getItemView(int id)
{
	std::map<int, ui::Widget*>::iterator it = mCellList.find(id);
	if (it != mCellList.end())
	{
		return it->second;
	}

	return nullptr;
}

void PropViewList::updateArrowDesc(float dt)
{
	mDescription->setVisible(false);
	mShowArrowDesc = false;
}

void PropViewList::hideOrShowDesc(bool v)
{
	if (mDescription)
	{
		if (v)
		{
			if (mShowArrowDesc)
				mDescription->setVisible(v);
		}
		else
		{
			mDescription->setVisible(v);
			mShowArrowDesc = false;

		}
		
	}

	hideOrShowDescDialog(0, false);
}

void PropViewList::hideOrShowDescDialog(int id, bool v)
{
	if (mDesDialog)
	{
		if (v)
		{
			const PropInfo* pInfo = sgTableManager()->getPropInfoById(id);
			if (!pInfo)
				return;

			std::string iconString = pInfo->icon.substr(0, pInfo->icon.find_last_of("."));
			std::string stIcon = StringUtils::format("%s_small.png", iconString.c_str());

			Sprite*  mIcon =  dynamic_cast<Sprite*>(mDesDialog->getChildByName("Icon_blb_small_2"));
			mIcon->setSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName(stIcon));

			ui::Text* tName = dynamic_cast<ui::Text*>(mDesDialog->getChildByName("Text_1"));
			tName->setText(pInfo->name);

			ui::Text* tText = dynamic_cast<ui::Text*>(mDesDialog->getChildByName("Text_2"));
			tText->setText(pInfo->explain1);


		}

		mDesDialog->setVisible(v);

	}
}


PropExchangeView::PropExchangeView()
{

}

PropExchangeView::~PropExchangeView()
{

}

bool PropExchangeView::init()
{
	bool ret = false;
	do
	{
		mRoot = CSLoader::createNode("ui/PropExchangeView.csb");
		mRoot->setAnchorPoint(Vec2(0.5, 0.5));
		CC_BREAK_IF(!mRoot);
		//mRoot->setPosition(Vec2(0, -50));

		this->addChild(mRoot);
		onLocateClickCallback(mRoot);

		ret = true;
	} while (0);


	return ret;
	return true;
}

void PropExchangeView::onShow()
{
	Node* pPanel = mRoot->getChildByName("Panel_1");
	if (!pPanel)
		return;
	

	Button* pBarrackBtn = dynamic_cast<Button*>(pPanel->getChildByName("mBarrack")); 
	if (pBarrackBtn)
		changePropFrameBg(ePropType::PROP_TYPE_EXCHANGE_B, pBarrackBtn);

	Button* pTurretBtn = dynamic_cast<Button*>(pPanel->getChildByName("mTurret"));
	if (pTurretBtn)
		changePropFrameBg(ePropType::PROP_TYPE_EXCHANGE_T, pTurretBtn);

	Button* pInsistuteBtn = dynamic_cast<Button*>(pPanel->getChildByName("mInsistute"));
	if (pInsistuteBtn)
		changePropFrameBg(ePropType::PROP_TYPE_EXCHANGE_I, pInsistuteBtn);
}

void PropExchangeView::changePropFrameBg(ePropType type, Button* btn)
{
	int id = sgTableManager()->getPropIdByType(type);
	const PropInfo* pInfo = sgTableManager()->getPropInfoById(id);
	std::string normalStr = "ui/general/pvpSelect1.png";
	std::string pressStr = "ui/general/pvpSelect1.png";
	std::string disableStr = "ui/general/pvpSelect2.png";
	if (pInfo->isPve == 1)
	{
		normalStr = "ui/general/pveSelect1.png";
		pressStr = "ui/general/pveSelect1.png";
		disableStr = "ui/general/pveSelect1.png";
	}
	btn->loadTextureNormal(normalStr, ui::TextureResType::PLIST);
	btn->loadTexturePressed(pressStr, ui::TextureResType::PLIST);
	btn->loadTextureDisabled(disableStr, ui::TextureResType::PLIST);
}

void PropExchangeView::onMenuItemAction(std::string cName, cocos2d::Ref*)
{
	if (cName == "onExchangeCancel")
	{
		sgPropsControl()->setExchange(false);
		this->setShow(false);
	}
	else if (cName == "onBarrack" || cName == "onTurret" || cName == "onInsistute")
	{
		int id = 0;

		if ("onBarrack" == cName )
		{
			id = sgTableManager()->getPropIdByType(ePropType::PROP_TYPE_EXCHANGE_B);
		}
		else if ("onTurret" == cName)
		{
			id = sgTableManager()->getPropIdByType(ePropType::PROP_TYPE_EXCHANGE_T);
		}
		else if ("onInsistute" == cName)
		{
			id = sgTableManager()->getPropIdByType(ePropType::PROP_TYPE_EXCHANGE_I);
		}

		if (id != 0 && !sgPropsControl()->checkIsCdTime(sgTableManager()->getPropIdByType(ePropType::PROP_TYPE_EXCHANGE)))
		{
			sgPropsControl()->setProp(id);
			this->setShow(false);
			sgGuideManager()->unRegistGuideMessage();
		}
	}
}





////////////////////////////////////////////////
////////////////////////////////////////////////
GuidePropItem::GuidePropItem()
{
	m_IsLoad = false;
	m_IconImg = NULL;
	m_Root = NULL;
	m_PropID = 0;
	m_Type = 0;
	m_Count = 0;
	m_IsSelect = false;
}

GuidePropItem::~GuidePropItem()
{

}

bool GuidePropItem::init()
{
	Node::init();
	initUI();
	return true;
}

void GuidePropItem::initUI()
{
	m_Root = CSLoader::getInstance()->createNode("PropItem.csb");
	addChild(m_Root);

	Layout* pLayout = dynamic_cast<ui::Layout*>(m_Root->getChildByName("Panel_1"));
	if (!pLayout)
		return;
	pLayout->addClickEventListener(CC_CALLBACK_0(GuidePropItem::onSelectCallback, this));

	ui::Text* pNumTxt = dynamic_cast<ui::Text*>(m_Root->getChildByName("Text_19"));
	if (pNumTxt)
		pNumTxt->setVisible(false);

	Node* pNode = m_Root->getChildByName("mIcon");
	if (!pNode)
		return;
	m_IconImg = ui::ImageView::create();
	m_Root->addChild(m_IconImg, 100);
	m_IconImg->setPosition(pNode->getPosition());
}

void GuidePropItem::onSelectCallback()
{ 
	if (m_IsLoad && !m_IsSelect)
	{
		m_IsSelect = true;
		sgGuideManager()->unRegistGuideMessage();
	}

}

void GuidePropItem::loadPropIcon(int propID)
{
	setVisible(true);
	m_PropID = propID;
	const PropInfo* pInfo = sgTableManager()->getPropInfoById(propID);
	if (!pInfo)
		return;
	if (m_IconImg)
	{
		std::string iconString = pInfo->icon.substr(0, pInfo->icon.find_last_of("."));
		std::string stIcon = StringUtils::format("%s_small.png", iconString.c_str());
		m_IconImg->loadTexture(stIcon, ui::TextureResType::PLIST);
	}
	m_Type = pInfo->type;
	m_Count = 1;
	m_IsSelect = false;
}

void GuidePropItem::setUsedFlag()
{
	m_Count = 0;
	if (!m_IconImg)
		return;

	ui::Scale9Sprite* sp = dynamic_cast<ui::Scale9Sprite*>(m_IconImg->getVirtualRenderer());
	setGray(sp->getSprite());
}
