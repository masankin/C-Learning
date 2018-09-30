#include "MessageManager.h"
#include "net/UserObj.h"
#include "tinyxml2/tinyxml2.h"
#include "TimeCalculator.h"
#include "EnvSet.h"
#include "battle/BattleControl.h"
#include "battle/BattleRecordsManager.h"

using namespace tinyxml2;

MessageManager::MessageManager()
{
	loadFactorys();


}

void MessageManager::loadFactorys()
{

	registerFactory(EMT_ATTACK, new MessageAttackFactory());
	registerFactory(EMT_TRANSFORM, new MessageTransformFactory());
	registerFactory(EMT_MOVE, new MessageMoveFactory());
	registerFactory(EMT_FIGHT, new MessageFightFactory());
	registerFactory(EMT_PRODUCT, new MessageProductFactory());
	registerFactory(EMT_UPGRADE, new MessageUpgradeFactory());
	registerFactory(EMT_TOWER_ATTACK, new MessageBuildAttactFactory());
	registerFactory(EMT_PAGE_GO, new MessagePageFactory());
	registerFactory(EMT_BUILD_DESTORY, new MessageBuildDestroyFactory());
	registerFactory(EMT_SUCC_CHECKSTAR, new MessageCheckStarFactory());
	registerFactory(EMT_REFRESH_STAR, new MessageRefreshStarFactory());
	registerFactory(EMT_CHECKPOINT_START, new MessageCheckPointStartFactory());
	registerFactory(EMT_CHECKPOINT_END, new MessageCheckPointEndFactory());
	registerFactory(EMT_STAR_STATU, new MessageStarStatuFactory());
	registerFactory(EMT_GUIDE_STEP_OVER, new MessageGuideStepOverFactory());
	registerFactory(EMT_OPEN_UI, new MessageOpenUIFactory());

	registerFactory(EMT_CHANNEL_LOGIN, new MessageChannelLoginFactory());
	registerFactory(EMT_CHANNEL_LOGOUT, new MessageChannelLogoutFactory());
	registerFactory(EMP_POWER_CHANGE, new MessagePowerChangedFactory());

	registerFactory(EMT_PROP_CHANGE, new MessageBuildChangeFactory());
	registerFactory(EMT_PROP_CHANGEOK, new MessageBuildChangeOkFactory());
	registerFactory(EMT_PROP_ADDTIME, new MessageAddTimeFactory());
	registerFactory(EMT_PROP_FULLPEOP, new MessageFullPeopelFactory());
	registerFactory(EMT_PROP_FREEZE, new MessageFreezeFactory());
	registerFactory(EMT_PROP_INVISIBILITY, new MessageInvisibilityFactory());
	registerFactory(EMT_PROP_COOKING, new MessageCookingFactory());
	registerFactory(EMT_PROP_Oil, new MessageOilFactory());
	registerFactory(EMT_GUIDE, new MessageGuideDataFactory());
	registerFactory(EMT_GUIDE_TIPS, new MessageGuideTipsFactory());
	registerFactory(EMT_PROP_USER, new MessagePropUserDataFactory());
	registerFactory(EMT_GUIDE_PAUSE, new MessageGuidePauseFactory()); 
	registerFactory(EMT_JUNGLE, new MessageJungleFactory());
	registerFactory(EMT_MORALE, new MessageMoraleFactory());
	registerFactory(EMT_BUFF, new MessageBuffFactory());
	registerFactory(EMT_REDUCEPEOPLE, new MessageReducePeopleFactory());
	registerFactory(EMT_MONEY_CHANGE, new MessageMoneyChangeFactory());
	registerFactory(EMT_RESMODE_SCORE, new MsgResModeScoreFactory());
	registerFactory(EMT_ENERGY_CHANGE, new MessageEnergyChangeFactory());
	registerFactory(EMT_POINT_CHANGE, new MessagePvpPointChangeFactory());
	registerFactory(EMT_CLOSEPAGE, new MessageClosePageFactory());
	registerFactory(EMT_STOP_MATCH, new MessagePvpCloseFactory());
	registerFactory(EMT_CLOSE_PVP, new MessagePvpDilogCloseFactory());
	registerFactory(EMT_CHANGE_NAME, new MessageChangeNameFactory());
	registerFactory(EMT_SIGNAL_CHANGE, new MessageSignalChangeFactory()); 
	registerFactory(EMT_OPEN_STAGE, new MessageOpenStageFactory());
	registerFactory(EMT_MORALESHOW, new MessageMoraleShowFactory());
	registerFactory(EMT_BUFFSHOW, new MessageBuffShowFactory());
	registerFactory(EMT_MENU_BTN, new MessageMenuBtnFactory());
	registerFactory(EMT_BUTTON_OUT_PERCENT, new MessageOutPercentBtnFactory());

	registerFactory(EMT_ROW_DEPART, new MessageRowDepartFactory());
	registerFactory(EMT_FORMATION_OVER, new MessageFormationOverFactory());

	registerFactory(EMT_ACHIEVE_GOTOLEVEL, new MessageAchieveGotoFactory());
	registerFactory(EMT_HONGDIAN_TIP, new MessageAchieveTipFactory());
	registerFactory(EMT_SHOP_PAYCOMPLENT, new MessageShopPayComplentFactory());
	registerFactory(EMT_BATTLE_TIP, new MessageBattleTipsFactory());
	registerFactory(EMT_ENABLE_OPTIONBUTTON, new MessageEnableOptionFactory());
	
	registerFactory(EMT_BTN_CONTROL,new MessageBtnControlFactory());
	registerFactory(EMT_MISSION_STATE, new MessageDayMissionFactory());
	registerFactory(EMT_ACTIVATION_TIP, new MessageActivationTipFactory());

	registerFactory(EMT_CHAT_MSG,new MessageChatMsgFactory());
	registerFactory(EMT_FRIEND_UPDATE, new MessageFriendFactory());
	registerFactory(EMT_SHOP_SHOW_BUYITEM,new MessageShopBuyItemFactory());
	registerFactory(EMT_SHOP_AFTER_BUYDIAMOND, new MessageAfterBuyDiamondFactory());
	registerFactory(EMT_CHANGE_PLAYER_ICON, new MessageChangePlayerIconFactory());
	registerFactory(EMT_BUFFER_TIP, new MessageBufferTipsFactory());
	registerFactory(EMT_GAMESTART, new MessageGameStartFactory());

	registerFactory(EMT_ITEM_OPERATION, new MessageItemOperationFactory());
	registerFactory(EMT_PVP_INFO, new MessagePVPInfoFactory());
	registerFactory(EMT_PROP_USE_ALL, new MessagePropUseAllFactory());
	
	registerFactory(EMT_LITTLETIPS, new LittleTipsMessageFactory());

	registerFactory(EMT_GETSAVEDATAOVER, new GetSaveDataMessageFactory());
	registerFactory(EMT_RQST_RESULT, new RqstResultMessageFactory());
	registerFactory(EMT_SDKLOGIN_SUCCESS, new SDKLOGINMessageFactory());
	registerFactory(EMT_GAMESERVERLOGIN_SUCCESS, new GAMESERVERLOGINMessageFactory());
	registerFactory(EMT_SIMPLIFY_UI, new SimplifyUIMessageFactory());
	registerFactory(EMT_UPDATE_PLAYER_BADGES, new MessageUpdatePlayerBadgesFactory());
	registerFactory(EMT_CHANGE_PLAYER_BADGE, new MessageChangePlayerBadgeFactory());

	registerFactory(EMT_BUTTON_LOGINSCENE, new MessageClickStartGameBtnFactory());
	registerFactory(EMT_BUTTON_LOGINSCENE_UNVISIBLE, new MessageClickStartGameBtnUnvisibleFactory());
	registerFactory(EMT_CRAZY_WEEK, new MessageCrazyWeekFactory());
	registerFactory(EMT_OPEN_LEVEL_DIALOG, new MessageOpenLevelDialogFactory);

	registerFactory(EMT_UPDATE_SHOPLIST, new MessageShopListFactory); 
	registerFactory(EMT_UPDATE_EXCHANGE, new MessageUpdateExchangeFactory);
	registerFactory(EMT_CLOSESHOP, new MessageCloseShopFactory);
	registerFactory(EMT_GOTO_ACTLAB, new MessageGotoActTabFactory);
	registerFactory(EMT_BUY_VIP, new MessageBuyVipFactory);
	registerFactory(EMT_PLAYER_UPGRADE, new MessagePlayerUpgradeFactory);

	registerFactory(EMT_GAME_LOGIN_FLOW, new MessageGameLoginFlowFactory);

	registerFactory(EMT_RED_PACKET_CAN_REWARD, new MessageRedRewardFactory);

	registerFactory(EMT_CHANGE_SERVER, new MessageChangeServerFactory);

	registerFactory(EMT_SELECTWORD_SCHEDULE, new MessageScheduleFactory);

	registerFactory(EMT_CHANGE_SIGN, new MessageChangeSignFactory);
	registerFactory(EMT_CHANGE_EXTRA_VISIBLE, new MessageChangeExtraVisibleFactory);
	registerFactory(EMT_GET_EXTRA, new MessageGetExtraFactory);
	registerFactory(EMT_RECORD_CREEPS_BORN, new MessageRecordCreepsBornFactory);
	registerFactory(EMT_RECORD_OUT_PERCENT, new MessageRecordOutPercentFactory);
	registerFactory(EMT_PVPRESULT_OPENBOX, new MessagePvpResultOpenBoxFactory);
	registerFactory(EMT_OPEN_DAILY_MISSION, new MessageOpenDailyMissionFactory);
	registerFactory(EMT_DALIY_MISSION_TIPS, new MessageDailyMissionTipsFactory);
	registerFactory(EMT_DAILY_MISSION_PROGRESS_BACK, new MessageDailyMissionProgressBackFactory);
	registerFactory(EMT_SHOW_DAILY_MISSION_PROGRESS, new MessageShowDailyMissionProgressFactory);
	registerFactory(EMT_SHARE_SUCCESS, new MessageShareSuccessFactory);
	registerFactory(EMT_SKIP_SUCCESS, new MessageSkipSuccessFactory);
	registerFactory(EMT_INVITE_BAT_SWITCH, new MessageInviteBatSwitchFactory);
	registerFactory(EMT_RECORD_TURRET_UPDATE, new MessageRecordTurretUpdateFactory);
	registerFactory(EMT_RECORD_EXECUTE_TURRET_UPDATE, new MessageRecordExecuteTurretUpdateFactory);
	registerFactory(EMT_HONOR_CHANGE, new MessageHonorChangeFactory);
	registerFactory(EMT_GUIDE_FIRST, new MessageGuideFirstFactory);
	registerFactory(EMT_RANK_CHANGE, new MessageRankChangeFactory);
}

void MessageManager::update()
{
	MESSAGE_LIST msgs;
	mMessages.swap(msgs);
	while (!msgs.empty())
	{
		GameMessage* message = msgs.front();
		unsigned int const id = message->getMessageType();

		MESSAGE_HANDLER_MAP::iterator it = mHandlers.find(id);
		if (it != mHandlers.end())
		{
			std::set<MessageHandler*> handlerset;
			handlerset.insert(it->second.begin(), it->second.end());

			std::set<MessageHandler*>& handlerset_ref = it->second;
			std::set<MessageHandler*>::iterator hanIt = handlerset.begin();
			for (; hanIt != handlerset.end(); ++hanIt)
			{
				if (handlerset_ref.find(*hanIt) != handlerset_ref.end())
					(*hanIt)->onReceiveMassage(message);
			}
		}

		delete message;
		msgs.pop_front();
	}
}

void MessageManager::sendMessage(const GameMessage* message)
{

	if(message == 0)
		return;
	mMessages.push_back(const_cast<GameMessage*>(message));
	//CCLOG("[trace] sendMessage in queue! ID:%d", message->getMessageType());
}

void MessageManager::sendMessage(eMessageType eType, std::string params)
{
	const GameMessage* pMessage = createMessage(eType, params);

	if (!pMessage)
		return;

	sendMessage(pMessage);

}

void MessageManager::regisiterMessageHandler(unsigned int id, MessageHandler* handler)
{
	if(handler == 0)
		return;

	MESSAGE_HANDLER_MAP::iterator it = mHandlers.find(id);
	if(it == mHandlers.end())
	{
		std::set<MessageHandler*> sec;
		sec.insert(handler);
		mHandlers.insert(std::make_pair(id,sec));
	}
	else 
	{
		std::set<MessageHandler*>& handlerset = it->second;
		if(handlerset.find(handler)==handlerset.end())
			handlerset.insert(handler);
	}
}
void MessageManager::removeMessageHandler(unsigned int id, MessageHandler* messagehandler)
{
	if(messagehandler == 0)
		return;

	MESSAGE_HANDLER_MAP::iterator it = mHandlers.find(id);
	if(it != mHandlers.end())
	{
		std::set<MessageHandler*>& handlerset = it->second;
		if(handlerset.find(messagehandler) != handlerset.end())
			handlerset.erase(messagehandler);
	}

}
void MessageManager::removeMessageHandler(MessageHandler* messagehandler)
{
	if(messagehandler == 0)
		return;

	MESSAGE_HANDLER_MAP::iterator it = mHandlers.begin();
	for(;it!=mHandlers.end();++it)
	{
		std::set<MessageHandler*>& handlerset = it->second;
		if(handlerset.find(messagehandler)!= handlerset.end())
		{
			handlerset.erase(messagehandler);
		}
	}
}
void MessageManager::removeMessageHandler(unsigned int id)
{
	MESSAGE_HANDLER_MAP::iterator it = mHandlers.find(id);
	if(it != mHandlers.end())
	{
		it->second.clear();
	}
}

bool MessageManager::registerFactory(int id, GameMessageFactory* factory)
{
	mMessageFactoryMap.insert(std::make_pair(id,factory));
	return true;
}

const GameMessage* MessageManager::createMessage(int id, std::string params)
{
	MessageFactoryMap::iterator it = mMessageFactoryMap.find(id);
	if (it == mMessageFactoryMap.end())
	{
		CCLOG("[error] MessageManager::createMessage factory:%d not registered.", id);
		return NULL;
	}
	//CCLOG("[trace] MessageManager::createMessage message:%d,params:%s.", id, params.c_str());
	return it->second->create(params);
}
