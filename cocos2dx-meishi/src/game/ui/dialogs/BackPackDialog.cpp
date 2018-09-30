#include "BackPackDialog.h"
#include "MessageManager.h"
#include "net/UserObj.h"
#include "ui/PageFunction.h"
#include "data/TableManager.h"
#include "utils/Comm.h"
#include "sync/SyncControl.h"
#include "prop/PropsControl.h"
#include "shop/ShopPanel.h"
#include "CCLuaEngine.h"
#define MAX_CLCUMN  4
#define MAX_ROW  2
#define MAX_CELLNUM 8

#define SMALL_MILK_BOTTLE 100
#define BIG_MILK_BOTTLE   101
#define SMALL_HAM_BOX     102
#define BIG_HAM_BOX       103

#define OPEN_MAX          10

BackPropInfoNode* BackPropInfoNode::create(int type, int propId, int count)
{
	BackPropInfoNode* pNode = new BackPropInfoNode();
	if (NULL != pNode && pNode->init(type, propId, count))
	{
		pNode->autorelease();
	}
	else
	{
		delete pNode;
		pNode = NULL;
	}
	return pNode;
}

BackPropInfoNode::BackPropInfoNode()
{
	mFun = NULL;
	mSender = NULL;
	mTarget = NULL;
}

BackPropInfoNode::~BackPropInfoNode()
{ 
}

bool BackPropInfoNode::init(int type, int propId, int count)
{
	bool isRet = false;
	do 
	{
		CC_BREAK_IF(!Layer::init());
		mPropID = propId;
		mCount = count;
		mType = type;
		initUI();
		isRet = true;
	} while (0);
	
	
	return isRet;
}

void BackPropInfoNode::initUI()
{
	mRoot = CSLoader::getInstance()->createNode("ui/backPackItem.csb");
	if (!mRoot)
		return;
	mRoot->setScale(0.8f);
	
	ui::Layout* pTouchPanel = dynamic_cast<ui::Layout*>(mRoot->getChildByName("Panel_1"));
	if (NULL != pTouchPanel)
	{
		pTouchPanel->addClickEventListener(CC_CALLBACK_0(BackPropInfoNode::onSelectCallback, this));
		pTouchPanel->setSwallowTouches(false);
	}

	ui::ImageView* pImg = dynamic_cast<ui::ImageView*>(mRoot->getChildByName("ItemBg"));
	if (!pImg)
		return;
	pImg->ignoreContentAdaptWithSize(false);
	std::string iconFrameStr = "ui/general/general_kuang.png";
	if (mType != E_BackPack_Stuff)
	{
		const PropInfo* pInfo = sgTableManager()->getPropInfoById(mPropID); 
		if (!pInfo)
			return;
		std::string sFile = pInfo->icon;
		pImg->loadTexture(sFile, ui::Widget::TextureResType::PLIST);

		if (pInfo->isPve == 1)
			iconFrameStr = "ui/general/general_kuang_4.png";
		else if (pInfo->isPve == 0)
			iconFrameStr = "ui/general/general_kuang_3.png";
	} 
	else
	{
		const GlyphStuffData* pData = sgTableManager()->getGlyphStuffData(mPropID);
		if (!pData)
			return;
		std::string sFile = pData->mStuffIcon;
		pImg->loadTexture(sFile, ui::Widget::TextureResType::PLIST);
	}
	if (mCount == 0)
	{
		ui::Scale9Sprite* sp = dynamic_cast<ui::Scale9Sprite*>(pImg->getVirtualRenderer());
		setGray(sp->getSprite());
	}
	pTouchPanel->setBackGroundImage(iconFrameStr, ui::Widget::TextureResType::PLIST);

	ui::Text* pCount = dynamic_cast<ui::Text*>(mRoot->getChildByName("ItemCount"));
	if (pCount)
		pCount->setText(CCString::createWithFormat("%d", mCount)->getCString());

	ImageView* pFrame = dynamic_cast<ui::ImageView*>(mRoot->getChildByName("ImageSelect"));
	if (pFrame)
		pFrame->setVisible(false);

	addChild(mRoot);


}

void BackPropInfoNode::setPropTouchCallback(Node* sender, SEL_CallFuncPage fun, Node* target)
{
	mSender = sender;
	mFun = fun;
	mTarget = target;
}

Size BackPropInfoNode::getNodeSize()
{ 
	ui::Button* pImg = dynamic_cast<ui::Button* >(mRoot->getChildByName("ItemButton"));
	if(pImg)
		return pImg->getContentSize();
	else
		return CCSizeMake(125, 125);;
}
 
void BackPropInfoNode::setItemSelect(bool select)
{
	ImageView* pFrame = dynamic_cast<ui::ImageView*>(mRoot->getChildByName("ImageSelect"));
	if (NULL != pFrame)
		pFrame->setVisible(select);
}

void BackPropInfoNode::onReceiveMassage(GameMessage * message)
{
	if(message->getMessageType() == EMT_SHOP_AFTER_BUYDIAMOND)
	{
		sgMessageManager()->sendMessage(EMT_CLOSESHOP, "");

		const PropInfo* pPorp = sgTableManager()->getPropInfoById(mPropID);
		int coin = sgUserObj()->getPersonalInfo()->diamond();// sgUserObj()->getGoldCoin();

		int pItemIdx = 0;
		ItemBuyList pItemList = sgTableManager()->getItemBuyList();
		for (auto it = pItemList.begin(); it!=pItemList.end(); it++)
		{
			if ((*it).second->PropId == mPropID)
			{
				pItemIdx = (*it).first;
				break;
			}
		}

		const ItemBuyInfo* pInfo = sgTableManager()->getItemBuyById(pItemIdx);
		if (pPorp && pInfo != NULL && coin >= pInfo->moneynum)
		{
			if(sgChannelApi()->isOnlineSdk() == true)
			{
				sgPropsControl()->sendBuyPropRqst(pInfo->PropId, pInfo->moneynum, pInfo->num);
				return;
			}
			std::string param = StringUtils::format("%d %d %d %d %d %s"
				, 1
				, 1
				, eVituralCurrencyType::Diamond
				, -pInfo->moneynum
				, sgUserObj()->getPersonalInfo()->diamond()//sgUserObj()->getGoldCoin()
				, "BackPropInfoNode::onReceiveMassage");
			sgMessageManager()->sendMessage(EMT_MONEY_CHANGE, param);

			sgMessageManager()->sendMessage(EMT_PROP_USER,__String::createWithFormat("%d",pInfo->PropId)->getCString()); 

			BackPackDialog* pDialog = dynamic_cast<BackPackDialog*>(sgPageFunction()->getWindowByPageName("backpackDialog.csb"));
			if(pDialog)
			{
				Size pWinSize = Director::getInstance()->getWinSize();
				pDialog->showTextTip(sgTextHelper(TEXT_BUY_ITEM_SUCCESS),Point(pWinSize.width/2,pWinSize.height/2));
			}
			
		}
	}
}

void BackPropInfoNode::onSelectCallback()
{
	if (NULL != mSender && NULL != mFun && NULL != mTarget)
	{
		if(mCount <= 0)
		{
			sgMessageManager()->removeMessageHandler(EMT_SHOP_AFTER_BUYDIAMOND);
			sgMessageManager()->regisiterMessageHandler(EMT_SHOP_AFTER_BUYDIAMOND,this);

			ShopDetailed::show(mPropID);
		} 
		(mSender->*mFun)(mPageIndex, mPropID, mTarget->getTag());
		sgBackPackControl()->setSelectStuffID(mPropID); 
		if (mType == E_BackPack_Stuff)
		{
			std:string pStr = StringUtils::format("sgGlyphControl():openStuffDetailUI(%d)", mPropID);
			LuaEngine::getInstance()->executeString(pStr.c_str());
		}
		ImageView* pFrame = dynamic_cast<ui::ImageView* >(mRoot->getChildByName("ImageSelect"));
		if(NULL != pFrame)
		{
			pFrame->setVisible(true);
		}
	}
}



void BackPropInfoNode::updateItem(cocos2d::Ref* pSender, int id)
{
	sgMessageManager()->sendMessage(EMT_MONEY_CHANGE,"");
}
//////////////////////////////////////////////

BackPackDialog::BackPackDialog()
	:mOpenBoxCount(0),
	mToolsId(0),
	mGiftBtn(nullptr),
	mSelectIndex(0),
	mPreBtn(nullptr),
	mNextBtn(nullptr),
	mMarkOpenCount(0)
{
	sgMessageManager()->regisiterMessageHandler(EMT_MONEY_CHANGE, this);
	sgMessageManager()->regisiterMessageHandler(EMT_PROP_USER, this);

	sgMessageManager()->regisiterMessageHandler(EMP_POWER_CHANGE, this);
	sgMessageManager()->regisiterMessageHandler(EMT_ENERGY_CHANGE, this);
	mShowPve = E_BackPack_Pve;
}

BackPackDialog::~BackPackDialog()
{
	 sgMessageManager()->removeMessageHandler(this);
}

bool BackPackDialog::init()
{
	ActionWindow::init();
	mRoot = CSLoader::createNode("ui/backpackDialog.csb");   
	this->addChild(mRoot, 300);
	setAnimation("fla_beibao");
	initUI();
	initData();
	return true;
} 

void BackPackDialog::initData()
{
	const PropList pInfoList = sgTableManager()->getPropInfoList(true);
	PropList::const_iterator tableIter = pInfoList.begin();
	for (; tableIter != pInfoList.end(); tableIter++)
	{
		PropInfo* pInto = tableIter->second;
		if (pInto->isPve == E_BackPack_Pve)
			mPvePropList.insert(std::make_pair(pInto->id, pInto));
		else if (pInto->isPve == E_BackPack_Pvp) {
			mPvpPropList.insert(std::make_pair(pInto->id, pInto));
		}
		else if (pInto->isPve == E_BackPack_Gift) {
			mGiftPropList.insert(std::make_pair(pInto->id, pInto));
		}
	}
	const GlyphStuffList pStuffList = sgTableManager()->getGlyphStuffMap();
	GlyphStuffList::const_iterator tableIter2 = pStuffList.begin();
	for (; tableIter2 != pStuffList.end(); tableIter2++)
	{
		GlyphStuffData* pData = tableIter2->second;
		if (pData->mStuffID != 100001){
			mStuffList.insert(std::make_pair(pData->mStuffID, pData));
		}
	}
}

void BackPackDialog::onEnter()
{
	ActionWindow::onEnter();
}

void BackPackDialog::onExit()
{
	mSelectIndex = 0;
	mShowPve = E_BackPack_Pve;
	ActionWindow::onExit();
}
  

void BackPackDialog::onReceiveMassage(GameMessage * message)
{
	if(message->getMessageType() == EMT_PROP_CHANGE || message->getMessageType() == EMT_PROP_USER)
	{
		refreshUI();
	}
	
	if (message->getMessageType() == EMT_ENERGY_CHANGE || message->getMessageType() == EMP_POWER_CHANGE){
		// 打开成功提示
		showOpenTips(mToolsId, mMarkOpenCount, 1);
	}
}

void BackPackDialog::onMenuItemAction(std::string cbName, cocos2d::Ref* sender)
{
	if (cbName == "onBack")
	{
		this->setShow(false);

		sgMessageManager()->removeMessageHandler(EMT_SHOP_AFTER_BUYDIAMOND);

		PageFunction::Get()->gotoFunction(MSG_FUC_MAIN_LEVEL); 
		sgMessageManager()->sendMessage(EMT_SELECTWORD_SCHEDULE, "");
	} 
	else if(cbName == "onShowPve")
	{
		shopPveProp();
	}
	else if(cbName == "onShowPvp")
	{
		showPvpProp();
	}
	else if (cbName == "onShowGift")
	{
		showGift();
	}
	else if (cbName == "onShowStuff")
	{
		showStuff();
	}
	else if (cbName == "onOpenBox")
	{
		// send open box request
		if (mToolsId != 0 && mOpenBoxCount > 0) {
			mMarkOpenCount = calculateOpenCount(mToolsId, mOpenBoxCount);
			sgPropsControl()->sendOpenChestRqst(mToolsId, mMarkOpenCount);
		}
	}
	else if (cbName == "onLeftBtn")
	{
		goPrePage();
	}
	else if (cbName == "onRightBtn")
	{
		goNextPage();
	}
}

int BackPackDialog::calculateOpenCount(int item_id, int item_count)
{
	int open_count = item_count;

	if (item_count >= OPEN_MAX) {
		open_count = OPEN_MAX;
	}

	/////////
	int needCount = fullPowerNeed(item_id);
	if (needCount != -POWER_BUY_MAX)
	{
		// 补充体力，精力判断
		if (needCount > 0) {
			if (needCount < OPEN_MAX && needCount < open_count) {
				open_count = needCount;
			}
		}
		else {
			//showTextTip("no need to charge ....");
			open_count = needCount;
			showOpenTips(item_id, open_count);
		}
	}
	/////////

	return open_count;
}

void BackPackDialog::showOpenTips(int open_id, int open_count, int show_type)
{
	const PropInfo* propInfo = sgTableManager()->getPropInfoById(open_id);
	if (!propInfo) {
		return;
	}

	std::string pveStr = sgTextHelper(TEXT_ACT_TILI);
	std::string pvpStr = sgTextHelper(TEXT_ACT_JINGLI);
	std::string txt1 = sgTextHelper(TEXT_INSTITUTE_1);
	std::string txt2 = sgTextHelper(TEXT_INSTITUTE_2);

	if(pveStr.empty() || pvpStr.empty() || txt1.empty() || txt2.empty())
	{
		return;
	}

	switch (propInfo->type)
	{
	case SMALL_MILK_BOTTLE:
	case BIG_MILK_BOTTLE:
	{
		if (show_type == 0) {
			showTextTip(StringUtils::format(txt2.c_str(), pveStr.c_str()));
		}
		else if (show_type == 1){		
			showTextTip(StringUtils::format(txt1.c_str(), open_count, propInfo->headText.c_str(), open_count * (int)propInfo->duration, pveStr.c_str()));
		}
		break;
	}
	case SMALL_HAM_BOX:
	case BIG_HAM_BOX:
	{

		if (show_type == 0) {
			showTextTip(StringUtils::format(txt2.c_str(), pvpStr.c_str()));
		}
		else if (show_type == 1) {
			showTextTip(StringUtils::format(txt1.c_str(), open_count, propInfo->headText.c_str(), open_count * (int)propInfo->duration, pvpStr.c_str()));
		}
		break;
	}
	default:
		break;
	}
}

int BackPackDialog::fullPowerNeed(int bottle_id)
{
	int needCount = -POWER_BUY_MAX;
	const PropInfo* propInfo = sgTableManager()->getPropInfoById(bottle_id);
	if (!propInfo) {
		return needCount;
	}

	int pveNum = (int)sgUserObj()->getPersonalInfo()->power();
	int pvpNum = (int)sgUserObj()->getPersonalInfo()->energy();

	int num = (int)propInfo->duration;
	if (num <= 0){
		num = 1;
	}

	switch (propInfo->type)
	{
	case SMALL_MILK_BOTTLE:
	case BIG_MILK_BOTTLE:
	{	
		needCount = (POWER_BUY_MAX + PVEPOWER_INIT_VALUE - pveNum) / num;
		break;
	}
	case SMALL_HAM_BOX:
	case BIG_HAM_BOX:
	{
		needCount = (POWER_BUY_MAX + PVPPOWER_INIT_VALUE - pvpNum) / num;
		break;
	}
	default:
		;
	}

	return needCount;
}

void BackPackDialog::onShow()
{
	Window::onShow(); 
	mSelectIndex = 0;
	if (sgBackPackControl()->getShowType() == E_BackPack_Stuff)
		showStuff();
	else
		shopPveProp();
	
}

void BackPackDialog::initUI()
{     
	mBgNode = dynamic_cast<Node* >(mRoot->getChildByName("BoxTopBg"));   
	mBgNode->setPosition(ccp(0, 20));

	mViewNode = dynamic_cast<ui::PageView* >(mBgNode->getChildByName("PropPageView"));  
	mViewNode->addEventListenerPageView(this, pagevieweventselector(BackPackDialog::onPageScrollCallback));
	mPropDescNode = dynamic_cast<Node* >(mRoot->getChildByName("PropDescNode"));  
	mPropDescNode->setPosition(ccp(0, 0));

	Button* pCloseBtn = dynamic_cast<Button*>(mRoot->getChildByName("CloseBtn"));
	pCloseBtn->setPosition(ccp(0, 0));

	mPveBtn = dynamic_cast<Button*>(mRoot->getChildByName("pveBtn"));
	mPveBtn->setPosition(ccp(0, 0));
	mShowBtnMap.insert(std::make_pair("pveTxt", mPveBtn));;

	mPvpBtn = dynamic_cast<Button*>(mRoot->getChildByName("pvpBtn"));
	mPvpBtn->setPosition(ccp(0, 0));
	mShowBtnMap.insert(std::make_pair("pvpTxt", mPvpBtn));

	mGiftBtn = dynamic_cast<Button*>(mRoot->getChildByName("giftBtn"));
	mGiftBtn->setPosition(ccp(0, 0));
	mShowBtnMap.insert(std::make_pair("giftTxt", mGiftBtn));

	mStuffBtn = dynamic_cast<Button*>(mRoot->getChildByName("stuffBtn"));
	mStuffBtn->setPosition(ccp(0, 0));
	mShowBtnMap.insert(std::make_pair("stuffTxt", mStuffBtn));

	mOpenBoxBtn = dynamic_cast<Button*>(mPropDescNode->getChildByName("BoxBtn"));
	mOpenBoxBtn->setVisible(false);


	mDescLable1 = dynamic_cast<ui::Text*>(mPropDescNode->getChildByName("PropDesc1"));
	mDescLabel2 = dynamic_cast<ui::Text*>(mPropDescNode->getChildByName("PropDesc2"));
	mImg = dynamic_cast<ui::ImageView*>(mPropDescNode->getChildByName("PropImg"));
	//mImg->setScale(2);
	mNameLable = dynamic_cast<ui::Text*>(mPropDescNode->getChildByName("PropName"));
	mPropTxt1 = dynamic_cast<ui::Text*>(mPropDescNode->getChildByName("PropTxt1"));
	mPropTxt2 = dynamic_cast<ui::Text*>(mPropDescNode->getChildByName("PropTxt2"));

  
	changeBone(mArmatureBg, "beibao_WP", mBgNode);
	mBgNode->removeFromParentAndCleanup(true);
	addChild(mBgNode);
	mArmatureBg->setZOrder(10);

	changeBone(mArmatureBg, "beibao_NR", mPropDescNode);
	changeBone(mArmatureBg, "beibao_guanbi", pCloseBtn);
	changeBone(mArmatureBg, "beibao_biaoti0", mPvpBtn);
	changeBone(mArmatureBg, "beibao_biaoti0sa", mPveBtn);
	changeBone(mArmatureBg, "beibao_biaoti00", mGiftBtn);
	changeBone(mArmatureBg, "beibao_biaoti", mStuffBtn);

	setPopType(E_POP_TYPE_NONE);
	onLocateClickCallback(mRoot);
	onLocateClickCallback(mBgNode);

	mPageTxt = dynamic_cast<ui::Text* >(mBgNode->getChildByName("PageBg")->getChildByName("PageText"));
	mPageTxt->setString("1/1");   

	mPreBtn = dynamic_cast<ui::Button*>(mBgNode->getChildByName("LeftBtn"));
	if (mPreBtn) {
		mPreBtn->setVisible(false);
	}
	mNextBtn = dynamic_cast<ui::Button*>(mBgNode->getChildByName("RightBtn"));
	if (mNextBtn){
		mNextBtn->setVisible(false);
	}
}


void BackPackDialog::refreshUI(bool isProp)
{
	clearPropDesc();

	if (isProp)
		refreshProp();
	else
		refreshStuff(); 
}

void BackPackDialog::refreshProp()
{
	mViewNode->removeAllPages();
	PropList pInfoList; 
	switch (mShowPve) {
	case 0:
		pInfoList = getPvpProp();
		break;
	case 1:
		pInfoList = getPveProp();
		break;
	case 2:
		pInfoList = getGiftProp();
		break;
	default:
		pInfoList = getPveProp();
		return;
	} 
	int ret = pInfoList.size() / MAX_CELLNUM;
	mPageCnt = pInfoList.size() % MAX_CELLNUM == 0 ? ret : ret + 1;
	int itemIndex = 0;
	bool newPage = false;
	int curPage = -1; 
	Layout* propLayout = NULL;
	for (PropList::iterator tableIter = pInfoList.begin(); tableIter != pInfoList.end(); tableIter++)
	{
		int newPage = itemIndex / MAX_CELLNUM;
		if (newPage > curPage && curPage < mPageCnt)
		{
			curPage = newPage;
			propLayout = Layout::create();
			mViewNode->insertPage(propLayout, curPage + 1);
			propLayout->setContentSize(mViewNode->getContentSize());
		}
		PropInfo* pInfo = tableIter->second; 
		int count = sgUserObj()->getProp(pInfo->id);
		BackPropInfoNode* pNode = createInfoNode(itemIndex, curPage, mShowPve, pInfo->id, count); 
		propLayout->addChild(pNode); 
		pNode->setPropTouchCallback(this, callfuncPage_selector(BackPackDialog::onShowPropDescCallback), pNode);
		itemIndex++;
	}
	if (itemIndex > 0) {
		mPageTxt->setString(StringUtils::format("%d/%d", 1, mPageCnt));
	}
	else{
		mPageTxt->setString("");
	}

	if (mPageCnt > 1) {
		showSwitchBtn(true);
	}
	else {
		showSwitchBtn(false);
	}
}

const PropList BackPackDialog::getGiftProp()
{
	mUsableGiftList.clear();

	for (PropList::iterator tableIter = mGiftPropList.begin(); tableIter != mGiftPropList.end(); tableIter++)
	{
		PropInfo* pInfo = tableIter->second;
		int count = sgUserObj()->getProp(pInfo->id);
		if (count > 0){
			mUsableGiftList.insert(std::make_pair(pInfo->id, pInfo));
		}
	}
		
	return mUsableGiftList;
}

void BackPackDialog::refreshStuff()
{
	mViewNode->removeAllPages();
	GlyphStuffList pInfoList = getGlyphStuff(); 

	GlyphStuffList visibleList;

	for (GlyphStuffList::iterator tableIter = pInfoList.begin(); tableIter != pInfoList.end(); tableIter++)
	{
		GlyphStuffData* pInfo = tableIter->second;
		int count = sgUserObj()->getGlyphStuffNum(pInfo->mStuffID);
		if (count > 0) {
			visibleList.insert(std::make_pair(pInfo->mStuffID, pInfo));
		}
	}

	int ret = visibleList.size() / MAX_CELLNUM;
	mPageCnt = visibleList.size() % MAX_CELLNUM == 0 ? ret : ret + 1;
	int itemIndex = 0;
	bool newPage = false;
	int curPage = -1; 
	Layout* propLayout = NULL;
	for (GlyphStuffList::iterator tableIter = visibleList.begin(); tableIter != visibleList.end(); tableIter++)
	{
		int newPage = itemIndex / MAX_CELLNUM;
		if (newPage > curPage && curPage < mPageCnt)
		{
			curPage = newPage;
			propLayout = Layout::create();
			mViewNode->insertPage(propLayout, curPage + 1);
			propLayout->setContentSize(mViewNode->getContentSize());
		}
		GlyphStuffData* pInfo = tableIter->second;
		int count = sgUserObj()->getGlyphStuffNum(pInfo->mStuffID);
		BackPropInfoNode* pNode = createInfoNode(itemIndex, curPage, mShowPve, pInfo->mStuffID, count);
		propLayout->addChild(pNode);
		pNode->setPropTouchCallback(this, callfuncPage_selector(BackPackDialog::onShowPropDescCallback), pNode);
		itemIndex++;
	}
	if (itemIndex > 0) {
		mPageTxt->setString(StringUtils::format("%d/%d", 1, mPageCnt));
	}
	else{
		mPageTxt->setString("");
	}

	if (mPageCnt > 1) {
		showSwitchBtn(true);
	}
	else {
		showSwitchBtn(false);
	}
}


BackPropInfoNode* BackPackDialog::createInfoNode(int refIndex, int pageIndex, int type, int propID, int count)
{
	Point firstCellPos = ccp(80, 231);

	BackPropInfoNode* pNode = BackPropInfoNode::create(mShowPve, propID, count);
	pNode->setPageIndex(pageIndex);
	int selectIndex = refIndex % MAX_CELLNUM;
	if (selectIndex == mSelectIndex)
	{ 
		if (pageIndex == 0)
		{
			pNode->setItemSelect(true);
			onShowPropDescCallback(pageIndex, propID, mSelectIndex);
		}
	}
		
	pNode->setTag(selectIndex);

	Point itemPos;
	int colIndex = refIndex % MAX_CLCUMN;
	int rowIndex = (refIndex - pageIndex * MAX_CELLNUM) / MAX_CLCUMN;
	itemPos.x = firstCellPos.x + colIndex * (pNode->getNodeSize().width + 20);
	itemPos.y = firstCellPos.y - rowIndex * (pNode->getNodeSize().height + 20);
	pNode->setNodePosition(itemPos);

	return pNode;
}

void BackPackDialog::onShowPropDescCallback(int pageIndex, int id, int index)
{
	mSelectIndex = index;
	closeOhterSelectFrame(id);  
	resetPropDesc(mShowPve, id); 
}

void BackPackDialog::clearPropDesc()
{
  mDescLable1->setVisible(false);
  mDescLabel2->setVisible(false);
  mNameLable->setVisible(false);
  setOpenBoxBtn(false);
  mPropTxt2->setVisible(false);
  mPropTxt1->setVisible(false);
  mImg->setVisible(false);
}

void BackPackDialog::resetPropDesc(int type, int id)
{
	std::string name = "";
	std::string icon = "";
	std::string desc1 = "";
	std::string desc2 = "";

	clearPropDesc();

	mPropTxt1->setVisible(true);

	if (type != E_BackPack_Stuff)
	{
		auto pInfo = sgTableManager()->getPropInfoById(id);
		if (!pInfo)
			return;
		name = pInfo->name;
		icon = pInfo->icon;
		desc1 = pInfo->explain1;
		desc2 = pInfo->explain2;
	}
	else
	{
		auto pInfo = sgTableManager()->getGlyphStuffData(id);
		if (!pInfo)
			return;
		name = pInfo->mStuffName;
		icon = pInfo->mStuffIcon;
		desc1 = pInfo->mStuffDesc; 
	}

	
	if (desc1 != "")
	{
		mDescLable1->setText(desc1);
		mDescLable1->setVisible(true);
	}

	if (desc2 != "")
	{
		mDescLabel2->setText(desc2);
		mDescLabel2->setVisible(true);
	}

	if (name != "")
	{
		mNameLable->setText(name);
		mNameLable->setVisible(true);
	}

	if (icon != "")
	{
		mImg->loadTexture(icon, ui::Widget::TextureResType::PLIST);
		mImg->setVisible(true);
	}


	mOpenBoxCount = 0;
	mToolsId = 0;
	mPropTxt1->setString(sgTextHelper(E_BackPackTxt1_Prop));
	// show the open box btn
	if (mShowPve == E_BackPack_Gift) {
		mOpenBoxCount = sgUserObj()->getProp(id);
		mToolsId = id;
		if (mOpenBoxCount > 0)
			setOpenBoxBtn(true, mOpenBoxCount, 1);
		else
			setOpenBoxBtn(true, mOpenBoxCount, 0); 
		mDescLabel2->setVisible(false);
	} 
	else if (mShowPve == E_BackPack_Stuff) {
		mPropTxt1->setString(sgTextHelper(E_BackPackTxt1_Stuff));
	}
	else
	{
		mPropTxt2->setVisible(true);
		mDescLabel2->setVisible(true);
	}
}

void BackPackDialog::closeOhterSelectFrame(int id)
{ 
	for (int pageIndex = 0; pageIndex < mPageCnt; pageIndex++)
	{
		if (!mViewNode->getPage(pageIndex))
			continue;
		cocos2d::Vector<Node* > pVec = mViewNode->getPage(pageIndex)->getChildren();
		cocos2d::Vector<Node* >::iterator iter = pVec.begin();
		for (; iter != pVec.end(); iter++)
		{
			BackPropInfoNode* pNode = dynamic_cast<BackPropInfoNode*>(*iter);
			if (pNode && pNode->getPropID() != id)
				pNode->setItemSelect(false);
			else
				pNode->setItemSelect(true);
		}
	} 
}

 

void BackPackDialog::showPvpProp()
{
	mShowPve = E_BackPack_Pvp;
	mSelectIndex = 0;
	refreshUI();
	resetShowBtn("pvpTxt"); 
}


void BackPackDialog::shopPveProp()
{
	mShowPve = E_BackPack_Pve;
	mSelectIndex = 0;
	refreshUI();  
	resetShowBtn("pveTxt"); 
}

void BackPackDialog::showGift()
{
	mShowPve = E_BackPack_Gift;
	mSelectIndex = 0;
	refreshUI();
	resetShowBtn("giftTxt"); 
}

void BackPackDialog::showStuff()
{
	sgUserObj()->loadGlyphStuff();
	mShowPve = E_BackPack_Stuff;
	mSelectIndex = 0;
	refreshUI(false);
	resetShowBtn("stuffTxt");
}

void BackPackDialog::setOpenBoxBtn(bool isShow, int count, int status)
{
	if (mOpenBoxBtn) {
		mOpenBoxBtn->setEnabled(isShow);
		mOpenBoxBtn->setVisible(isShow);
		mOpenBoxBtn->setBright(isShow);

		if (count >= 10) {
			mOpenBoxBtn->getChildByName("all")->setVisible(false);
			mOpenBoxBtn->getChildByName("ten")->setVisible(true);
		}
		else {
			mOpenBoxBtn->getChildByName("all")->setVisible(true);
			mOpenBoxBtn->getChildByName("ten")->setVisible(false);
		}
		
	}

	if (status == 0) {
		mOpenBoxBtn->setEnabled(false);
		mOpenBoxBtn->setBright(false);
	} 
}

void BackPackDialog::onPageScrollCallback(Ref* ref, ui::PageViewEventType type)
{
	if (type == ui::PageViewEventType::PAGEVIEW_EVENT_TURNING)
	{
		int curPageIndex = mViewNode->getCurPageIndex();
		mPageTxt->setString(StringUtils::format("%d/%d", curPageIndex + 1, mPageCnt));
	}
}

void BackPackDialog::resetShowBtn(std::string typeStr)
{
	std::map<std::string, Button* >::iterator iter = mShowBtnMap.begin();
	for (; iter != mShowBtnMap.end(); iter++)
	{
		Button* pBtn = iter->second;
		ui::ImageView* img = dynamic_cast<ui::ImageView*>(pBtn->getChildByName("Image_1"));
		if (!img)
			continue;
		if (iter->first == typeStr)
		{
			pBtn->setBrightStyle(BrightStyle::HIGHLIGHT);
			img->loadTexture(StringUtils::format("ui/backPack/%s%d.png", iter->first.c_str(), 1), ui::TextureResType::PLIST);
		} 
		else
		{
			pBtn->setBrightStyle(BrightStyle::NORMAL);
			img->loadTexture(StringUtils::format("ui/backPack/%s%d.png", iter->first.c_str(), 0), ui::TextureResType::PLIST);
		}
		img->ignoreContentAdaptWithSize(false);
	}
}

void BackPackDialog::goNextPage()
{
	CCLOG("  onBtn on Right ..................... ");
	int curPage = mViewNode->getCurPageIndex();
	mViewNode->scrollToPage(++curPage);
}

void BackPackDialog::goPrePage()
{
	CCLOG("  onBtn on Left ..................... ");
	int curPage = mViewNode->getCurPageIndex();
	mViewNode->scrollToPage(--curPage);
}
 
void BackPackDialog::showSwitchBtn(bool isShow)
{
	if (mPreBtn && mNextBtn) {
		mPreBtn->setVisible(isShow);
		mNextBtn->setVisible(isShow);
	}
}
