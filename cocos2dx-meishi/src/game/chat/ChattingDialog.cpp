#include "ChattingDialog.h"
#include "game/ui/PageFunction.h"
#include "channel/ChannelApi.h"
#include "game/data/TableManager.h"
#include "EnvSet.h"
#include "guide/GuideManager.h"
#include "MessageManager.h"
#include "cocos2d.h"
#include "game/net/UserObj.h"
#include "core/WordChecker.h"
#include "SoundManager.h"
#include "ChattingControl.h"
#include "InfoPanel.h"
#include "pvp/PvpRoomControl.h"
#include "Gif/GifAnimation.h"
#include "game/utils/Comm.h"
#include "game/prop/PropsControl.h"
#include "core/SoundManager.h"

#define Emoji_Width (150)
#define Emoji_Height (96)
#define Word_Width (350)
#define Word_Width_Add (75)
#define Word_Height_Add (35)
#define Chatting_Bg_Width (550)
#define Image_Emoji_Width (120)
#define Image_Word_Add_Width (50)


ChattingButton::ChattingButton()
{
	mShow = false;
	m_pLabel = NULL;
	sgMessageManager()->regisiterMessageHandler(EMT_CHAT_MSG, this);
}

ChattingButton::~ChattingButton()
{
	sgMessageManager()->removeMessageHandler(this);
}


ChattingButton* ChattingButton::create()
{
	ChattingButton* pret = new ChattingButton();
	if (pret && pret->init())
	{
		return pret;
	}
	CC_SAFE_DELETE(pret);
	return NULL;
}

void ChattingButton::onReceiveMassage(GameMessage * message)
{
	if (message->getMessageType() == EMT_CHAT_MSG)
	{
		MessageChatMsg* pMes = dynamic_cast<MessageChatMsg*>(message);
		if (pMes && pMes->getChatType() == MSG_TYPE_WORLD)
		{
			changeChatMsg();
		}
	}
}

bool ChattingButton::init()
{
	Node* mRoot = CSLoader::getInstance()->createNode("ui/ChattingButton.csb");
	if (!mRoot)
	{
		CCLOG("[error] ArrowWidget::initUI load csb error!");
		return false;
	}
	this->addChild(mRoot);

	//mRoot->setPositionX(-Director::getInstance()->getWinSize().width/2);
	mImageBg = dynamic_cast<ImageView*>(mRoot->getChildByName("ImageChatBg"));
	mImageBg->addClickEventListener(CC_CALLBACK_1(ChattingButton::chatCallback, this));
	mImageBg->setVisible(false);

	Button* mButton = dynamic_cast<Button*>(mRoot->getChildByName("BtnChat"));
	mButton->addClickEventListener(CC_CALLBACK_1(ChattingButton::chatCallback, this));
	return true;
}

void ChattingButton::resetUI(bool flag)
{	
	if (flag)
	{		
		mShow = flag;
		mImageBg->setContentSize(Size(310,63));
		this->setPositionX(100);
		changeChatMsg();
	}
	else if(mShow != flag)
	{
		mShow = flag;
		mImageBg->setContentSize(Size(391,63));
		this->setPositionX(0);		
		changeChatMsg();
	}
}

void ChattingButton::chatCallback(cocos2d::Ref* psender)
{
	CCLOG("[trace]ChattingButton::chatCallback");
	if(sgGuideManager()->getGuideLock() == true)
		return;

	sgSoundManager()->playMusicEffect(SOUND_CLICK_COMMON);
	sgPageFunction()->gotoFunction(MSG_FUC_CHATMSG);
}

void ChattingCell::onClickBuyEngerCallBack(cocos2d::Ref*)
{
	//if (sgUserObj()->getPvpPower() >= POWER_BUY_MAX)
	if (sgUserObj()->getPersonalInfo()->energy() > POWER_BUY_MAX)
	{
		GenerlDialog::show(sgTextHelper(TEXT_COMMTIPS_TIPS),
			__String::createWithFormat("%s%s", sgTextHelper(TEXT_ACT_JINGLI).c_str(), sgTextHelper(TEXT_BUY_POWER_MAX).c_str())->getCString());
		return;
	}
	PowerBuy* pPowerBuy = sgTableManager()->getPowerBuy(sgUserObj()->getPersonalInfo()->energybuycount());//sgUserObj()->getEnergyBuyCount());
	if (pPowerBuy)
	{
		//if (sgUserObj()->getGoldCoin() >= pPowerBuy->getEnergyNum())
		if (sgUserObj()->getPersonalInfo()->diamond() >= pPowerBuy->getEnergyNum())
		{
			sgPropsControl()->sendBuyPropRqst(PVP_ENERGY_TYPE, pPowerBuy->getEnergyNum(), 1);
		}
		else
		{
			PageFunction::Get()->gotoFunction(MSG_FUC_WIN_SHOP);
		}
		GenerlDialog::hide();
	}
}


ChattingCell* ChattingCell::create(ChatInfo* info)
{
	ChattingCell* pret = new ChattingCell();
	if (pret && pret->init(info))
	{
		return pret;
	}
	CC_SAFE_DELETE(pret);
	return NULL;
}

void ChattingCell::update(float delta)
{
	mText->setTag(MSG_TYPE_NULL);
	mText->setTextColor(cocos2d::Color4B(166,166,166,255));
}

bool ChattingCell::init(ChatInfo* info)
{
	mText = NULL;
	Node* mRoot = nullptr;
	bool isMe = true;
	int vipLevel = 0;
	if (info->mUin == sgUserObj()->getUin())
	{
		mRoot = CSLoader::getInstance()->createNode("ui/ChattingCell1.csb");
		isMe = true;
		vipLevel = sgUserObj()->getPersonalInfo()->viplevel();//sgUserObj()->getVipLevel();
	}
	else
	{
		mRoot = CSLoader::getInstance()->createNode("ui/ChattingCell.csb");
		isMe = false;
		vipLevel = info->mVipLevel;
	}
	mRoot->setAnchorPoint(Vec2(0, 0));
	ui::Text* pText = dynamic_cast<ui::Text*>(mRoot->getChildByName("Text_Name"));
#ifdef MGAME_PLATFORM_MPLUS
	if (pText) {
		pText->setFontName(DEFAULT_FONT);
	}
#endif
	pText->setText(info->mName);
	auto spriteTitle = dynamic_cast<Sprite*>(mRoot->getChildByName("Sprite_2"));
	if (spriteTitle)
	{
		spriteTitle->setVisible(false);
		if (info->mTitle)
		{
			auto pData = sgTableManager()->getBadgeData(info->mTitle);
			if (pData && pData->getActive() != 0)
			{
				spriteTitle->setSpriteFrame(GetPlayerBadgeIconNameById(pData->getId(), true));
				spriteTitle->setScale(0.3f);
				spriteTitle->setVisible(true);
			}
		}
		
	}

#ifdef	MGAME_PLATFORM_MPLUS
// add vip icon to name
addVipSymbol(pText,vipLevel,isMe);
#endif

	ui::Button* mHeadBtn = dynamic_cast<ui::Button*>(mRoot->getChildByName("Button_7"));
	if (info->mUin != sgUserObj()->getUin())
	{
		mHeadBtn->addClickEventListener(CC_CALLBACK_1(ChattingCell::popCharaterInfo, this,info->mUin));
		mHeadBtn->setTag(0);
	}	
	mHeadBtn->setUserData(info);
	
	const char* icon = FriendSystem::GetPlayerIconNameFromId(info->iconID);
	mHeadBtn->loadTextureNormal(icon,TextureResType::PLIST);

	if (info->mContentType == ContentType_Normal || info->mContentType == ContentType_Challenge)
	{
		ui::Text* pText = dynamic_cast<ui::Text*>(mRoot->getChildByName("Text_2"));
#ifdef MGAME_PLATFORM_MPLUS
		if (pText) {
			pText->setFontName(DEFAULT_FONT);
		}
#endif
		mText = dynamic_cast<ui::Text*>(mRoot->getChildByName("Text_1"));
#ifdef MGAME_PLATFORM_MPLUS
		if (mText) {
			mText->setFontName(DEFAULT_FONT);
		}
#endif
		pText->setText(info->mContent);

		int hid = mText->getFontSize() + 13;
		int wid = pText->getContentSize().width < Word_Height_Add ? Word_Height_Add : pText->getContentSize().width;

		if (wid > Word_Width)
		{
			int row = ((int)wid) % Word_Width ;
			int line = wid / Word_Width;
			line = (row == 0 ? line : line + 1);

			hid = hid *line;
			wid = Word_Width;
		}
		
		mImage = dynamic_cast<ImageView*>(mRoot->getChildByName("Image_2"));
		
		if (info->mContentType == ContentType_Challenge)
		{
			if(info->mMsgType == MSG_TYPE_NULL)
			{
				mText->setTextColor(cocos2d::Color4B(166,166,166,255));
				mImage->setTouchEnabled(false);
			}
			else
			{
				mText->setTextColor(cocos2d::Color4B(70,194,14,255));
				mImage->setTouchEnabled(true);
			}		
			mImage->addClickEventListener(CC_CALLBACK_1(ChattingCell::inviteCallback, this, info->mUin));
			mText->setTag(info->mMsgType);
			mText->setName(info->mName);
			mText->setCallbackName(info->mServerNode);

			this->scheduleOnce(schedule_selector(ChattingCell::update),Msg_Invite_Time);
		}
		else
		{
			mText->setTag(MSG_TYPE_NULL);
		}

		mText->setText(info->mContent);	
		mText->setContentSize(Size(wid,hid));
		pText->setString("");
		mImage->setContentSize(Size(mText->getContentSize().width + Word_Width_Add,mText->getContentSize().height + Word_Height_Add));
		this->setContentSize(Size(Chatting_Bg_Width,mImage->getContentSize().height + Image_Word_Add_Width));
	}	
	else if (info->mContentType == ContentType_Emoji)
	{
		Node* node = mRoot->getChildByName("Node_2");
		std::string imageName = info->mContent;
		imageName = FileUtils::getInstance() -> fullPathForFilename(imageName.c_str());

		// 动画定义
		GifAnimationDef def;
		def.loops = -1;						// 循环次数
		def.filePath = imageName;				// 文件路径
		def.delayPerUnit = 0.08f;			// 每帧间隔
		def.restoreOriginalFrame = true;	// 还原初始状态

		// 创建动画
		auto pAnimation = GifAnimation::Get()->createAnimation(def);

		// 创建精灵播放动画
		auto ani = Sprite::create();
		ani->runAction(Animate::create(pAnimation));

		if(ani){
			node->addChild(ani);
		}	
		mRoot->getChildByName("Image_2")->setContentSize(Size(Emoji_Width,Emoji_Height));
		this->setContentSize(Size(Chatting_Bg_Width,Image_Emoji_Width));
	}
	this->addChild(mRoot);
	return this;
}

void ChattingCell::addVipSymbol(Node* nameNode,int vipLevel, bool isMe)
{
	if (!nameNode) return;

	// test
	// vipLevel = 3;
	//CCLOG("[jim]  vip level === %d", vipLevel);

	if (vipLevel < 1 || vipLevel > 3) return;

	std::string vipIconName = StringUtils::format("ui/general/general_vip%d.png", vipLevel);
	Vec2 posbegin = nameNode->getPosition();
	Sprite *vip_begin = Sprite::createWithSpriteFrameName(vipIconName);
	nameNode->getParent()->addChild(vip_begin);
	Sprite *vip_end = Sprite::createWithSpriteFrameName(vipIconName);
	nameNode->getParent()->addChild(vip_end);
	vip_end->setRotationY(180);
	if (isMe) {
		nameNode->setPosition(Vec2(posbegin.x - vip_begin->getContentSize().width, posbegin.y));
		vip_begin->setPosition(Vec2(nameNode->getPosition().x - nameNode->getContentSize().width - vip_begin->getContentSize().width / 2, posbegin.y));
		vip_end->setPosition(Vec2(posbegin.x - vip_begin->getContentSize().width / 2, posbegin.y));
	}
	else {
		nameNode->setPosition(Vec2(posbegin.x + vip_begin->getContentSize().width, posbegin.y));
		vip_begin->setPosition(Vec2(posbegin.x + vip_begin->getContentSize().width / 2, posbegin.y));
		vip_end->setPosition(Vec2(nameNode->getPosition().x + nameNode->getContentSize().width + vip_begin->getContentSize().width / 2, posbegin.y));
	}
}

void ChattingCell::inviteCallback(cocos2d::Ref* pSender,int uin)
{
	if(mText->getTag() == MSG_TYPE_NULL)
	{
		mImage->setTouchEnabled(false);
		sgChattingControl()->getChattingDialogPtr()->showTextTip(sgTextHelper(TEXT_CHAT_EXPIRED));
	}
	else
	{
		if (!sgEnvSet("MGAME_OPEN_PVP_LIMIT"))
		{
			//int openLevel = sgUserObj()->getMaxOpenCheckPoint();
			int openLevel = sgUserObj()->getMaxOpenGateID();
			if (openLevel < sgEnvSet("PVP_ROOM_LEVEL"))
			{
				sgChattingControl()->getChattingDialogPtr()->showTextTip(sgTextHelper(TEXT_ROOM_LEVEL));
				return;
			}
		}
		mInviteRoomId = mText->getTag();
		mServerNode = mText->getCallbackName();
		std::string content = StringUtils::format(sgTextHelper(TEXT_PVP_ROOM_ACCEPT_CONFIRM).c_str(), mText->getName().c_str());
		// change by jim, unify the tips ui
		//GeneralDialogOther::show(content,GeneralDialogOther::E_GENERAL_TYPE_PK_CONFIRM, CC_CALLBACK_1(ChattingCell::acceptInviteCallback, this));
		GenerlDialog::show(sgTextHelper(TEXT_COMM_TIPS), content, CC_CALLBACK_1(ChattingCell::acceptInviteCallback, this),false, true, E_GENERAL_TYPE_PK_CONFIRM);
	}
	
}

void ChattingCell::popCharaterInfo(Ref* pSender,int uin){
	sgUserObj()->setCurInfoUin(uin);
	sgUserObj()->setCurInfoType(0);
	sgPageFunction()->gotoFunction(MSG_FUC_INFO);
}

void ChattingCell::acceptInviteCallback(cocos2d::Ref* sender)
{
	//if (sgUserObj()->getPvpPower() < PVP_NEED_PSYCH)
	if (sgUserObj()->getPersonalInfo()->energy() < PVP_NEED_PSYCH)
	{
		PowerBuy* pPowerBuy = sgTableManager()->getPowerBuy(sgUserObj()->getPersonalInfo()->energybuycount());// sgUserObj()->getEnergyBuyCount());
		if (pPowerBuy)
		{
			GenerlDialog::show(sgTextHelper(TEXT_BUY_ITEM__JINGLI),
				__String::createWithFormat(sgTextHelper(TEXT_BUY_ITEM_JINGLI_SURE).c_str(), pPowerBuy->getEnergyNum())->getCString(),
				CC_CALLBACK_1(ChattingCell::onClickBuyEngerCallBack, this));
		}
	}
	else
	{
		GenerlDialog::hide();
		sgPvpRoomControl()->sendEnterRoomRequest(mInviteRoomId,mServerNode);
		sgChattingControl()->getChattingDialogPtr()->moveShow(false);
	}
		


}

void ChattingButton::changeChatMsg()
{
	if (sgChattingControl()->getChatWorldHty()->size() < 1) return;
	ChatInfo* _chatinfo= sgChattingControl()->getChatWorldHty()->back(); 
	if(!_chatinfo) return; 

	int type = _chatinfo->mContentType;
	std::string _str;
	if(type == ContentType_Emoji)
	{
		_str = StringUtils::format("[%s]\\a[%s]",_chatinfo->mName.c_str(),_chatinfo->mContent.c_str());
	}
	else
	{
		_str = StringUtils::format("[%s]%s",_chatinfo->mName.c_str(),_chatinfo->mContent.c_str());
	}

	if(!m_pLabel)
	{
		m_pLabel = StringLabel::create();	
		m_pLabel->setTextColor(Color3B(247,217,178));
		m_pLabel->setLineNum(2);
		
		m_pLabel->setPosition(ccp(85, 37)); 
		
#ifdef MGAME_PLATFORM_MPLUS
		m_pLabel->setTextSize(20);
		m_pLabel->setFontName(DEFAULT_FONT);
#else
		m_pLabel->setTextSize(22);
		m_pLabel->setFontName("fonts/font.TTF");
#endif
		
		m_pLabel->setAnchorPoint(ccp(0, 0.5));
		mImageBg->addChild(m_pLabel);
	}
	if(mShow)
	{
		m_pLabel->setContentSize(CCSize(215,0));
	}
	else
	{
		m_pLabel->setContentSize(CCSize(300,0));
	}
	PowerString ps; 
	ps.init(_str.c_str()); 
	m_pLabel->setPowerString(&ps);	
	mImageBg->setVisible(true);
}



void ChattingDialog::onMenuItemAction(std::string cName, cocos2d::Ref* sender)
{
	if (cName == "onClose")
	{
		moveShow(false);
	}
	else if (cName == "onSend")
	{	
		std::string msg = mTextField->getString();
		int len = msg.length();
		if(len && len <= TextInput_Count)
		{
//#ifdef MGAME_SHOW_FPS
			if(msg.compare("/61server") == 0)
			{
				msg = sgLoginProxy()->getGameServer();
				if(msg.length() < 1) msg = sgChannelApi()->getDirURL();
					
				if(std::string::npos != msg.find(":")){
					std::string _addrs = msg.substr(msg.length() - 8, msg.length());
					mTextField->setText(_addrs);
				}			
				return;
			}
//#endif
			//int openLevel = sgUserObj()->getMaxOpenCheckPoint();
			int openLevel = sgUserObj()->getMaxOpenGateID();
			if (openLevel < sgEnvSet("MAIN_CHAT_LEVEL"))
			{
				showTextTip(sgTextHelper(TEXT_CHAT_LEVEL));
				return;
			}
			
			if (sgUserObj()->getChatFreeCount() < 1)
			{
				int flag = sgUserObj()->getChatBuyTip();
				if (flag == 1)
				{
					ChattingTips::showChattingTips(CC_CALLBACK_1(ChattingDialog::callBack, this));
					return;
				}
				else//不弹出窗提示 扣除消耗
				{
					sgChattingControl()->getChattingProxy()->sendChattingRpst(MSG_TYPE_WORLD, mTextField->getString(),1);
					sendComplete();
					return;
				}
			}
			sgChattingControl()->getChattingProxy()->sendChattingRpst(MSG_TYPE_WORLD,msg);
			sendComplete();
		}
		else if(len == 0)
		{
			showTextTip(sgTextHelper(TEXT_CHAT_NULL));
		}
		else
		{
			showTextTip(sgTextHelper(TEXT_CHAT_LEN));
		}
	}
	else if(cName == "onEmoji")
	{
		Window* pPage = Window::Create("ExpressionPanel.csb");
		pPage->setShow(true);
	}
	else if(cName == "onChallenge")
	{
		setChannel(false);
	}
	else if(cName == "onAll")
	{
		setChannel(true);
	}
}

bool ChattingDialog::init()
{
	bool ret = false;
	do
	{
		mRoot = CSLoader::getInstance()->createNode("ui/ChattingDialog.csb");
		mRoot->setAnchorPoint(Vec2(0, 0));	
		CC_BREAK_IF(!mRoot);
		onLocateClickCallback(mRoot);
		this->addChild(mRoot);
		Size _size = Director::getInstance()->getWinSize();
		mRoot->setPosition(-_size.width*1.5,-_size.height/2);
		mTextField = dynamic_cast<ui::TextField*>(mRoot->getChildByName("TextField_1"));
		mTextField->addEventListenerTextField(this, textfieldeventselector(ChattingDialog::TextFieldCallback));

		mTextCount = dynamic_cast<ui::Text*>(mRoot->getChildByName("Text_Count"));
		mTextCount->setText(StringUtils::format("%d",TextInput_Count));

		mScrollView_1 = dynamic_cast<ui::ScrollView*>(mRoot->getChildByName("ScrollView_1"));
		mScrollView_2 = dynamic_cast<ui::ScrollView*>(mRoot->getChildByName("ScrollView_2"));

		mBtnAll = dynamic_cast<ui::Button*>(mRoot->getChildByName("Button_1"));
		mBtnChallage = dynamic_cast<ui::Button*>(mRoot->getChildByName("Button_2"));

		mTextLevelLimit = dynamic_cast<ui::Text*>(mRoot->getChildByName("text_levelLimit"));
		mTextLevelLimit->setText(StringUtils::format(sgTextHelper(TEXT_ROOM_LEVEL1).c_str(),sgTableManager()->getPlayerInfo()->getInviteLevel()));

		mTextFree = dynamic_cast<ui::Text*>(mRoot->getChildByName("Text_Free"));
		mTextGap = dynamic_cast<ui::Text*>(mRoot->getChildByName("Text_Gap"));
		mSpriteGold = dynamic_cast<Sprite*>(mRoot->getChildByName("Sprite_Gold"));

		ret = true;
	} while (0);


	return ret;
}

void ChattingDialog::TextFieldCallback(Ref* pSender, TextFiledEventType type)
{
	int count = mTextField->getString().length();
	if (type == TextFiledEventType::TEXTFIELD_EVENT_INSERT_TEXT || type == TextFiledEventType::TEXTFIELD_EVENT_DELETE_BACKWARD)
	{
		if (count >0)
		{
			mTextField->setOpacity(255.0);
		}
		else
		{
			mTextField->setOpacity(122.5);
		}
		if (count <= TextInput_Count)
		{
			mTextCount->setText(StringUtils::format("%d",TextInput_Count - count));
		}
	}
	//else if(type == TextFiledEventType::TEXTFIELD_EVENT_DETACH_WITH_IME && count > 0)
	//{
		//onMenuItemAction("onSend", NULL);
	//}
}

void ChattingDialog::onShow()
{
	sgChattingControl()->setChattingDialogPtr(this);
	moveShow(true);
	loadChatHistory();
	
	sgChattingControl()->calculateFreeCount();
	startUpdateTime();
}

void ChattingDialog::onClose()
{
	sgChattingControl()->setChattingDialogPtr(NULL);
	this->unschedule(schedule_selector(ChattingDialog::updateTime));
}

void ChattingDialog::moveShow(bool show)
{
	Size _size = Director::getInstance()->getWinSize();
	if (show)
	{		
		setChannel(true);
		mRoot->runAction(MoveTo::create(0.25,Vec2(-_size.width/2,-_size.height/2)));
	}
	else
	{
		Size _size = Director::getInstance()->getWinSize();
		mRoot->runAction(Sequence::create(MoveTo::create(0.25,Vec2(-_size.width,-_size.height/2)),
			CallFunc::create(CC_CALLBACK_0(ChattingDialog::moveCallBack, this)),nullptr));
	}

}

void ChattingDialog::setChannel(bool all)
{
	if(mBtnAll->isBright() != all) return;

	mScrollView_1->setTouchEnabled(all);
	mScrollView_2->setTouchEnabled(!all);
	mScrollView_1->setVisible(all);
	mScrollView_2->setVisible(!all);
	mBtnAll->setBright(!all);
	mBtnChallage->setBright(all);
	//if (mScrollView_2->isVisible() && sgUserObj()->getPlayerLevel() < sgTableManager()->getPlayerInfo()->getInviteLevel() )
	if (mScrollView_2->isVisible() && sgUserObj()->getPersonalInfo()->explevel() < sgTableManager()->getPlayerInfo()->getInviteLevel())
	{
		mTextLevelLimit->setVisible(true);
	}
	else
		mTextLevelLimit->setVisible(false);
}

void ChattingDialog::moveCallBack()
{
	this->setShow(false);
}

ChattingDialog::ChattingDialog() :mTextField(0),
	mScrollView_1(0),
	mScrollView_2(0),
	mTextCount(0)
{
}

ChattingDialog::~ChattingDialog()
{
}

void ChattingDialog::loadChatHistory()
{
	std::vector<ChatInfo*>* tmpWorldHty = sgChattingControl()->getChatWorldHty();
	if (tmpWorldHty->size()<1)
	{
		return;
	}
	mScrollView_1->removeAllChildren();
	mScrollView_2->removeAllChildren();
	std::vector<ChatInfo*>::iterator iter = tmpWorldHty->begin(); 
	std::vector<ChatInfo*>::iterator iterend = tmpWorldHty->end();

	for (; iter != tmpWorldHty->end(); iter++) {
		ChatInfo* minfo = *iter;
		Node* node = ChattingCell::create(minfo);
		mScrollView_1->addChild(node);
		if(minfo->mContentType == ContentType_Challenge)
		{
			node = ChattingCell::create(minfo);
			mScrollView_2->addChild(node);
		}
	}
	updateChating(mScrollView_1);
	updateChating(mScrollView_2);
}

void ChattingDialog::addMessage()
{
	ChatInfo* _chatinfo= sgChattingControl()->getChatWorldHty()->back(); 
	if(!_chatinfo) return; 
	
	Node* node = ChattingCell::create(_chatinfo);
	mScrollView_1->addChild(node);

	if(_chatinfo->mContentType == ContentType_Challenge)
	{
		node = ChattingCell::create(_chatinfo);
		mScrollView_2->addChild(node);
		updateChating(mScrollView_2);
	}

	updateChating(mScrollView_1);
}


void ChattingDialog::updateChating(ui::ScrollView* mScrollView)
{
	Size viewSize = mScrollView->getContentSize();
	int msgListHeight = 0;
	
	for(auto it :mScrollView->getChildren())
	{
		msgListHeight += it->getContentSize().height;
		it->setPositionY(msgListHeight);
	}

	if(msgListHeight < viewSize.height){
		msgListHeight = viewSize.height;
	}
	mScrollView->setInnerContainerSize(Size(mScrollView->getContentSize().width,msgListHeight));

	int startY = msgListHeight;
	for(auto it :mScrollView->getChildren())
	{
		it->setPositionY(startY);
		startY -= it->getContentSize().height;
	}

	mScrollView->jumpToBottom();
}

void ChattingDialog::callBack(cocos2d::Ref*)
{
	sgChattingControl()->getChattingProxy()->sendChattingRpst(MSG_TYPE_WORLD, mTextField->getString(),1);
	sendComplete();
	ChattingTips::hide();
}

void ChattingDialog::updateTime(float dt)
{
	int sendTime = sgTimeCalculator()->getTimeLeft(Chat_Text);
	if (sendTime <= 0)
	{
		if (sgUserObj()->getChatFreeCount() == (Msg_Free_Max - 1))
		{
			mTextGap->setVisible(false);
			this->unschedule(schedule_selector(ChattingDialog::updateTime));
		}
		else
		{
			sgTimeCalculator()->createTimeCalcultor(Chat_Text, Msg_Chat_Time);
		}
		//sgUserObj()->setChatFreeCount(sgUserObj()->getChatFreeCount() + 1);
		//sgUserObj()->setChatTimeLeft(time(NULL));
		mTextFree->setString(StringUtils::format(sgTextHelper(7010).c_str(), sgUserObj()->getChatFreeCount()));
		mSpriteGold->setVisible(false);
	}
	
	mTextGap->setString(StringUtils::format("%d:%02d", sendTime / 60, sendTime % 60));
}

void ChattingDialog::sendComplete()
{
	mTextField->setText("");
	mTextField->setOpacity(122.5);
	mTextCount->setText(StringUtils::format("%d", TextInput_Count));
	setChannel(true);
}

void ChattingDialog::startUpdateTime()
{
	if (sgUserObj()->getChatFreeCount() == Msg_Free_Max)
	{
		mSpriteGold->setVisible(false);
		mTextGap->setVisible(false);
		mTextFree->setString(StringUtils::format(sgTextHelper(TEXT_CHAT_FREE).c_str(), Msg_Free_Max));
		this->unschedule(schedule_selector(ChattingDialog::updateTime));
	}
	else
	{
		mTextGap->setVisible(true);
		if (!sgTimeCalculator()->hasKey(Chat_Text))
			sgTimeCalculator()->createTimeCalcultor(Chat_Text, Msg_Chat_Time);

		int sendTime = sgTimeCalculator()->getTimeLeft(Chat_Text);
		mTextGap->setString(StringUtils::format("%d:%02d", sendTime / 60, sendTime % 60));

		if (!this->isScheduled(schedule_selector(ChattingDialog::updateTime)))
			this->schedule(schedule_selector(ChattingDialog::updateTime), 1.0);

		if (sgUserObj()->getChatFreeCount() < 1)
		{
			mTextFree->setString(sgTextHelper(TEXT_CHAT_COST));
			mSpriteGold->setVisible(true);
		}
		else
		{
			mTextFree->setString(StringUtils::format(sgTextHelper(TEXT_CHAT_FREE).c_str(), sgUserObj()->getChatFreeCount()));
			mSpriteGold->setVisible(false);
		}
	}
}



void ChattingTips::onMenuItemAction(std::string cName, cocos2d::Ref* sender)
{
	if (cName == "onClose")
	{
		if (mRoot->getChildByName("Sprite_Yes")->isVisible())
		{
			sgUserObj()->saveToFile();
		}
		this->setShow(false);
	}
	else if (cName == "onChatTip")
	{
		int flag = sgUserObj()->getChatBuyTip();
		mRoot->getChildByName("Sprite_Yes")->setVisible(flag == 1? true : false);
		sgUserObj()->setChatBuyTip(flag == 1 ? 0 : 1);
		sgUserObj()->setChatTimeLeft(time(NULL));
	}
}

bool ChattingTips::init()
{
	bool ret = false;
	do
	{
		mRoot = CSLoader::getInstance()->createNode("ui/ChattingTips.csb");
		mRoot->setAnchorPoint(Vec2(0.5, 0.5));
		CC_BREAK_IF(!mRoot);
		this->addChild(mRoot);

		mSure = dynamic_cast<ui::Widget*>(mRoot->getChildByName("m_sure"));

		onLocateClickCallback(mRoot);

		ret = true;
	} while (0);


	return ret;
}

void ChattingTips::hide()
{
	ChattingTips* pRet = dynamic_cast<ChattingTips*>(sgPageFunction()->getWindowByPageName("ChattingTips.csb"));
	if (pRet)
	{
		pRet->setShow(false);
		//pRet->onLocateClickCallback(pRet->mRoot);
	}
}

ChattingTips::ChattingTips()
{
}

ChattingTips::~ChattingTips()
{

}

void ChattingTips::showChattingTips(const std::function<void(Ref*)>& callBack)
{
	ChattingTips* pRet = dynamic_cast<ChattingTips*>(Window::Create("ChattingTips.csb"));
	if (pRet)
	{
		pRet->onLocateClickCallback(pRet->mRoot);
		pRet->mSure->addClickEventListener(callBack);
		pRet->setShow(true);
		pRet->mRoot->getChildByName("Sprite_Yes")->setVisible(false);
	}
}

