#include "MoraleView.h"
#include "MessageManager.h"
#include "BattleControl.h"
#include "chat/ChattingControl.h"
#include "pvp/PvpControl.h"
#include "game/utils/Comm.h"
#include "Build.h"
#include "BattleView.h"
#include "core/ResManager.h"
#include "data/TableManager.h"
#include "utils/HeadIconUtils.h"
USING_NS_CC;
using namespace ui;

#define  MORALE_PROGRESS										100
#define  MORRALE_TOP_VALUE_BEGAIN						50
#define  MORRALE_BOTTOM_VALUE_BEGAIN               50+25.0
#define MORRALE_TOP_VALUE_MAX							 50 +25
#define  MORRALE_BOTTOM_VALUE_MAX                    100
#define MORRALE_TOP_VALUE_MID								 MORRALE_TOP_VALUE_BEGAIN+25*0.8
#define  MORRALE_BOTTOM_VALUE_MID					MORRALE_BOTTOM_VALUE_BEGAIN + 25*0.8


MoraleView::MoraleView(eCampType pType, MORALE_POSITION pPosition) :mRoot(NULL), mSprSession(NULL)
{
	mCampType = pType;
	mPositionType = pPosition;
	sgMessageManager()->regisiterMessageHandler(EMT_MORALESHOW,this);
	sgMessageManager()->regisiterMessageHandler(EMT_CHAT_MSG, this);
}


MoraleView::~MoraleView(void)
{
	sgMessageManager()->removeMessageHandler(this);
	Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(this);
	CC_SAFE_RELEASE(mGlyphInfoNode);
}

bool MoraleView::init()
{
	if (!Node::init())
	{
		return false;
	}

	
	return true;
}

void MoraleView::initWithCampType(Node* pNode, Node* glyInfoNode)
{
	Node *pParentLayout = pNode->getParent();

	Point pos = pNode->getPosition(); // 保存位置
	Point anPos = pNode->getAnchorPoint();
	int nZOrder = pNode->getLocalZOrder(); // 保存Z序
	
	mRoot = pNode;
	mGlyphInfoNode = glyInfoNode;
	CC_SAFE_RETAIN(mGlyphInfoNode);
	mAttackTxt = dynamic_cast<ui::Text*>(mGlyphInfoNode->getChildByName("Node_attack")->getChildByName("textlv"));
	mSpeedTxt = dynamic_cast<ui::Text*>(mGlyphInfoNode->getChildByName("Node_speed")->getChildByName("textlv"));
	mDefTxt = dynamic_cast<ui::Text*>(mGlyphInfoNode->getChildByName("Node_defend")->getChildByName("textlv"));
	mPPTxt = dynamic_cast<ui::Text*>(mGlyphInfoNode->getChildByName("Node_troops")->getChildByName("textlv"));
	mArTxt = dynamic_cast<ui::Text*>(mGlyphInfoNode->getChildByName("Node_morale")->getChildByName("textlv"));
	mTotalTxt = dynamic_cast<ui::Text*>(mGlyphInfoNode->getChildByName("Text_16"));
	mSprSession = dynamic_cast<Sprite*>(pNode->getChildByName("Sprite_Session"));
	mSprSession->setLocalZOrder(2);

	mRoot->setVisible(true);
	setPosition(pos);
	setLocalZOrder(nZOrder);
	setAnchorPoint(anPos);
	pNode->addChild(this,1,MORALEVIEW_TAG);
	initSelf();
	

	
}

void MoraleView::changeCampType(eCampType type)
{
	this->mCampType = type;
	changeIco();
	changeName();
}

MoraleView* MoraleView::create(eCampType pType, MORALE_POSITION pPosition)
{
	MoraleView* pMorale  = new MoraleView(pType, pPosition);
	if (pMorale && pMorale->init())
	{
		pMorale->autorelease();

		return pMorale;
	}

	CC_SAFE_DELETE(pMorale);

	return NULL;
}

void MoraleView::changeIco()
{
	std::string pString ="";

	//if (!sgBattleControl()->getIsPvp())
	{
		switch (mCampType)
		{
		case ECAMP_TYPE_Player:
		case ECAMP_TYPE_Player1:
			mCampType1 = ECAMP_TYPE_Player;
			mCampType2 = ECAMP_TYPE_Player1;
			pString = "ui/Icon/Icon_bz.png";
			break;
		case ECAMP_TYPE_AI:
		case ECAMP_TYPE_Player2:
			mCampType1 = ECAMP_TYPE_AI;
			mCampType2 = ECAMP_TYPE_Player2;
			pString = "ui/Icon/Icon_zz.png";
			break;
		case ECAMP_TYPE_AI2:
		case ECAMP_TYPE_Player3:
			mCampType1 = ECAMP_TYPE_AI2;
			mCampType2 = ECAMP_TYPE_Player3;
			pString = "ui/Icon/Icon_hbb.png";
			break;
		case ECAMP_TYPE_AI3:
		case ECAMP_TYPE_Player4:
			mCampType1 = ECAMP_TYPE_AI3;
			mCampType2 = ECAMP_TYPE_Player4;
			pString = "ui/Icon/Icon_cake.png";
			break;
		default:
			break;
		}
	}
	mIcon = dynamic_cast<ui::ImageView*>(mRoot->getChildByName("Image_15"));
	mIcon->setScaleX(0.8);
	mIcon->setScaleY(0.8);
	if (sgBattleControl()->getIsPvp())
	{
		mIcon->setTouchEnabled(true);
		mIcon->setSwallowTouches(true);
		mIcon->addTouchEventListener(this,toucheventselector(MoraleView::onHeadTouchEvent));
	}

	mSprSession->setVisible(false);
	if (sgBattleControl()->getIsPvp() || mCampType == sgBattleControl()->getOwnCamp())
	{
		std::string iconId = sgUserObj()->getPersonalInfo()->iconid();
		if (sgBattleControl()->getIsPvp())
		{
			iconId = sgPvpControl()->getPvpPlayerIconId(mCampType);
		}
		sgHeadIconUtils()->changeIcon(mIcon, iconId);
		pString = "";
		
		if (sgBattleControl()->getIsPvp())
		{
			if (sgUserObj()->getIsGoneNewerProtect())
			{
				int point = sgPvpControl()->getPlayerPoint(mCampType);
				const GradeData* gradeData = sgTableManager()->getGradeData(point);
				if (gradeData && mSprSession)
				{
					mSprSession->setSpriteFrame(StringUtils::format("ui/Icon2/ranking_Emblem-%02d.png", gradeData->getID()));
					mSprSession->setVisible(true);
				}
			}
		}
	}else if (!pString.empty())
	{
		mIcon->loadTexture(pString, ui::TextureResType::PLIST);
	}
#ifdef MGAME_PLATFORM_MPLUS
	else if (mCampType == sgBattleControl()->getOwnCamp())
	{
		ui::ImageView* pIcon = dynamic_cast<ui::ImageView*>(mRoot->getChildByName("Image_15"));
		//std::string pString = FriendSystem::GetPlayerIconNameFromId(sgUserObj()->getPlayerIconID(), sgMplusControl()->getThirdPartyIcon());
		std::string pString = FriendSystem::GetPlayerIconNameFromId(sgUserObj()->getPersonalInfo()->iconid(), sgMplusControl()->getThirdPartyIcon());
		pIcon->loadTexture(pString, ui::TextureResType::LOCAL);
		pIcon->setScaleX(0.8);
		pIcon->setScaleY(0.8);
	}
#endif
	ui::Text* pName = dynamic_cast<ui::Text*> (mRoot->getChildByName("mName"));

	switch (mPositionType)
	{
	case M_P_1:
		{
			mGlyphInfoVec = Vec2(mRoot->getPositionX()+50, mRoot->getPositionY()-40);
			mProgress->setPercentage(MORRALE_TOP_VALUE_BEGAIN);
		}
		break;
	case M_P_2:
		{
			mGlyphInfoVec = Vec2(mRoot->getPositionX()-50-400, mRoot->getPositionY()-40);
			mProgress->setPercentage(MORRALE_TOP_VALUE_BEGAIN);
			pName->setScaleX(-1);
		}
		break;
	case M_P_3:
		{
			mGlyphInfoVec = Vec2(mRoot->getPositionX()+50, mRoot->getPositionY()+40+160);
			mProgress->setPercentage(MORRALE_BOTTOM_VALUE_BEGAIN);
		}
		break;
	case M_P_4:
		{
			mGlyphInfoVec = Vec2(mRoot->getPositionX()-50-400, mRoot->getPositionY()+40+160);
			mProgress->setPercentage(MORRALE_BOTTOM_VALUE_BEGAIN);
			pName->setScaleX(-1);
		}
		break;
	default:
		break;
	}
}

void MoraleView::changeIconBg(MORALE_POSITION ePosition, bool bIsSelf)
{
	int camp = mCampType;
	if (sgBattleControl()->getIsPvp())
	{
		camp = camp - 4;
	}
	else
	{
		camp = mCampType == sgBattleControl()->getOwnCamp() ? 2 : 1;
	}

	int pos = ePosition > 2 ? 1 : 0;
	std::string strIcon = StringUtils::format("ui/battle/Battle_bg_%d_%d.png", camp, pos);
	SpriteFrame* pframe = SpriteFrameCache::getInstance()->getSpriteFrameByName(strIcon);

	if (pframe)
	{
		(dynamic_cast<Sprite*>(mRoot->getChildByName("Sprite_1")))->setDisplayFrame(pframe);
	}
}

void MoraleView::initSelf()
{
	mAniState = E_MORALEANI_IDLE;
	SpriteFrame* pframe1  = NULL;//= SpriteFrameCache::getInstance()->getSpriteFrameByName("ui/battle/Battle_bg_92.png");

	switch (mPositionType)
	{
	case M_P_1:
		{
			mProgress = ProgressTimer::create(Sprite::create("ui/battle/ui_battle_Battle_jindut.png"));
			mProgress->setReverseProgress(true);
			mRoot->addChild(mProgress,1,MORALE_PROGRESS);
			mProgress->setPosition(Vec2(8,-5));
			mProgress->setScale(.8f);
			mProgress->setPercentage(MORRALE_TOP_VALUE_BEGAIN);
			
			//左上角
			mMoraleArm = sgResManager()->getAramatur("fla_benergyA"); //cocostudio::Armature::create("fla_benergyA");
			mMoraleArm->setScaleX(-0.75);
			mMoraleArm->setScaleY(0.75); 
			mMoraleArm->setPosition(mMoraleArm->getPosition());
			
		}

		break;
	case M_P_2:
		{
			//pframe1 = SpriteFrameCache::getInstance()->getSpriteFrameByName("ui/battle/Battle_bg_92.png");
			mProgress = ProgressTimer::create(Sprite::create("ui/battle/ui_battle_Battle_jindut.png"));
			mProgress->setPosition(Vec2(8, -5));
			mProgress->setScale(.8f);
			mProgress->setReverseProgress(true);
			mRoot->addChild(mProgress,1,MORALE_PROGRESS);
			mProgress->setPercentage(MORRALE_TOP_VALUE_BEGAIN);
			
			//右上角
			mMoraleArm = sgResManager()->getAramatur("fla_benergyA"); //cocostudio::Armature::create("fla_benergyA"); 
			mMoraleArm->setScaleX(-0.75);
			mMoraleArm->setScaleY(0.75); 
			mMoraleArm->setPosition(mMoraleArm->getPosition() +  ccp(1,0)); 
		}

		break;
	case M_P_3:
		{
			//pframe1 = SpriteFrameCache::getInstance()->getSpriteFrameByName("ui/battle/Battle_bg_102.png");
			mProgress = ProgressTimer::create(Sprite::create("ui/battle/ui_battle_Battle_ju.png"));
			mProgress->setReverseProgress(true);
			mProgress->setPosition(Vec2(5.09, 8.90));
			mProgress->setScale(.8f);
			mRoot->addChild(mProgress,1,MORALE_PROGRESS);
			mProgress->setPercentage(MORRALE_BOTTOM_VALUE_BEGAIN);

			//左下角
			mMoraleArm = cocostudio::Armature::create("fla_benergyB");
			mMoraleArm->setScaleX(-0.75f); 
			mMoraleArm->setScaleY(0.75f);
		}

		break;
	case M_P_4:
		{
			//pframe1 = SpriteFrameCache::getInstance()->getSpriteFrameByName("ui/battle/Battle_bg_102.png");
			mProgress = ProgressTimer::create(Sprite::create("ui/battle/ui_battle_Battle_ju.png"));
			mProgress->setReverseProgress(true);
			mProgress->setPosition(Vec2(5.09, 8.90));
			mProgress->setScale(.8f);
			mRoot->addChild(mProgress,1,MORALE_PROGRESS);
			mProgress->setPercentage(MORRALE_BOTTOM_VALUE_BEGAIN);

			//右下角
			mMoraleArm = cocostudio::Armature::create("fla_benergyB"); 
			mMoraleArm->setScaleX(-0.75f); 
			mMoraleArm->setScaleY(0.75f);
		}
		break;
	default:
		break;
	}
	mRoot->addChild(mMoraleArm);  
	mMoraleArm->setVisible(false);
	mMoraleArm->getAnimation()->stop();
	mMoraleArm->getAnimation()->setMovementEventCallFunc(this, movementEvent_selector(MoraleView::animationCallback));
	/*if (!(mCampType == ECAMP_TYPE_Player || mCampType == ECAMP_TYPE_Player1))
	{
		if (pframe1)
		{
			(dynamic_cast<Sprite*>(mRoot->getChildByName("Sprite_1")))->setDisplayFrame(pframe1);
		}
	}*/
	changeIco();
	changeName();
	changeIconBg(mPositionType, mCampType == sgBattleControl()->getOwnCamp());
}

void MoraleView::onReceiveMassage(GameMessage * message)
{
	if (message->getMessageType() == EMT_MORALESHOW)
	{
		
		MessageMoraleShow* pMorale = dynamic_cast<MessageMoraleShow*>(message);
		if(NULL != pMorale)		 
		{
			float moraleValue = pMorale->getMoraleValue();
			int srcID = pMorale->getSrcID();
			
			float pProgressValue = moraleValue/4.0;
			switch (mPositionType)
			{
			case M_P_1:
				{
					if (mCampType1 ==  pMorale->getCampType()|| mCampType2 ==  pMorale->getCampType())
					{
						pProgressValue+=MORRALE_TOP_VALUE_BEGAIN;
						mProgress->setPercentage(pProgressValue);
						if (pProgressValue >= MORRALE_TOP_VALUE_MID)
						{
							doPlayBorn(); 
						} 
						else
						{
							doPlayEnd();
						}
						if(srcID > 0)
						{ 
							BaseBuild* pBuild = sgBattleControl()->getTargetBuildByIns(srcID); 
							playMoraleParticle(pBuild->getPosition(), getPosition()); 
						}
					}
				}
				break;
			case M_P_2:
				{
					if (mCampType1 ==  pMorale->getCampType() || mCampType2 ==  pMorale->getCampType())
					{
						pProgressValue+=MORRALE_TOP_VALUE_BEGAIN;
						mProgress->setPercentage(pProgressValue);
						if (pProgressValue >= MORRALE_TOP_VALUE_MID)
						{
							doPlayBorn();
						} 
						else
						{
							doPlayEnd();
						}
						if(srcID > 0)
						{ 
							BaseBuild* pBuild = sgBattleControl()->getTargetBuildByIns(srcID); 
							playMoraleParticle(pBuild->getPosition(), getPosition()); 
						}
					}
					
				}
				break;
			case M_P_3:
				{
					if (mCampType1 ==  pMorale->getCampType() || mCampType2 ==  pMorale->getCampType())
					{
						pProgressValue+=MORRALE_BOTTOM_VALUE_BEGAIN;
						mProgress->setPercentage(pProgressValue);
						if (pProgressValue >= MORRALE_BOTTOM_VALUE_MID)
						{ 
							doPlayBorn();
						} 
						else
						{
							doPlayEnd();
						}
						if(srcID > 0)
						{ 
							BaseBuild* pBuild = sgBattleControl()->getTargetBuildByIns(srcID); 
							playMoraleParticle(pBuild->getPosition(), getPosition()); 
						}
					}
				}
				break;
			case M_P_4:
				{
					if (mCampType1 ==  pMorale->getCampType() || mCampType2 ==  pMorale->getCampType())
					{
						pProgressValue+=MORRALE_BOTTOM_VALUE_BEGAIN;
						mProgress->setPercentage(pProgressValue);
						if (pProgressValue >= MORRALE_BOTTOM_VALUE_MID)
						{ 
							doPlayBorn();
						} 
						else
						{
							doPlayEnd();
						}
						if(srcID > 0)
						{ 
							BaseBuild* pBuild = sgBattleControl()->getTargetBuildByIns(srcID); 
							playMoraleParticle(pBuild->getPosition(), getPosition()); 
						}
					}
				}
				break;
			default:
				break;
			}
			 
		}
	}
	else if (message->getMessageType() == EMT_CHAT_MSG)
	{
		MessageChatMsg* pMes = dynamic_cast<MessageChatMsg*>(message);
		if (pMes && pMes->getChatType() == MSG_TYPE_PVP)
		{
			changeChatMsg();
		}
	}
}

#define  BubblePanel_Tag (11)
void MoraleView::changeChatMsg()
{
	if (sgChattingControl()->getchatPvpHty()->size() < 1) return;

	ChatInfo* _chatinfo= sgChattingControl()->getchatPvpHty()->front(); 
	if(!_chatinfo) return;

	eCampType type = sgPvpControl()->getCampTypeByUid(_chatinfo->mUin);

	if (mCampType == type)
	{
		Node* mBubble = mRoot->getChildByTag(BubblePanel_Tag);
		if (mBubble == nullptr)
		{
			mBubble = CSLoader::getInstance()->createNode("ui/BubblePanel.csb");
			mRoot->addChild(mBubble,2,BubblePanel_Tag);
			Sprite* pBg = dynamic_cast<Sprite*>(mBubble->getChildByName("MsgBg"));
			mMsgLabel = dynamic_cast<ui::Text*>(mBubble->getChildByName("MsgLabel"));
			switch (mPositionType)
			{
			case M_P_1:
				pBg->setSpriteFrame("ui/general/chat_up.png");
				mBubble->setPosition(Vec2(192.27,-85));
				break;
			case M_P_2:
				pBg->setSpriteFrame("ui/general/chat_up.png");
				mBubble->setPosition(Vec2(192.27,-85));
				mMsgLabel->setScaleX(-1.0);
				break;
			case M_P_3:
				pBg->setSpriteFrame("ui/general/chat_down.png");
				mBubble->setPosition(Vec2(192.27,85));
				break;
			case M_P_4:
				pBg->setSpriteFrame("ui/general/chat_down.png");
				mBubble->setPosition(Vec2(192.27,85));
				mMsgLabel->setScaleX(-1.0);
				break;
			default:
				break;
			}
			
		}
		mBubble->setVisible(true);
		mMsgLabel->setText(_chatinfo->mContent);
		mBubble->setOpacity(255.0);
		mBubble->stopAllActions();
		mBubble->runAction(Sequence::create(
			DelayTime::create(3.0),
			FadeOut::create(2.0), NULL));

		CC_SAFE_DELETE(sgChattingControl()->getchatPvpHty()->back());
		sgChattingControl()->getchatPvpHty()->pop_back();
	}
}


void MoraleView::playMoraleParticle(Point startPos, Point endPos)
{
	CCLOG("[trace]endPos is %f,%f", endPos.x, endPos.y);
	ParticleSystemQuad* pParticle = ParticleSystemQuad::create("solider_return.plist");
	if (nullptr == pParticle)
		return;

	pParticle->setPosition(startPos);
	pParticle->setAutoRemoveOnFinish(true);

	Point vecDir = (endPos - startPos) / 2.0f;
	Point ptControl;
	int retRand = cocos2d::random(0, 2);
	if (retRand == 0)
	{
		ptControl = vecDir + vecDir.getPerp() / 2.0f + startPos;
	}
	else if ( retRand == 1)
	{
		ptControl = vecDir + startPos;
	}
	else
	{
		ptControl = vecDir + vecDir.getRPerp() / 2.0f + startPos;
	}

	ccBezierConfig config;
	config.endPosition = endPos;
	config.controlPoint_1 = ptControl;
	config.controlPoint_2 = ptControl;

	pParticle->runAction(Sequence::create(BezierTo::create(0.5f, config), NULL));

	sgBattleControl()->getBattleView()->addChild(pParticle, EBVZ_EFFECT_LOW);
}

void MoraleView::doAnimation(int state)
{
	 
}

void MoraleView::doPlayBorn()
{
	if(mAniState == E_MORALEANI_BORN || mAniState == E_MORALEANI_ING)
		return;
	mAniState = E_MORALEANI_BORN;
	mMoraleArm->setVisible(true);
	mMoraleArm->getAnimation()->play("born");
}

void MoraleView::doPlayStand()
{
	if(mAniState == E_MORALEANI_ING)
		return;
	mAniState = E_MORALEANI_ING;
	mMoraleArm->setVisible(true);
	mMoraleArm->getAnimation()->play("stand", -1, 1);
}

void MoraleView::doPlayEnd()
{
	if(mAniState == E_MORALEANI_END || mAniState == E_MORALEANI_IDLE)
		return;
	mAniState = E_MORALEANI_END;
	mMoraleArm->setVisible(true);
	mMoraleArm->getAnimation()->play("end");
}

void MoraleView::animationCallback(Armature * pAr, MovementEventType type, const std::string& aniName)
{
	if (type == COMPLETE && pAr == mMoraleArm)
	{
		if(aniName == "born")
		{
			doPlayStand();
		}
		else if(aniName == "end")
		{
			mAniState = E_MORALEANI_END;
		}
	}
}

void MoraleView::changeName()
{

	bool v = sgBattleControl()->getIsPvp();
	ui::ImageView* pNameBg = dynamic_cast<ui::ImageView*>(mRoot->getChildByName("Image_1"));
	ui::Text* pName = dynamic_cast<ui::Text*> (mRoot->getChildByName("mName"));

	pNameBg->setVisible(v);
	pName->setVisible(v);

	if (v)
	{
		std::string name = sgPvpControl()->getPvpPlayerName(mCampType);

		pName->setText(name);
	}

	Node* mBubble = mRoot->getChildByTag(BubblePanel_Tag);
	if (mBubble != nullptr)
	{
		mBubble->setVisible(false);
	}
}

void MoraleView::onHeadTouchEvent(cocos2d::Ref*, cocos2d::ui::TouchEventType touchEvent)
{
	if (touchEvent == TOUCH_EVENT_BEGAN)
	{
		updateGlyphInfo();
	}
	else if (touchEvent == TOUCH_EVENT_ENDED || touchEvent == TOUCH_EVENT_CANCELED)
	{
		mGlyphInfoNode->setVisible(false);
	}
}

void MoraleView::updateGlyphInfo()
{
	mGlyphInfoNode->setVisible(true);
	Camp* mCamp= sgBattleControl()->getTargetCamp(mCampType);
	int atkLv = mCamp->getAtkLevel();
	int defLv = mCamp->getDefLevel();
	int spLv = mCamp->getSpdLevel();
	int ppLv = mCamp->getPplLevel();
	int mrLv = mCamp->getMrlLevel();

	
	mAttackTxt->setString(String::createWithFormat("Lv%d",atkLv)->getCString());
	mDefTxt->setString(String::createWithFormat("Lv%d", defLv)->getCString());
	mSpeedTxt->setString(String::createWithFormat("Lv%d", spLv)->getCString());
	mPPTxt->setString(String::createWithFormat("Lv%d", ppLv)->getCString());
	mArTxt->setString(String::createWithFormat("Lv%d", mrLv)->getCString());

	mTotalTxt->setString(String::createWithFormat("%d",atkLv+defLv+spLv+ppLv+mrLv)->getCString());

	mGlyphInfoNode->setPosition(mGlyphInfoVec);
}
