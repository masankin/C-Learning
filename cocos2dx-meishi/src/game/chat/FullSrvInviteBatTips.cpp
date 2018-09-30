#include "FullSrvInviteBatTips.h"
#include "game/ui/PageFunction.h"
#include "channel/ChannelApi.h"
#include "game/data/TableManager.h"
#include "EnvSet.h"
#include "MessageManager.h"
#include "cocos2d.h"
#include "game/net/UserObj.h"
#include "SoundManager.h"
#include "ChattingControl.h"
#include "pvp/PvpRoomControl.h"
#include "game/utils/Comm.h"
#include "game/prop/PropsControl.h"
#include "core/SoundManager.h"

FullSrvInviteBatTips::FullSrvInviteBatTips(void):mInviteBatState(InviteBatType_Null)
{
	cocostudio::ArmatureDataManager::getInstance()->addArmatureFileInfo("fla_kaifangtishi.png", "fla_kaifangtishi.plist", "fla_kaifangtishi.xml");
}


FullSrvInviteBatTips::~FullSrvInviteBatTips(void)
{
	cocostudio::ArmatureDataManager::getInstance()->removeArmatureFileInfo("fla_kaifangtishi.xml");
}

void FullSrvInviteBatTips::onClickBuyEngerCallBack(cocos2d::Ref*)
{
	if (sgUserObj()->getPersonalInfo()->energy() > POWER_BUY_MAX)
	{
		GenerlDialog::show(sgTextHelper(TEXT_COMMTIPS_TIPS),
			StringUtils::format("%s%s", sgTextHelper(TEXT_ACT_JINGLI).c_str(), sgTextHelper(TEXT_BUY_POWER_MAX).c_str()));
		return;
	}
	PowerBuy* pPowerBuy = sgTableManager()->getPowerBuy(sgUserObj()->getPersonalInfo()->energybuycount());
	if (pPowerBuy)
	{
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


FullSrvInviteBatTips* FullSrvInviteBatTips::create()
{

	FullSrvInviteBatTips* pret = new FullSrvInviteBatTips();
	if (pret && pret->init())
	{
		return pret;
	}
	CC_SAFE_DELETE(pret);
	return NULL;
}

void FullSrvInviteBatTips::updateTime(float delta)
{
	this->hideAni();
	sgChattingControl()->setPVPInviteChatInfoPtr(NULL);
}

void FullSrvInviteBatTips::updateUI(ChatInfo* info)
{
	if (!info) this->setVisible(false);
	if (info->mUin == sgUserObj()->getUin())
		return;
	if (info->mContentType != ContentType_Challenge) return;
	resetUI(info);
}

void FullSrvInviteBatTips::resetUI(ChatInfo* info)
{
	if (info->mContentType == ContentType_Challenge)
	{
		mPanelInvite->setEnabled(false);
		mPanelInvite->addClickEventListener(CC_CALLBACK_1(FullSrvInviteBatTips::inviteCallback, this, info->mUin));
		mText->setTag(info->mMsgType);
		mText->setName(info->mName);
		mText->setCallbackName(info->mServerNode);
		this->unschedule(schedule_selector(FullSrvInviteBatTips::updateTime));
		if (sgTimeCalculator()->getTimeLeft(TIME_PVP_FullSrv_Invite) > 0)
		{
			this->scheduleOnce(schedule_selector(FullSrvInviteBatTips::updateTime), sgTimeCalculator()->getTimeLeft(TIME_PVP_FullSrv_Invite));
			mInviteAni->getAnimation()->setMovementEventCallFunc(this, movementEvent_selector(FullSrvInviteBatTips::aniCallback));
			if (mInviteBatState == InviteBatType_Null || mInviteBatState == InviteBatType_End)
			{
				mInviteAni->getAnimation()->play("born");
				this->setInviteBatState(InviteBatType_Born);
			}
			this->setVisible(true);
		}
		else
		{
			this->setInviteBatState(InviteBatType_Null);
			sgChattingControl()->setPVPInviteChatInfoPtr(NULL);
			this->setVisible(false);
		}
	}

}

bool FullSrvInviteBatTips::init()
{
	mRoot = CSLoader::createNode("FullSrvInviteBatTipsNode.csb");
	mPanelInvite = dynamic_cast<ui::Layout*>(mRoot->getChildByName("PanelInvite"));
	mText = dynamic_cast<ui::Text*>(mRoot->getChildByName("Text_1"));
	mInviteAni = cocostudio::Armature::create("fla_kaifangtishi"); 
	mRoot->getChildByName("NodeAni")->addChild(mInviteAni);
	this->addChild(mRoot);
	return this;
}

void FullSrvInviteBatTips::inviteCallback(cocos2d::Ref* pSender,int uin)
{
	if (mText->getTag() == MSG_TYPE_NULL)
	{
		this->hideAni();
		sgChattingControl()->setPVPInviteChatInfoPtr(NULL);
		Window::showTextTip(sgTextHelper(TEXT_CHAT_EXPIRED));
	}
	else
	{
		if (!sgEnvSet("MGAME_OPEN_PVP_LIMIT"))
		{
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
		
		GenerlDialog::show(sgTextHelper(TEXT_COMM_TIPS), content, CC_CALLBACK_1(FullSrvInviteBatTips::acceptInviteCallback, this),false, true, E_GENERAL_TYPE_PK_CONFIRM);
	}
}

void FullSrvInviteBatTips::acceptInviteCallback(cocos2d::Ref* sender)
{
	if (sgUserObj()->getPersonalInfo()->energy() < PVP_NEED_PSYCH)
	{
		PowerBuy* pPowerBuy = sgTableManager()->getPowerBuy(sgUserObj()->getPersonalInfo()->energybuycount());// sgUserObj()->getEnergyBuyCount());
		if (pPowerBuy)
		{
			GenerlDialog::show(sgTextHelper(TEXT_BUY_ITEM__JINGLI),
				StringUtils::format(sgTextHelper(TEXT_BUY_ITEM_JINGLI_SURE).c_str(), pPowerBuy->getEnergyNum()),
				CC_CALLBACK_1(FullSrvInviteBatTips::onClickBuyEngerCallBack, this));
		}
	}
	else
	{
		GenerlDialog::hide();
		sgPvpRoomControl()->sendEnterRoomRequest(mInviteRoomId,mServerNode);
		this->setInviteBatState(InviteBatType_Null);
		this->setVisible(false);
	}
}


void  FullSrvInviteBatTips::aniCallback(cocostudio::Armature * pAr, cocostudio::MovementEventType type, const std::string& aniName)
{
	if (type == cocostudio::COMPLETE)
	{
		if (aniName == "born")
		{
			mPanelInvite->setEnabled(true);
			this->setInviteBatState(InviteBatType_Stand);
			pAr->getAnimation()->play("stand", -1, 1);
		}
		else if (aniName == "end")
		{
			this->setInviteBatState(InviteBatType_Null);
			this->setVisible(false);
		}
	}
}

void FullSrvInviteBatTips::hideAni()
{
	mInviteAni->getAnimation()->play("end");
	this->setInviteBatState(InviteBatType_End);
	mPanelInvite->setEnabled(false);
}
