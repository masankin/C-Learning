#include "MsgPanel.h"
#include "cocostudio/ActionTimeline/CSLoader.h"

#include "EnvSet.h"
#include "game/net/UserObj.h"
#include "data/TableManager.h"
#include "core/SoundManager.h"

MsgPanel::MsgPanel()
{
}

MsgPanel::~MsgPanel()
{
}

void MsgPanel::onTouchEnded(Touch *touch, Event *event)
{

	Vec2 touchLocation = touch->getLocation(); // Get the touch position
	touchLocation = mImageBg->getParent()->convertToNodeSpace(touchLocation);
	Rect bBox = mImageBg->getBoundingBox();
	if(!bBox.containsPoint(touchLocation))
	{
		this->setVisible(false);
	}
	
}

MsgPanel* MsgPanel::create(int chatType )
{
	MsgPanel* pret = new MsgPanel();
	if (pret && pret->init(chatType))
	{
		return pret;
	}
	CC_SAFE_DELETE(pret);
	return NULL;
}

bool MsgPanel::init(int chatType)
{
	mChatType = chatType;

	Node* mRoot = CSLoader::getInstance()->createNode("MsgPanel.csb");
	if (!mRoot)
	{
		CCLOG("[error] ArrowWidget::initUI load csb error!");
		return false;
	}
	this->addChild(mRoot);
	//setTouchEnabled(true);
	//auto listen = EventListenerTouchOneByOne::create();  
	//listen->onTouchEnded = CC_CALLBACK_2(MsgPanel::onTouchEnded,this);  
	//listen->onTouchBegan = CC_CALLBACK_2(MsgPanel::onTouchBegan, this);
	//listen->onTouchMoved = CC_CALLBACK_2(MsgPanel::onTouchMoved, this);
	//_eventDispatcher->addEventListenerWithSceneGraphPriority(listen,this); 
	//listen->setSwallowTouches(true);

	mRoot->setAnchorPoint(Vec2(0, 0));
	
	Node* mNode = nullptr;
	if (mChatType == Room_Chat)
	{
		mNode = mRoot->getChildByName("Node_2");
		Node* mNode1 = mRoot->getChildByName("Node_1");
		mNode1->setVisible(false);
	}
	else
	{
		mNode = mRoot->getChildByName("Node_1");
		//Node *mNode2 = mRoot->getChildByName("Node_2");
		//mNode2->setVisible(false);
	}
	mNode->setVisible(true);
	mImageBg = dynamic_cast<ui::ImageView*>(mNode->getChildByName("Image_Bg"));

	std::string sText[4];
	int iId[4];

	if (mChatType == Room_Chat)
	{
		std::vector<PvpChat*>* mData = sgTableManager()->getChatType(Pvp_Chat);
		
		for (int i = 0; i < mData->size(); i++)
		{
			sText[i] = mData->at(i)->mText;
			iId[i] = mData->at(i)->mID;
		}
	}
	else
	{
		int i = 0;
		auto itor = sgTableManager()->getPvpChatMap()->begin();
		for (;itor != sgTableManager()->getPvpChatMap()->end();itor++)
		{
			if(itor->second.at(0)->mType == Pvp_Chat) continue;
			sText[i] = itor->second.at(0)->mTitle;
			iId[i] = itor->second.at(0)->mType;
			i++;
		}
	}


	for (int i = 0; i < 4; i++)
	{
		mMsgText[i] = dynamic_cast<ui::Text*>(mNode->getChildByName(StringUtils::format("Type_%d",i+1)));
		mMsgText[i]->setText(sText[i]);
		ui::Button* mImage = dynamic_cast<ui::Button*>(mNode->getChildByName(StringUtils::format("Button_%d",i+1)));
		mImage->addClickEventListener(CC_CALLBACK_1(MsgPanel::onMenuClicked, this,iId[i]));
	}

	return true;
}



void MsgPanel::onMenuClicked(cocos2d::Ref* _ref,int id)
{
	if (mChatType == Fight_Chat)
	{
		std::vector<PvpChat*>* mData = sgTableManager()->getChatType(id);

		int index = rand()%(mData->size());
		id = mData->at(index)->mID;
	}

	sgChattingControl()->getChattingProxy()->sendChattingRpst(MSG_TYPE_PVP,"",0,id);
	this->setVisible(false);
}
