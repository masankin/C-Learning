#include "NewsBibleDialog.h"
#include "SoundManager.h"
#include "ui/PageFunction.h"
#include "battle/BattleRecordsManager.h"
#include "guide/GuideManager.h"
#include "lua/LuaFuncHelper.h"
NewsBibleDialog::NewsBibleDialog()
{
	mNewPassLevel = 0;
}

NewsBibleDialog::~NewsBibleDialog()
{

}

bool NewsBibleDialog::init(){
	bool ret = false;
	do
	{
		mIsScroll = false;

		mRoot = CSLoader::createNode("ui/NewsBibleDialog.csb");
		CC_BREAK_IF(!mRoot);

		mRoot->setAnchorPoint(Vec2(0.5, 0.5));
		this->addChild(mRoot);

		setPopType(E_POP_TYPE_NONE);
		
		//滚动面板
		mScrollView = dynamic_cast<ui::ScrollView*>(mRoot->getChildByName("ScrollView_2"));
		mScrollView->addTouchEventListener(this, toucheventselector(NewsBibleDialog::onScroll));

		//添加自定义列表控件
		Button* keepTabBtn = nullptr,*keepTabItemBtn = nullptr;
		int tabItemId = sgUserObj()->getBibleTabItemId();

		checkHasPassNewLevel();

		Bible* bibleItem = sgTableManager()->getBible(tabItemId);
		if (!bibleItem || bibleItem->mark == 4)
		{
			tabItemId = 101;
			sgUserObj()->setBibleTabItemId(tabItemId);
		}

		mListBox = CustomDropDownListBox::DropDownList::create();
		mListBox->setTabSpace(10);
		mListBox->setTabItemSpace(5);

		BibleList* bibleList = sgTableManager()->getBibleList();

		Button* tabBtn,*tabItemBtn;
		BibleList::iterator it = bibleList->begin();
		for (; it != bibleList->end(); it++)
		{
			int itId = it->first;
			Bible* bibleItem = it->second;

			if (bibleItem->mark == 4) continue;

			if (itId % 100 == 0)
			{
				tabBtn = mListBox->addTab(bibleItem->title, bibleItem->mark, bibleItem->id);
				if (tabItemId / 100 * 100 == itId)
					keepTabBtn = tabBtn;	
			}
			else
			{
				tabItemBtn = mListBox->addTabItem(bibleItem->title, bibleItem->mark, tabBtn, bibleItem->id);
				if (tabItemId == itId)
					keepTabItemBtn = tabItemBtn;
			}

		}

		mScrollView->setInnerContainerSize(Size(mScrollView->getInnerContainerSize().width, (mListBox->getTabSize().height + mListBox->getTabSpace()) * mListBox->getTabCount() + 11.5));
		mListBox->setPosition(Point(mScrollView->getContentSize().width/2, mScrollView->getInnerContainerSize().height-11.5));
		mScrollView->addChild(mListBox);

		setAnimation("fla_Xinshouyindao");

		mImage = NULL;
		mArmature = getArmatureById(NEWCAMP_TAG);
		if (mArmature != NULL)
		{
			mArmature->getAnimation()->play("play", -1, 1);
			mArmature->setPosition(Point(0, 0));
			this->addChild(mArmature);
			changeBone(mArmatureBg, "Xinshouyindao_NR", mArmature);
		}

		auto returnBtn = Button::create("ui/general/general_close.png","ui/general/general_close.png","",TextureResType::PLIST);
		returnBtn->setTouchEnabled(true);
		returnBtn->setCallbackName("onReturn");
		returnBtn->setCallbackType("Click");
		this->addChild(returnBtn,10);

		changeBone(mArmatureBg, "Xinshouyindao_PZ", mRoot);	
		changeBone(mArmatureBg, "Xinshouyindao_caca", returnBtn);

		onLocateClickCallback(this);
		
		onClickTab(keepTabBtn);
		onClickTabItem(keepTabItemBtn);

		ret = true;

		sgPageFunction()->setCurrentUIType(MSG_FUC_MAIN_LEVEL);

	} while (0);


	return ret;
}

cocostudio::Armature* NewsBibleDialog::getArmatureById(int id)
{
	Armature* armature = NULL;
	const Bible* bible = sgTableManager()->getBible(id);
	std::string strSource = bible->icon;
	char strDes[4] = "000";
	strncpy(strDes, strSource.c_str(), 3);
	if (strcmp(strDes, "fla") == 0)
		armature = Armature::create(bible->icon);	
	return armature;
}

void NewsBibleDialog::onMenuItemAction(std::string cName, cocos2d::Ref* sender)
{
	if (cName == "onReturn")
	{
		this->setShow(false);
		sgPageFunction()->enableOptionButton(true);
		sgMessageManager()->sendMessage(EMT_SELECTWORD_SCHEDULE, "");
		sgMessageManager()->sendMessage(EMT_CLOSEPAGE, "");
		sgGuideManager()->setOpenRookieCamp(false);		
		return;
	}	
	else if (cName == "onSelect")
	{		
		int tag = dynamic_cast<Node*>(sender)->getTag();
		//if (tag > E_NEWCAMP_1 && sgUserObj()->getCheckPointState(tag - 10) != E_CHECKPOINTSTATE_PASS){
		if (tag > E_NEWCAMP_1 && sgUserObj()->getGateState(tag - 10) != E_CHECKPOINTSTATE_PASS){
			std::string szTitle = sgTextHelper(TEXT_COMMTIPS_TIPS);
			std::string szContent = sgTextHelper(TEXT_COMMTIPS_CHECKPT);
			GenerlDialog::show(szTitle, szContent);
			return;
		}
		if (tag == E_NEWCAMP_6 && sgUserObj()->getMaxOpenGateID() < 3010)
		{
			GenerlDialog::show(sgTextHelper(TEXT_COMMTIPS_TIPS), sgTextHelper(TEXT_GUIDE_LEVEL_TIP));
			return;
		}
		const char* pScript = String::createWithFormat("sgBthControl():sendPveNofityRequest(%d,1,99,0)", tag)->getCString();
		LuaEngine::getInstance()->executeString(pScript);

		sgGamePrecedure()->selectLevel(tag);
		PageFunction::Get()->gotoFunction(MSG_FUC_PLAYING);
		sgGuideManager()->rollbackRookieGuide();
		return;
	}
	int tag = Value(cName).asInt()%100;
	if (!mIsScroll)
	{
		if (tag == 0 && Value(cName).asInt() != 0)
		{
			onClickTab(sender);
		}
		else
		{
			onClickTabItem(sender);
		}
	}
}

void NewsBibleDialog::onClickTab(cocos2d::Ref* sender){
	auto tabBtn = static_cast<Button*>(sender);
	int tabId = Value(tabBtn->getCallbackName()).asInt();
	int tabitemCount = 0;

	if (mListBox->getCurTabId() == tabId)
	{	
		if (tabId == NEWCAMP_TAG - 1) return;

		mListBox->showTabItem(false, tabBtn);
		mListBox->moveTab(false, tabBtn);
		mListBox->onTabIsSelected(false, tabBtn);
		mListBox->onTabItemIsSelected(false, tabBtn);
		mListBox->setCurTabId(0);
		mListBox->setCurTabItemId(0);
	}
	else
	{	
		updateBibleData(tabId);	
		tabBtn->removeChildByTag(12);

		tabitemCount = mListBox->getTabItemCountByTabId(tabId);

		auto curBtn = mListBox->getCurTab();
		if (curBtn != NULL)
		{
			mListBox->showTabItem(false, curBtn);
			mListBox->moveTab(false, curBtn);
			mListBox->onTabIsSelected(false, curBtn);
			mListBox->onTabItemIsSelected(false, curBtn);
		}

		mListBox->setCurTabId(tabId);
		mListBox->showTabItem(true,tabBtn);
		mListBox->moveTab(true,tabBtn);
		mListBox->onTabIsSelected(true, tabBtn);
		auto tabItemBtn = mListBox->getSelectedTabItemByTabId(tabId);
		if (tabItemBtn != NULL)
			onClickTabItem(tabItemBtn);
		else
			mListBox->setCurTabItemId(0);
	}

	mScrollView->setInnerContainerSize(Size(mScrollView->getInnerContainerSize().width, (mListBox->getTabSize().height + mListBox->getTabSpace())* mListBox->getTabCount() + tabitemCount*mListBox->getTabItemSize().height + 11.5));
	mScrollView->getInnerContainer()->setPositionY(mScrollView->getContentSize().height - mScrollView->getInnerContainerSize().height);
	mListBox->setPosition(Point(mScrollView->getContentSize().width / 2, mScrollView->getInnerContainerSize().height-11.5));
}

void NewsBibleDialog::onClickTabItem(cocos2d::Ref* sender){
	auto tabItemBtn = static_cast<Button*>(sender);
	int tabItemId = Value(tabItemBtn->getCallbackName()).asInt();
	
	if (mListBox->getCurTabItemId() == tabItemId)
		return;
	
	updateBibleData(tabItemId);
	tabItemBtn->removeChildByTag(12);

	sgUserObj()->setBibleTabItemId(tabItemId);
	auto curBtn = mListBox->getCurTab();

	mListBox->onTabItemIsSelected(false, curBtn);
	mListBox->setCurTabItemId(tabItemId);
	mListBox->onTabItemIsSelected(true, curBtn);

	if (mImage != NULL)
	{
		mImage->removeFromParentAndCleanup(true);
		mImage = NULL;
	}	
	if (mArmature != NULL)
	{
		mArmature->removeFromParentAndCleanup(true);
		mArmature = NULL;
	}

	mArmature = getArmatureById(tabItemId);
	if (mArmature != NULL)
	{
		const Bible* bible = sgTableManager()->getBible(tabItemId);
		int loop = bible->loop;
		mArmature->getAnimation()->play("play", -1, loop);
		mArmature->setPosition(Point(0, 0));
		this->addChild(mArmature);
		changeBone(mArmatureBg, "Xinshouyindao_NR", mArmature);

		if (tabItemId <= NEWCAMP_TAG)
		{
			newCampChangeBone();
		}
	}
	else
	{
		const Bible* bible = sgTableManager()->getBible(tabItemId);
		mImage = ImageView::create("ui/NewsBible/" + bible->icon + ".png", TextureResType::PLIST);
		mImage->setPosition(Point(0.0, 0.0));
		this->addChild(mImage);
		changeBone(mArmatureBg, "Xinshouyindao_NR", mImage);
	}
}

void NewsBibleDialog::onScroll(Ref* ref, Widget::TouchEventType type){
	if (type == Widget::TouchEventType::MOVED)
	{	
		auto m = mScrollView->getTouchDownPoint();
		auto n = mScrollView->getTouchCurPoint();
		if (abs(m.y-n.y)>10)
		{
			mIsScroll = true;
		}
	}

	if (type == Widget::TouchEventType::ENDED)
	{
		mIsScroll = false;
	}

}

void NewsBibleDialog::onEnter()
{
	ActionWindow::onEnter();

	sgSoundManager()->playMusicBg(MUSIC_UI_TEACH_DIALOG);
	
	if (mArmature != NULL)
	{
		auto animation = mArmature->getAnimation();
		const Bible* bible = sgTableManager()->getBible(sgUserObj()->getBibleTabItemId());
		int loop = bible->loop;
		animation->play("play",-1,loop);
	}
	//发送请求奖励领取
	sgUserObj()->getNewBieReward();

	sgGuideManager()->unRegistGuideMessage(); 
	std::map<int, bool> idMap;
	idMap.insert(std::make_pair(GuideGroup_SelectRookieCamp, true));
	idMap.insert(std::make_pair(GuideGroup_SkipRookieCamp, true));
	sgUserObj()->syncGuideFinish(idMap);

	Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("gDoneScene");
}

void NewsBibleDialog::onExit()
{
	if (mArmature != NULL)
	{
		auto animation = mArmature->getAnimation();
		animation->stop();
	}

	sgSoundManager()->playLastMusicBg();

	ActionWindow::onExit();

	sgUserObj()->saveToFile();
}

void NewsBibleDialog::aniCallback(cocostudio::Armature * pAr, cocostudio::MovementEventType type, const std::string& aniName)
{
	if (mArmature != NULL)
		mArmature->getAnimation()->setSpeedScale(0.5f);
}

void NewsBibleDialog::newCampChangeBone()
{
	Node* mRoot1 = CSLoader::createNode("ui/NewCampPanel.csb");
	mArmature->addChild(mRoot1);
	mRoot1->setTag(NEWCAMP_TAG);

	bool flag = false;
	for (int i = E_NEWCAMP_1; i <= E_NEWCAMP_6; i += 10)
	{
		std::string str = StringUtils::format("XSjiaoxue_q%d", i);
		std::string str1 = StringUtils::format("Image_%d", i);
		std::string str2 = StringUtils::format("XSjiaoxue_yitongguo%d", i);
		std::string str3 = StringUtils::format("XSjiaoxue_%d", i);
		std::string str4 = StringUtils::format("XSjiaoxue_jiantou%d", i);
		std::string str5 = StringUtils::format("XSjiaoxue_jiangli%d", i);
		std::string str6 = StringUtils::format("Text_%d", i);

		ImageView* pCampImage = dynamic_cast<ImageView*>(mRoot1->getChildByName(str1));

		changeBone(mArmature, str, pCampImage);
		if (i != E_NEWCAMP_1)
			changeBone(mArmature, str5, mRoot1->getChildByName(str6));
		else
			changeBone(mArmature, str5, Node::create());
		pCampImage->setTag(i);
		cocostudio::Bone* pBone2 = mArmature->getBone(str2);
		cocostudio::Bone* pBone3 = mArmature->getBone(str3);
		cocostudio::Bone* pBone4 = mArmature->getBone(str4);
		cocostudio::Bone* pBone5 = mArmature->getBone(str5);
		if (pBone2)
		{
			if (sgUserObj()->getGateState(i) != E_CHECKPOINTSTATE_PASS)
			{
				if (i == E_NEWCAMP_6 && sgUserObj()->getGateState(3010) == E_CHECKPOINTSTATE_CLOSE)
				{
					//ui::Scale9Sprite* sp = dynamic_cast<ui::Scale9Sprite*>(pCampImage->getVirtualRenderer());
					//CommFunc::commSetGray(sp->getSprite());
					pCampImage->loadTexture("ui/NewsBible/campNoSelect.png", cocos2d::ui::Widget::TextureResType::PLIST);
					pBone3->getChildArmature()->getAnimation()->play("play2");
					continue;
				}
				pBone2->getChildArmature()->getAnimation()->play("born");
				if (!flag){
					pCampImage->loadTexture("ui/NewsBible/campSelect.png", cocos2d::ui::Widget::TextureResType::PLIST);
					flag = true;

					Armature* pAni = Armature::create("fla_tishi");
					pAni->getAnimation()->play("play");
					pAni->setScale(0.5f);
					pBone2->addDisplay(pAni, 0);
					pBone2->setRotation(45.0f);
				}
				else
				{
					//ui::Scale9Sprite* sp = dynamic_cast<ui::Scale9Sprite*>(pCampImage->getVirtualRenderer());
					//CommFunc::commSetGray(sp->getSprite());
					pCampImage->loadTexture("ui/NewsBible/campNoSelect.png", cocos2d::ui::Widget::TextureResType::PLIST);
					pBone3->getChildArmature()->getAnimation()->play("play2");
				}
				if (pBone4){
					pBone4->getChildArmature()->getAnimation()->play("play1");
				}
				if (i == E_NEWCAMP_5) continue;
				auto pText = dynamic_cast<Text*>(mRoot1->getChildByName(str6)->getChildByName("Text_2"));
				if (!pText) continue;
				const CheckPoint* pCP = sgTableManager()->getCheckPointByID(i);
				if (!pCP) continue;
				const GiftAward* pGiftAward = sgTableManager()->getGiftAward(pCP->baseBonus);
				if (pGiftAward && pGiftAward->gold)
				{
					pText->setString(StringUtils::format("x%d", pGiftAward->gold));
				}
			}
			else{
				if (pBone5)
				{
					pBone5->addDisplay(Node::create(), 0);
				}
				pCampImage->loadTexture("ui/NewsBible/campNotSelect.png", cocos2d::ui::Widget::TextureResType::PLIST);
				if (mNewPassLevel == i)
				{
					pBone2->getChildArmature()->getAnimation()->play("stand");

					if (pBone4){
						pBone4->getChildArmature()->getAnimation()->play("play2");
					}
					pBone3->getChildArmature()->getAnimation()->play("play1");
					mNewPassLevel = 0;
				}
				else
				{
					pBone2->getChildArmature()->getAnimation()->play("end");

					if (pBone4){
						pBone4->getChildArmature()->getAnimation()->play("play3");
					}
				}
			}
		}
	}
	//changeBone(mArmature, "XSjiaoxue_TB", mRoot1->getChildByName("Text_1"));
	changeBone(mArmature, "XSjiaoxue_TD", mRoot1->getChildByName("Text_2"));
	changeBone(mArmature, "XSjiaoxue_dise", mRoot1->getChildByName("Sprite_1"));
	changeBone(mArmature, "XSjiaoxue_TD2", mRoot1->getChildByName("Text_3"));
	onLocateClickCallback(mRoot1);
}

void NewsBibleDialog::checkHasPassNewLevel()
{
	BibleList* bibleList = sgTableManager()->getBibleList();
	BibleList::iterator it = bibleList->begin();
	for (; it != bibleList->end(); it++)
	{
		int itId = it->first;	
		Bible* bibleItem = it->second;
		if (bibleItem->level == 0) continue;
		if (sgUserObj()->getGateState(bibleItem->level) == E_CHECKPOINTSTATE_PASS)
		{
			int size = sgUserObj()->getSaveData()->bibledata().size();
			bool flag = false;
			for (int i = 0; i < size; i++)
			{
				fgame::NewsBibleData* pData = sgUserObj()->getSaveData()->mutable_bibledata(i);
				if (pData->bibleid() == itId)
				{
					flag = true;
					if (pData->pass() != 2)
					{
						if (pData->pass() != 1)
							mNewPassLevel = bibleItem->level;
						pData->set_pass(1);
						if (itId % 100 == 0)
							bibleItem->mark = 2;
						else
							bibleItem->mark = 3;
					}
				}
			}
			if (!flag)
			{
				fgame::NewsBibleData* pData = sgUserObj()->getSaveData()->add_bibledata();
				pData->set_bibleid(itId);
				pData->set_pass(1);
				mNewPassLevel = bibleItem->level;
				if (itId % 100 == 0)
					bibleItem->mark = 2;
				else
					bibleItem->mark = 3;
			}
		}
		else
		{
			bibleItem->mark = 4;
		}
	}
}

void NewsBibleDialog::updateBibleData(int id)
{
	Bible* bibleItem = sgTableManager()->getBible(id);
	if (bibleItem)
		bibleItem->mark = 0;
	int size = sgUserObj()->getSaveData()->bibledata().size();
	for (int i = 0; i < size; i++)
	{
		fgame::NewsBibleData* pData = sgUserObj()->getSaveData()->mutable_bibledata(i);
		if (pData->bibleid() == id)
		{
			pData->set_pass(2);
		}
	}
}
