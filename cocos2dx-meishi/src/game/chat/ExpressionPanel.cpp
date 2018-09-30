#include "ExpressionPanel.h"
#include "SoundManager.h"
#include "ChattingControl.h"
#include "net/UserObj.h"
#include "data/TableManager.h"
#include "core/TimeCalculator.h"
#include "ui/dialogs/GenerlDialog.h"
#include "game/utils/Comm.h"

void ExpressionPanel::onMenuItemAction(std::string cName, cocos2d::Ref* sender)
{
	if (cName == "onClose")
	{
		this->setShow(false);
	}
	else
	{		
		//int openLevel = sgUserObj()->getMaxOpenCheckPoint();
		int openLevel = sgUserObj()->getMaxOpenGateID();
		if (openLevel < sgEnvSet("MAIN_CHAT_LEVEL"))
		{
			this->setShow(false);
			TipsManager::showTextTip(sgTextHelper(TEXT_CHAT_LEVEL));
			return;
		}		
		mExpressionID = (dynamic_cast<Node*>(sender))->getTag();
		if (sgUserObj()->getChatFreeCount() < 1)
		{
			int flag = sgUserObj()->getChatBuyTip();
			if (flag == 1)
			{
				ChattingTips::showChattingTips(CC_CALLBACK_1(ExpressionPanel::callBack, this));
				return;
			}	
			else//不弹出窗提示 扣除消耗
			{
				this->setShow(false);
				sgChattingControl()->setExpression(mExpressionID,1);
				return;
			}
		}
		this->setShow(false);
		sgChattingControl()->setExpression(mExpressionID);
	}
}

ExpressionPanel::ExpressionPanel()
{
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("emojiPlist.plist");
}

ExpressionPanel::~ExpressionPanel()
{
	SpriteFrameCache::getInstance()->removeSpriteFramesFromFile("emojiPlist.plist");
}

bool ExpressionPanel::init()
{
	bool ret = false;
	do
	{
		mRoot = CSLoader::getInstance()->createNode("ui/ExpressionPanel.csb");
		mRoot->setAnchorPoint(Vec2(0.5, 0.5));
		CC_BREAK_IF(!mRoot);
		
		this->addChild(mRoot);
		Size _size = Director::getInstance()->getWinSize();
		mRoot->setPosition(-_size.width/2,-_size.height/2);

		mNode_1 = mRoot->getChildByName("Image_1")->getChildByName("Node_1");

		onCreateExpression();

		onLocateClickCallback(mRoot);

		ret = true;
	} while (0);


	return ret;
}


void ExpressionPanel::onCreateExpression()
{
	int tColCount = 7;
	int tExpressionCount = 20;

	for(int i = 0; i < tExpressionCount; i++)
	{
		__String* tS1 = __String::createWithFormat("emoji_(%d).png",i+1);
		int tCurRow = i / tColCount;
		int tCurCol = i % tColCount;
		Button* pBtn = Button::create();
		mNode_1->addChild(pBtn);  

		pBtn->loadTextures(tS1->getCString(),tS1->getCString(),"",TextureResType::PLIST);
		pBtn->setPosition(ccp(tCurCol*125,-tCurRow*110));
		__String* tS2 = __String::createWithFormat("/%d#",i+1);
		pBtn->setCallbackName(tS2->getCString());
		pBtn->setCallbackType("Click");
		pBtn->setTag(i+1);
	}
}

void ExpressionPanel::callBack(cocos2d::Ref*)
{
	sgChattingControl()->setExpression(mExpressionID,1);
	ChattingTips::hide();
	this->setShow(false);
}
