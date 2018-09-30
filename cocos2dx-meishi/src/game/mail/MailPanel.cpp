#include "MailPanel.h"
#include "CVTime.h"
#include "data/TableManager.h"
#include "MailProxy.h"
#include "net/proto/SaveData.pb.h"
#include "ui/dialogs/GiftAwardDialog.h"
#include "core/MessageManager.h"

USING_NS_CC;
using namespace ui;
#define  LIST_ITEM_TAG 10
#define CELL_WIDTH 960
#define CELL_HEIGHT_OPEN 470
#define CELL_HEIGHT_CLOSE 122

MailPanel::MailPanel(void) :mNowJumpPercent(0), mIsCanAutoFresh(true)
{
}


MailPanel::~MailPanel(void)
{
	sgMailProxy()->setMailPanel(NULL);
}

bool MailPanel::init()
{
	if (!ActionWindow::init())
	{
		return false;
	}

	mRoot = CSLoader::createNode("ui/MailNode.csb");
	setAnimation("fla_fulixin");

	Button* pClose = dynamic_cast<Button*>(mRoot->getChildByName("BtnBack"));


	mContentNode = dynamic_cast<Node*>(mRoot->getChildByName("Node_7"));
	changeBone(mArmatureBg, "fulixin_qiandao", mContentNode);

	changeBone(mArmatureBg, "fulixin_guanbianniu", pClose);
	changeBone(mArmatureBg, "fulixin_di", mRoot->getChildByName("Sprite_1"));
	changeBone(mArmatureBg, "fulixin_gazi", mRoot->getChildByName("Sprite_2"));
	changeBone(mArmatureBg, "fulixin_daiziaia", mRoot->getChildByName("Sprite_3"));

	mMailListView = dynamic_cast<ListView*>(mContentNode->getChildByName("ListView_mail"));
	mMailListView->setSwallowTouches(false);
	mMailListView->setItemsMargin(0);
	{
		mMailListView->addEventListenerScrollView(this, scrollvieweventselector(MailPanel::onScrollviewEvent));
	}

	mTitleTxt = dynamic_cast<Text*>(mContentNode->getChildByName("Text_1"));
	mContentTxt = dynamic_cast<Text*>(mContentNode->getChildByName("Text_1_0"));
	mOpenBtn = dynamic_cast<Button*>(mContentNode->getChildByName("Button_1"));
	{
		mStringLabel = StringLabel::create();
		mStringLabel->setContentSize(mContentTxt->getContentSize());
		mStringLabel->setTextSize(mContentTxt->getFontSize());
		
		const Color4B ltextColor4b = mContentTxt->getTextColor();
		mStringLabel->setTextColor(ccc3(ltextColor4b.r, ltextColor4b.g, ltextColor4b.b));
		mStringLabel->setAnchorPoint(Vec2(0, 1));
		mStringLabel->setPosition(Vec2(-36, 135));
		mStringLabel->setFontName(mContentTxt->getFontName());
		//mStringLabel->setSingleLine(false);
		mStringLabel->setLineNum(7);
		mContentNode->addChild(mStringLabel);
		mContentTxt->setString("");
	}

	for (int i = 1; i <= 4; i++)
	{
		Node* lProjectNode = dynamic_cast<Node*>(mContentNode->getChildByName(__String::createWithFormat("ProjectNode_%d", i)->getCString()));
		mAwardNodeVec.push_back(lProjectNode);
	}
	Button* lflag1 = dynamic_cast<Button*>(mContentNode->getChildByName("downBtn"));
	Button* lflag2 = dynamic_cast<Button*>(mContentNode->getChildByName("upBtn"));
	mFlagImageVec.push_back(lflag1);
	mFlagImageVec.push_back(lflag2);

	addChild(mRoot);

	setPopType(E_POP_TYPE_NONE);
	onLocateClickCallback(mRoot);
	sgMailProxy()->setMailPanel(this);
	return true;
}

void MailPanel::onItemTouch(Ref*, Widget::TouchEventType)
{
	mIsCanAutoFresh = true;
}

void MailPanel::onScrollviewEvent(Ref* ref, cocos2d::ui::ScrollviewEventType  sType)
{
	if (!(sType == SCROLLVIEW_EVENT_SCROLLING || sType == SCROLLVIEW_EVENT_BOUNCE_TOP || 
		sType == SCROLLVIEW_EVENT_BOUNCE_BOTTOM))
	{
		return;
	}
	

	if (mMailListView&&mMailListView->getItems().size()>0)
	{
		bool lisHasHind = false;
		Vector<Widget*> lwidgets = mMailListView->getItems();
		for (int i = 0; i<lwidgets.size() - 1; i++)
		{
			Point lpt = PointApplyAffineTransform(lwidgets.at(i)->getPosition(), mMailListView->getItem(0)->getNodeToWorldAffineTransform());
			//CCLOG("[trace] MailPanel::updateUI() i:%d, posy:%f,  mMailListView_height:%f-------------",i,lpt.y,mMailListView->getInnerContainer()->getContentSize().height);
			if (lpt.y <= 228 + mMailListView->getInnerContainer()->getContentSize().height)
			{
				lwidgets.at(i)->setVisible(false);
				lisHasHind = true;
			}
			else
				lwidgets.at(i)->setVisible(true);
		}
		if (mIsCanAutoFresh || sType == SCROLLVIEW_EVENT_BOUNCE_TOP || sType == SCROLLVIEW_EVENT_BOUNCE_BOTTOM)
		{
			mNowJumpPercent = (((mMailListView->getInnerContainer()->getPositionY()) +
				mMailListView->getInnerContainer()->getContentSize().height) /
				mMailListView->getInnerContainer()->getContentSize().height) * 100;
			CCLOG("mNowJumpPercent =%d", mNowJumpPercent);
		}

	}
}

void MailPanel::onMenuItemAction(std::string name, cocos2d::Ref*)
{
	if (name == "onBack")
	{
		setShow(false);
		sgMessageManager()->sendMessage(EMT_SELECTWORD_SCHEDULE, "");
	}
	else if (name == "onGet")
	{
		
		int ltag = mOpenBtn->getTag();
		std::vector<int> ltagVec;
		ltagVec.push_back(ltag);
		sgMailProxy()->sendDelMail(ltagVec);
	}
	else if (name == "onUp")
	{
		mIsCanAutoFresh = false;
		float lHeight = mMailListView->getInnerContainer()->getContentSize().height;
		float lMoveHeight = 250.0;
		CCLOG("lHeight =%f ", lHeight);
		int lprent = lMoveHeight / lHeight * 100;
		
		mNowJumpPercent -= lprent;
		mMailListView->scrollToPercentVertical(mNowJumpPercent, 0.25, true);
	}
	else if (name == "onDown")
	{
		mIsCanAutoFresh = false;
		float lHeight = mMailListView->getInnerContainer()->getContentSize().height;
		int lItemSize = mMailListView->getItems().size();
		float lMoveHeight = 250.0;
		int lprent = lMoveHeight / lHeight * 100;
		mNowJumpPercent += lprent;
		mMailListView->scrollToPercentVertical(mNowJumpPercent, 0.25, true);
	}
}

void MailPanel::onShow()
{
	ActionWindow::onShow();

	if (sgMailData()->getMailSize() <= 0)
	{
		mContentNode->setVisible(false);
	}

	sgMailProxy()->sendGetMail(sgMailData()->getMailNo());
}

void MailPanel::updateInfo()
{
	updateUI();
}

void MailPanel::updateUI()
{
	if (sgMailData()->getMailSize()>0)
	{
		mContentNode->setVisible(true);
		sgMessageManager()->sendMessage(EMT_HONGDIAN_TIP, StringUtils::format("%d 1", MSG_FUC_MAIL));
		sgMailData()->setNewMail(true);

		int pScrollToItem = 0;

		int pbeginSize = mMailListView->getItems().size();
		if (pbeginSize>1)
		{
			mMailListView->removeItem(pbeginSize - 1);
			pbeginSize--;
		}
		for (; pbeginSize<sgMailData()->getMailSize(); pbeginSize++)
		{
			const fgame::DataMailInfo& pInfo = sgMailData()->getMailInfo(pbeginSize);
			{
				Node* pNode1 = CSLoader::getInstance()->createNode("ui/MailCell.csb");
				Widget* pWidget0 = dynamic_cast<Widget*>(pNode1->getChildByName("Image_2"));
				Widget* pWidget = dynamic_cast<Widget*>(pWidget0->getChildByName("Image_1"));
				pWidget->setSwallowTouches(true);
				if (pWidget0)
				{
					pWidget0->removeFromParentAndCleanup(false);
				}
				mMailListView->pushBackCustomItem(pWidget0);

				pWidget->setTag(pInfo.id());
				pWidget->addTouchEventListener(std::bind(&MailPanel::onItemTouch, this,
					std::placeholders::_1, std::placeholders::_2));
				pWidget->addClickEventListener(CC_CALLBACK_1(MailPanel::onItemClickCallBack, this));
				float px = pWidget->getPositionX() + rand() % 50;
				float py = -40;
				if (pbeginSize>0)
				{
					//py = -;//rand()%18;
				}
				pWidget->setPosition(Vec2(px, py));
				CCLOG("[trace] MailPanel::updateUI() mailno:%d,posy:%f", pbeginSize, pWidget0->getPositionY());
				if (pWidget0->getPositionY()>360)
				{
					pWidget0->setVisible(false);
				}

				dynamic_cast<ui::Text*>(pWidget->getChildByName("title_txt"))->setString(pInfo.title());

				if (mDelIdVec.size() == 1 && mDelIdVec[0] == pInfo.id())
				{
					pScrollToItem = pbeginSize;
				}
			}
		}
		{
			Node* pNode1 = CSLoader::getInstance()->createNode("ui/MailCell.csb");
			Widget* pWidget0 = dynamic_cast<Widget*>(pNode1->getChildByName("Image_2"));
			Widget* pWidget = dynamic_cast<Widget*>(pWidget0->getChildByName("Image_1"));
			if (pWidget)
			{
				pWidget->removeFromParentAndCleanup(false);
			}
			mMailListView->pushBackCustomItem(pWidget);//空的占位
			pWidget->setVisible(false);
			//pWidget->setVisible(false);
		}
		scheduleOnce(schedule_selector(MailPanel::scheduleUpdateListView), 1.0);
		const fgame::DataMailInfo& pInfo = sgMailData()->getMailInfo(--pbeginSize);
		updateConten(pInfo);

		if (mMailListView->getItems().size() <= 5)
		{
			mFlagImageVec[0]->setVisible(false);
			mFlagImageVec[1]->setVisible(false);
			mFlagImageVec[0]->setTouchEnabled(false);
			mFlagImageVec[1]->setTouchEnabled(false);
		}
		else
		{
			mFlagImageVec[0]->setVisible(true);
			mFlagImageVec[1]->setVisible(true);
			mFlagImageVec[0]->setTouchEnabled(true);
			mFlagImageVec[1]->setTouchEnabled(true);
		}
		mDelIdVec.clear();
	}
	else
	{
		sgMailData()->setNewMail(false);
		sgMessageManager()->sendMessage(EMT_HONGDIAN_TIP, StringUtils::format("%d 0", MSG_FUC_MAIL));
		setShow(false);
	}
}

void MailPanel::delCell(std::vector<int> delMailStateVec)
{
	if (mAwardMap.size() > 0)
	{
		std::vector<int> pdelMailIDVec;
		for (int i = 0; i< delMailStateVec.size(); i++)
		{
			if (delMailStateVec[i] != 0)
			{
				//问题邮件
				auto pdelit = mAwardMap.begin();
				int pdelid = i;
				while (pdelid >= 0 && pdelit != mAwardMap.end())
				{
					pdelit++;
					pdelid--;
				}
				if (pdelit != mAwardMap.end())
					pdelMailIDVec.push_back(pdelit->first);
			}
		}
		if (pdelMailIDVec.size()>0)
		{
			for (auto it = pdelMailIDVec.begin(); it != pdelMailIDVec.end(); it++)
			{
				mAwardMap.erase(mAwardMap.find(*it));
			}
		}
	}

	//获得物品表现
	GiftAwardDialog::showDialogByAward(mAwardMap);
	mAwardMap.clear();//领取完清除
	//表现
	//重新load
	mMailListView->removeAllItems();
	mMailListView->removeAllChildren();
	updateInfo();
}
//#define MAIL_CELL_ZORDER 999
void MailPanel::onItemClickCallBack(Ref* ref)
{
	Widget* pWidget = dynamic_cast<Widget*>(ref);
	int ltag = pWidget->getTag();
	const fgame::DataMailInfo* pInfo = sgMailData()->getMailInfoById(ltag);
	updateConten(*pInfo);
	//pWidget->getParent()->setZOrder(MAIL_CELL_ZORDER);
}

void MailPanel::updateConten(const fgame::DataMailInfo& info)
{
	mTitleTxt->setString(info.title());
	//mContentTxt->setString(info.content());
	PowerString ps;
	ps.init(info.content().c_str());
	mStringLabel->setPowerString(&ps);
	const Color4B ltextColor4b = mContentTxt->getTextColor();
	mStringLabel->setTextColor(ccc3(ltextColor4b.r, ltextColor4b.g, ltextColor4b.b));

	mOpenBtn->setTag(info.id());

	mAwardMap.clear();
	mAwardMap = sgMailData()->getAward(info.plugin());

	int itemIndex = 1;


	for (std::map<std::string, int>::iterator iter_realProp = sgMailData()->mRealPorp.begin(); iter_realProp != sgMailData()->mRealPorp.end(); iter_realProp++)
	{
		Node* pNode = mAwardNodeVec[itemIndex - 1];
		if (itemIndex > mAwardMap.size())
		{
			pNode->setVisible(false);
		}
		else
		{
			auto pIcon = dynamic_cast<ImageView*>(pNode->getChildByName("Image_2"));
			auto pNumb = dynamic_cast<Text*>(pNode->getChildByName("Text_1"));

			if ("itemqb.png" == iter_realProp->first)
			{
				pIcon->loadTexture("ui/Icon/itemqb.png", TextureResType::PLIST);
				pNumb->setString(StringUtils::format("%d", iter_realProp->second));

				std::string  frameBg = "ui/general/general_kuang_4.png";
				if (pNode->getChildByName("Image_1"))
					dynamic_cast<ui::ImageView*>(pNode->getChildByName("Image_1"))->loadTexture(frameBg, TextureResType::PLIST);
			}
		}

		itemIndex++;
	}

	auto pAwardIt = mAwardMap.begin();
	for (int i = itemIndex; i <= 4; i++)
	{
		Node* pNode = mAwardNodeVec[i - 1];
		if (i > mAwardMap.size() + (itemIndex - 1))
		{
			pNode->setVisible(false);
		}
		else
		{
			pNode->setVisible(true);
			auto pIcon = dynamic_cast<ImageView*>(pNode->getChildByName("Image_2"));
			auto pNumb = dynamic_cast<Text*>(pNode->getChildByName("Text_1"));

			int lgoodsType = sgMailData()->getGoodsType(pAwardIt->first);
			const GoodsInfo* lgoodsInfo = sgTableManager()->getGoodsInfo(lgoodsType, pAwardIt->first);
			if (!lgoodsInfo)
			{
				continue;
			}
			std::string pIconStr = lgoodsInfo->mIconSmall;

			pIcon->loadTexture(pIconStr, TextureResType::PLIST);
			pNumb->setString(StringUtils::format("%d", pAwardIt->second));


			const PropInfo*  pProp = sgTableManager()->getPropInfoById(pAwardIt->first);
			if (pProp)
			{
				std::string frameBg = "ui/general/general_kuang_3.png";
				if (pProp->isPve == 1)
					frameBg = "ui/general/general_kuang_4.png";
				if (pNode->getChildByName("Image_1"))
					dynamic_cast<ui::ImageView*>(pNode->getChildByName("Image_1"))->loadTexture(frameBg, TextureResType::PLIST);
			}

			pAwardIt++;
		}
	}


	sgMailData()->mRealPorp.clear();
}

void MailPanel::scheduleUpdateListView(float dt)
{
	mMailListView->refreshView();
}
