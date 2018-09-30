#include "UserObj.h"
#include "CVTime.h"
#include "TimeCalculator.h"
#include "Message.h"
#include "core/MessageManager.h"
#include "core/HelperFunc.h"
#include <fstream>
#include "data/TableManager.h"
#include "google/protobuf/io/coded_stream.h"
#include "ui/dialogs/GenerlDialog.h"
#include "guide/GuideManager.h"
#include "login/LoginProxy.h"
#include "sync/SyncControl.h"
#include "ui/PageFunction.h"
#include "battle/BattleControl.h"
#include "battle/BattleRecordsManager.h"
#include "ui/dialogs/GiftAwardDialog.h"
#include "lua/LuaFuncHelper.h"
#include "CCLuaEngine.h"
#include "game/battle/Camp.h"
#include "game/utils/GameDefault.h"
#include "game/prop/PropsControl.h"
#include "game/balance/BalanceControl.h"

#define SAVE_USER_FILE "userdata.db"  
#define TEMP_SAVE_USER_FILE "temp_userdata.db"
#define UIN_BASE_DATA 987654
UserObj::UserObj() :
mIsHost(false),
mWaitNum(0),
mIsNewSavaData(false),
mSendGuideRqst(false),
mOpenCheckPtId(0),
mIsShopBuy(false),
mNowPassPt(0),
mLvlUpgrade(false),
mPlayerUpgradeState(0),
mOpendID("0"),
mCurInfoUin(0),
mCurInfoType(0), 
mShowGiftId(0),
mMaxOpenGateID(0), 
mIsShowDailyMissionHot(false),
mIsShowDailyMissionBtn(false),
mIsAfterAwardOpenAgain(false),
mIsShowDailyMissionProgress(false),
mIsOpenDailyMissionFirst(true),
mIsOpenDailyMissionDialog(false), 
mWinnerType(-1),
mIsOpenCrazyWeek(true),
mChannelPlayerName(""),
mHeadIconUrl(""),
mIsSkipSuccess(false),
mHaveGlyph(false)
{
	mPlayerData.Clear();
	initPlayerData();
	Director::getInstance()->getEventDispatcher()->addCustomEventListener("eBthEnterMission", CC_CALLBACK_0(UserObj::doEnterPoint, this));
}

UserObj::~UserObj()
{
	
}


void UserObj::initUserObj()
{ 
	
	std::string dbPath = FileUtils::getInstance()->getWritablePath() + SAVE_USER_FILE;
	std::string tempPath = FileUtils::getInstance()->getWritablePath() + TEMP_SAVE_USER_FILE;
	if(FileUtils::getInstance()->isFileExist(dbPath) == true && loadDataFromFile(dbPath) == true)
	{  
		mIsNewSavaData = false;
	} 
	else if(FileUtils::getInstance()->isFileExist(tempPath) == true)
	{ 
		//将上此rename失败的文件重新命名一次
		int renameRet = rename(tempPath.c_str(), dbPath.c_str());
		if(renameRet == 0)
		{
			mIsNewSavaData = false;
		}
		else
		{
			mIsNewSavaData = true; 
			CCLOG("[error]UserObj::initUserObj rename result is %d", renameRet);
		}
	}
	else
	{
		mIsNewSavaData = true;
	}
	loadPlayerData();
}

bool UserObj::loadDataFromFile(std::string dbPath)
{
	std::fstream dbReadHand(dbPath.c_str(), std::fstream::binary | std::ios::in);
	if (dbReadHand.fail() == true)
		return false;	

	dbReadHand.seekg(0, std::ios::end);
	int length = dbReadHand.tellg();
	dbReadHand.seekg(0, std::ios::beg);
	if (length <= 0)
		return false;

	char* buffer = new char[length];
	dbReadHand.read(buffer, length); 
	bool bParse = mPlayerData.ParseFromArray(buffer, length); 
	delete[] buffer;
	dbReadHand.close();
	return true; 
}


void UserObj::initPlayerData()
{
	CVTime stNow(time(0));
	
	//int uin = HelperFunc::getRandomValule(10000,9999999);
	//mPlayerData.Clear();
	mPlayerData.set_openid("");
	mPlayerData.set_lastsave(stNow.GetTime());
	mPlayerData.set_gold(0);
	mPlayerData.clear_lostpointdata();
	mPlayerData.set_inputid("");
	mPlayerData.clear_achievedata();
	mPlayerData.clear_propitmedata();
	//登录服务列表数据不清
	//mPlayerData.clear_logindata();
	mPlayerData.clear_mcostdata();
	mPlayerData.set_version(1);
	mPlayerData.set_bibletabitemid(101);
	mPlayerData.set_firstfail(1);
	mPlayerData.clear_actnoticedatalist();
	mPlayerData.set_actinformtime(0);
	mPlayerData.set_opengamepasstip(0);
	mPlayerData.set_chatbuytip(0);
	mPlayerData.set_chatfreecount(0);
	mPlayerData.set_chattimeleft(0);
	mPlayerData.set_saveleveltip(0);	
	mPlayerData.clear_bibledata();
	
	//proto数据初始化
	m_stPersonalInfo.set_playertitle(0);
	m_stPersonalInfo.set_roomticket(0);
	m_stPersonalInfo.set_achievementlevel(1);
	m_stPersonalInfo.set_shopguide(0);
	m_stPersonalInfo.set_power(0);
	m_stPersonalInfo.set_powerslot(50);
	m_stPersonalInfo.set_energy(0);
	m_stPersonalInfo.set_energyslot(50);
	m_stPersonalInfo.set_energybuycount(1);
	m_stPersonalInfo.set_diamond(0);
	m_stPersonalInfo.set_glyphguide(0);
	m_stPersonalInfo.set_addexp(0);
	m_stPersonalInfo.set_rank(0);
	m_stPersonalInfo.set_name("");
	m_stPersonalInfo.set_honorcount(0);
	m_stPersonalInfo.set_explevel(1);
	m_stPersonalInfo.set_exp(0);
	m_stPersonalInfo.set_viplevel(0);
	m_stPersonalInfo.set_score(0);
	m_stPersonalInfo.set_winrate(0);
	m_stPersonalInfo.set_pvpround(0);

	mDailyMissionTipList.clear();
	m_LostPointDataMap.clear();
	mActNoticeList.clear();
	mPropMap.clear();
	mObtainedBadges.clear();
}

 

void UserObj::loadPlayerData()
{
	mOccupyModeFlag = mPlayerData.saveleveltip();
	loadPropItem();
	loadLostPointData();
	loadActNoticeData();
}

 
void UserObj::saveToFile()
{
	mPlayerData.set_saveleveltip(mOccupyModeFlag);
	savePropItem();
	saveLostPointData();
	saveActNoticeData();
	//存储放在这个savePlayerData方法之前
	savePlayerData(); 
	
}

void UserObj::savePlayerData()
{
	 std::string tempPath = FileUtils::getInstance()->getWritablePath() + TEMP_SAVE_USER_FILE;
	 if(saveTempPlayerData(tempPath))
	 {
		 bool canRename = true;
		 std::string dbPath = FileUtils::getInstance()->getWritablePath() + SAVE_USER_FILE; 
		 if(FileUtils::getInstance()->isFileExist(dbPath))
		 {
			 if(remove(dbPath.c_str()) != 0)
			 {
				 canRename = false;
			 }
		 } 
		 if(canRename)
		 {
			 int renameRet = rename(tempPath.c_str(), dbPath.c_str()); 
			 if(renameRet != 0)
				 CCLOG("[error]UserObj::savePlayerData rename result = %d", renameRet);
		 }
	 }
}

bool UserObj::saveTempPlayerData(std::string path)
{
	int iProtoSize = mPlayerData.ByteSize(); 
	char* buffer = new char[iProtoSize];
	mPlayerData.SerializeToArray(buffer, iProtoSize); 
	std::fstream* dbSaveHand = new std::fstream(path.c_str(), std::fstream::binary | std::ios::out);
	if (dbSaveHand)
	{
		dbSaveHand->seekp(0, std::ios::beg);
		dbSaveHand->write(buffer, iProtoSize);
		dbSaveHand->flush();
		dbSaveHand->close();
	}
	delete[] buffer; 
	return true; 
}

void UserObj::update(float dt)
{ 
		updateLastServerTime();
}

void  UserObj::changePvePower(int value)
{
	m_stPersonalInfo.set_power(value);//mPVECount = value;
	sgMessageManager()->sendMessage(EMP_POWER_CHANGE,"");
}
void UserObj::changePvpPower(int value)
{
	m_stPersonalInfo.set_energy(value);//mPVPCount = value;
	sgMessageManager()->sendMessage(EMT_ENERGY_CHANGE,"");
}

void UserObj::setDiamond(int value, int src)
{
	//mDiamondCount = value;
	m_stPersonalInfo.set_diamond(value);
	if (src != 1)	//1为充值
		sgMessageManager()->sendMessage(EMT_MONEY_CHANGE, "");
}

void UserObj::setGuideFinish(int groupID)
{
	mGetNewBieListResp.add_newbieids(groupID);
	if(sgTableManager()->getGuideDataVec().size() == mGetNewBieListResp.newbieids_size())
		mGetNewBieListResp.set_iscomplete(true);
	mUserProxy.sendFinishGuideID(groupID,mGetNewBieListResp.iscomplete());
}

void UserObj::syncGuideFinish(std::map<int, bool>& idMap)
{ 
	/*if (idMap.size() == mGetNewBieListResp.newbieids_size())
		mGetNewBieListResp.set_iscomplete(true);*/
	std::vector<int > idList;
	for (std::map<int, bool>::iterator iter = idMap.begin(); iter != idMap.end(); iter++)
	{
		if (iter->second == true)
			idList.push_back(iter->first);
	}
	if (idList.size() == 0)
		return;
	mUserProxy.sendFinishGuideIDList(idList, false);
}

bool UserObj::getGuideFinish(int groupID)
{
	if (mGetNewBieListResp.iscomplete())
	{
		return true;
	}
	for (int i = 0; i< mGetNewBieListResp.newbieids_size(); i++)
	{
		if (groupID == mGetNewBieListResp.newbieids(i))
		{
			return true;
		}
	}
	return false;
}

int UserObj::getMaxGuideGroup()
{
	int maxGroupID = 1;
	for (int i = 0; i< mGetNewBieListResp.newbieids_size(); i++)
	{
		if (maxGroupID >= mGetNewBieListResp.newbieids(i))
			continue;
		else 
			maxGroupID = mGetNewBieListResp.newbieids(i);
	}
	return maxGroupID;
}

void UserObj::sendAddGuideDiamond(int count)
{
	/*if(getShopGuideNum() >= 1)
	{
		doAddGuideDiamond(getGoldCoin()); 
		return;
	}*/
	mUserProxy.sendGuideDiamonRqst(count);
}

void UserObj::doAddGuideDiamond(int count)
{	
	setDiamond(count);
	std::string param = StringUtils::format("%d %d",fgame::CS_CMD_SHOP_GUIDE,0);
	sgMessageManager()->sendMessage(EMT_RQST_RESULT, param);
}

void UserObj::addShopGuideNum(int value)
{
	//setShopGuideNum(getShopGuideNum() + value);
	m_stPersonalInfo.set_shopguide(m_stPersonalInfo.shopguide() + value);
}


 ////////////////
void UserObj::sendAddGuideGlyphStuff()
{ 
	mUserProxy.sendGuideGlyphStuffRqst();
}

void UserObj::doAddGuideGlyphStuff(int value)
{ 
	std::string param = StringUtils::format("%d %d", fgame::CS_CMD_GLYPH_GUIDE_ADDSTUFF, 0);
	sgMessageManager()->sendMessage(EMT_RQST_RESULT, param);
}

void UserObj::addGlyphStuffGuideNum(int value)
{
	//setGlyphStuffGuideNum(getGlyphStuffGuideNum() + value);
	m_stPersonalInfo.set_glyphguide(m_stPersonalInfo.glyphguide() + value);
}
///////////////

 

void UserObj::setBibleTabItemId(int tabItemID)
{
	mPlayerData.set_bibletabitemid(tabItemID);
}

int UserObj::getBibleTabItemId()
{
	int id = 0;
	if (mPlayerData.has_bibletabitemid()) {
		return mPlayerData.bibletabitemid();
	}
	else
	{
		mPlayerData.set_bibletabitemid(101);
		id = 101;
	}
	return id;
}

void UserObj::setFirstFail(int failTag)
{
	mPlayerData.set_firstfail(failTag);
}

int UserObj::getFirstFail()
{
	int id = 0;
	if (mPlayerData.has_firstfail()) {
		return mPlayerData.firstfail();
	}
	else
	{
		mPlayerData.set_firstfail(1);
		id = 1;
	}
	return id;
}

void UserObj::setProp(int id, int count)
{
	PropMap::iterator it = mPropMap.find(id);
	if(it != mPropMap.end())
	{
		it->second = count;
	}
	else
	{
		mPropMap.insert(std::make_pair(id, count));
	}
}
	
int UserObj::getProp(int id)
{
	PropMap::iterator it = mPropMap.find(id);
	if(it != mPropMap.end())
	{
		return it->second;
	}
	return 0;
}

void  UserObj::initProp(const fgame::CSSycPlayerInfoResp& resp)
{
	mPropMap.clear();
	for (int i = 0; i<resp.items_size(); i++)
	{
		mPropMap.insert(std::make_pair(resp.items(i).itemid(),
			resp.items(i).count()));
	}
}

void UserObj::setPropItem(int pos, int id)
{
	PropItemMap::iterator it = mPropItemMap.find(pos);
	if (it != mPropItemMap.end())
	{
		it->second = id;
	}
	else
	{
		mPropItemMap.insert(std::make_pair(pos, id));
	}
	CCLOG("[trace]UserObj::setPropItem save to file");
	saveToFile();
}

void UserObj::cleanPropItem(int id)
{
	PropItemMap::iterator it = mPropItemMap.begin();
	for (; it != mPropItemMap.end();it++)
	{
		if (it->second == id)
		{
			it->second = 0;
		}
	}
}

int UserObj::getPropItem(int pos)
{
	PropItemMap::iterator it = mPropItemMap.find(pos);
	if (it != mPropItemMap.end())
	{
		return it->second;
	}
	return 0;
}

void UserObj::loadPropItem()
{
	mPropItemMap.clear();
	int size = mPlayerData.mutable_propitmedata()->size();
	for (int i = 0; i < size; i++)
	{
		fgame::PropItmeSaveData* pData = mPlayerData.mutable_propitmedata(i);
		if (pData->pos() >= 1 && pData->pos() <= 3)
		{
			mPropItemMap.insert(std::make_pair(pData->pos(), pData->id()));
		}
	}
}

void UserObj::savePropItem()
{
	PropItemMap::iterator it = mPropItemMap.begin();
	for (; it != mPropItemMap.end(); it++)
	{
		int pos = it->first;
		int id = it->second;
		if (refreshPropItemData(pos, id) == false)
		{
			fgame::PropItmeSaveData* pData = mPlayerData.add_propitmedata();
			pData->set_id(id);
			pData->set_pos(pos);
		}
	}
}

bool UserObj::refreshPropItemData(int pos, int id)
{
	int size = mPlayerData.mutable_propitmedata()->size();
	for (int i = 0; i < size; i++)
	{
		fgame::PropItmeSaveData* pData = mPlayerData.mutable_propitmedata(i);
		if (pData->pos() == pos)
		{
			pData->set_id(id);
			return true;
		}
	}
	return false;
}

std::map<int, int> UserObj::getPropList()
{
	return mPropMap;
}

bool UserObj::getIsBranch(int levelID)
{
	return levelID %10>0 ? true : false;
}

int UserObj::getOneLostPointData(int id)
{
	 LostPointMap::iterator iter = m_LostPointDataMap.find(id);
	 if(iter != m_LostPointDataMap.end())
	 {
		 return iter->second;
	 }
	 return -1;
}

void UserObj::updateLostPointData(int id, bool pass, bool save)
{
	int state = pass == true ? 1 : 0;
	LostPointMap::iterator iter = m_LostPointDataMap.find(id);
	if(iter != m_LostPointDataMap.end())
	{ 
		iter->second = state; 
	}
	else
	{
		m_LostPointDataMap.insert(std::make_pair(id, state));	 
	}
	if(save)
	{
		CCLOG("[trace]UserObj::updateLostPointData save to file");
		savePlayerData();
	} 
}

bool UserObj::refreshLostPointData(int id, int state)
{
	int size = mPlayerData.mutable_lostpointdata()->size();
	for(int i = 0; i < size; i++)
	{
		fgame::LostPointData* pData = mPlayerData.mutable_lostpointdata(i);
		if(pData->id() == id)
		{
			pData->set_pass(state); 
			return true;
		}
	}
	return false;
}

void UserObj::loadLostPointData()
{
	m_LostPointDataMap.clear();
	int size = mPlayerData.mutable_lostpointdata()->size();
	for(int i = 0; i < size; i++)
	{
		fgame::LostPointData* pData = mPlayerData.mutable_lostpointdata(i);
		m_LostPointDataMap.insert(std::make_pair(pData->id(), pData->pass()));
	}
}

void UserObj::saveLostPointData()
{
	LostPointMap::iterator iter = m_LostPointDataMap.begin();
	for(; iter != m_LostPointDataMap.end(); iter++)
	{
		if(refreshLostPointData(iter->first, iter->second) == false)
		{
			fgame::LostPointData* pData = mPlayerData.add_lostpointdata();
			pData->set_id(iter->first);
			pData->set_pass(iter->second);
		}
	}
}

bool UserObj::checkIsHaveAllNoticeData()
{
	if (mActNoticeList.size() > 0 && mActNoticeList.size() >= sgTableManager()->getActInformList().size())
	{
		return true;
	}
	else
	{
		return false;
	}

}
bool UserObj::checkIsHaveNoticeData(int id)
{
	bool bFound = false;

	std::vector<int>::iterator iter = mActNoticeList.begin();
	for (; iter != mActNoticeList.end(); iter++)
	{
		if (*iter == id)
			bFound = true;
	}
	return bFound;
}

void UserObj::updateActInformData()
{
	if (!mPlayerData.has_actinformtime())
	{
		sgUserObj()->cleanmActNoticeList();
		mPlayerData.set_actinformtime(time(NULL));
	}
	else 
	{
		if (!CVTimeUtils::IsSameDay(getActInformTime(), time(NULL)))
		{
			sgUserObj()->cleanmActNoticeList();
			mPlayerData.set_actinformtime(time(NULL));
		}
	}

}

int64_t UserObj::getActInformTime()
{
	if (mPlayerData.has_actinformtime())
		return mPlayerData.actinformtime();
	return 0;
}

void UserObj::addActNoticeData(int id)
{
	if (checkIsHaveNoticeData(id))
		return;
	mActNoticeList.push_back(id);
	saveActNoticeData();
}


void UserObj::loadActNoticeData()
{
	mActNoticeList.clear();
	if (!mPlayerData.has_actnoticedatalist()) return;
	int size = mPlayerData.mutable_actnoticedatalist()->actid_size();
	for (int i = 0; i < size; i++)
	{
		int actId = mPlayerData.mutable_actnoticedatalist()->actid(i);
		mActNoticeList.push_back(actId);
	}
}

void UserObj::saveActNoticeData()
{
	mPlayerData.mutable_actnoticedatalist()->clear_actid();
	std::vector<int>::iterator iter = mActNoticeList.begin();
	for (; iter != mActNoticeList.end(); iter++)
	{
		mPlayerData.mutable_actnoticedatalist()->add_actid(*iter);
	}
}


unsigned int UserObj::getUin()
{
	return mUin;
}

void UserObj::setUin(unsigned int value)
{
	mUin = value;
	mNewUin = convertToNewUin(value);
}

unsigned int UserObj::getNewUin()
{
	return mNewUin;
}

unsigned int UserObj::convertToNewUin(unsigned int Uin)
{
	int a[10] = { 5, 4, 6, 7, 1, 0, 2, 3, 9, 8 };
	int tempUin = Uin + UIN_BASE_DATA;
	std::string tempData = StringUtils::format("%d", tempUin);
	std::string strData = "";
	int bitCount = tempData.length();
	for (int i = 0; i < bitCount; i++)
	{
		int bitPow = pow(10, bitCount - i - 1);
		int bitData = (tempUin / bitPow) % 10;
		strData = StringUtils::format("%s%d", strData.c_str(), a[bitData]);
	}
	int newUin = atoi(strData.c_str());
	return newUin;
}

unsigned int UserObj::convertToUin(unsigned int NewUin)
{
	int b[10] = { 5, 4, 6, 7, 1, 0, 2, 3, 9, 8 };
	std::string tempData = StringUtils::format("%d", NewUin);
	std::string strData = "";
	int bitCount = tempData.length();
	for (int i = 0; i < bitCount; i++)
	{
		int bitPow = pow(10, bitCount - i - 1);
		int bitData = (NewUin / bitPow) % 10;
		strData = StringUtils::format("%s%d", strData.c_str(), b[bitData]);
	}
	int tempUin = atoi(strData.c_str());
	int Uin = tempUin - UIN_BASE_DATA;
	return Uin;
}

std::string UserObj::getPlayerName()
{
	std::string szNickName;

	//if(mPlayerName == "")
	if (m_stPersonalInfo.name() == "")
	{
		szNickName = getOpenID().substr(0, 7); 
	}
	else
	{ 
		szNickName = m_stPersonalInfo.name();//mPlayerName;
	} 
	return szNickName;
}

void UserObj::setOpenID(std::string value)
{ 
	SAVE_USER_DEFAULT("USER_OPENID", value);
	if (mPlayerData.openid() != value && mPlayerData.openid() != "")
	{
		initPlayerData();
	}
	mPlayerData.set_openid(value);
	mOpendID = value;
}

std::string UserObj::getOpenID()
{
	return mOpendID;
}

void UserObj::setInputID(std::string value)
{  
	mOpendID = value;
}

bool UserObj::isCanGetMonthCard()
{
	//return mMonthCard>0?true:false;
	return m_stPersonalInfo.monthcarddays() > 0 ? true : false;
}

static bool isLogServerceFirstHeartBeat = true;

bool UserObj::isInitLoginData()
{
	if (!mPlayerData.has_logindata())
	{
		mPlayerData.mutable_logindata()->set_gameservername("gamed0");
		mPlayerData.mutable_logindata()->set_lastservertime(0);
		mPlayerData.mutable_logindata()->set_loginserverdir(sgChannelApi()->getDefaultLogin());
	}

	return true;
}

void UserObj::updateLastServerTime()
{
	if (sgTimeCalculator()->hasKey(TIME_LASTSERVER))
	{
		if (!sgLoginProxy()->getLoginSuccess())
		{
			sgTimeCalculator()->removeTimeCalcultor(TIME_LASTSERVER);
			if (isInitLoginData())
			{
				mPlayerData.mutable_logindata()->set_lastservertime(sgLoginProxy()->getServerTime());
				savePlayerData();
			}
			return;
		}

		//首次登陆的第一个心跳包   不然要等3分钟才存储
		if (isLogServerceFirstHeartBeat &&sgLoginProxy()->getServerTime()>0)
		{
			isLogServerceFirstHeartBeat = false;
			if (isInitLoginData())
			{
				mPlayerData.mutable_logindata()->set_lastservertime(sgLoginProxy()->getServerTime());
				savePlayerData();
			}
		}

		float _left = sgTimeCalculator()->getTimeLeft(TIME_LASTSERVER);

		if (_left <= 0.0f)
		{
			if (isInitLoginData())
			{
				mPlayerData.mutable_logindata()->set_lastservertime(sgLoginProxy()->getServerTime());
				savePlayerData();
			}
			sgTimeCalculator()->resetTimeCalcultor(TIME_LASTSERVER);
		}
	}
}


int64_t UserObj::getLastServerTime()
{
	if (isInitLoginData())
	{
		return mPlayerData.logindata().lastservertime();
	}

	return 0;
}

void UserObj::setGameServerId(std::string name)
{
	if (isInitLoginData())
	{
		mPlayerData.mutable_logindata()->set_gameservername(name);
		//savePlayerData();
	}

	if (sgTimeCalculator()->hasKey(TIME_LASTSERVER)==false)
	{
		isLogServerceFirstHeartBeat = true;
		sgTimeCalculator()->createTimeCalcultor(TIME_LASTSERVER, 60*3);
	}
}

std::string UserObj::getGameServerName()
{
	if (isInitLoginData())
	{
		return mPlayerData.logindata().gameservername();
	}
	return "";
}

int64_t UserObj::getNowTime()
{
	CVTime pRet(time(0));
	return sgLoginProxy()->getServerTime()>0?sgLoginProxy()->getServerTime():pRet.GetTime();
}

void UserObj::setLoginServerDir(std::string dir)
{
	if (isInitLoginData())
	{
		mPlayerData.mutable_logindata()->set_loginserverdir(dir);
		CCLOG("[trace]UserObj::setLoginServerDir : Login dir = %s", dir.c_str());
	}
}

std::string UserObj::getLoginServerDir()
{
	if (isInitLoginData())
	{
		return mPlayerData.logindata().loginserverdir();
	}
	return "";
}

bool UserObj::isInitCostData()
{
	if (!mPlayerData.has_mcostdata())
	{
		mPlayerData.mutable_mcostdata()->set_costday(0);
		mPlayerData.mutable_mcostdata()->set_costmoney(0);
		mPlayerData.mutable_mcostdata()->set_costmonth(0);
		mPlayerData.mutable_mcostdata()->set_costmonthmoney(0);

	}
	return true;
}

void UserObj::addCostMoney(int money)
{
	if(isInitCostData())
	{
		CVTime pNowTime(getNowTime());
		CVTime pCostTime(mPlayerData.mcostdata().costday());
		if (mPlayerData.mcostdata().costday() ==0|| pNowTime.GetDay()!=pCostTime.GetDay())
		{
			mPlayerData.mutable_mcostdata()->set_costday(pNowTime.GetTime());
			mPlayerData.mutable_mcostdata()->set_costmoney(0);
		}

		if(mPlayerData.mcostdata().costmonth() == 0|| mPlayerData.mcostdata().costmonth()!=pCostTime.GetMonth())
		{
			mPlayerData.mutable_mcostdata()->set_costmonth(pNowTime.GetMonth());
			mPlayerData.mutable_mcostdata()->set_costmonthmoney(0);
		}

		mPlayerData.mutable_mcostdata()->set_costmoney(money+mPlayerData.mcostdata().costmoney());
		mPlayerData.mutable_mcostdata()->set_costmonthmoney(money+mPlayerData.mcostdata().costmonthmoney());
	}
}


void UserObj::addTestMoney(int money)
{
}

bool UserObj::isCostMax(int money)
{
	if (sgChannelApi()->getOperatorId()==ChannelApi::OperatorID_Windows )
	{
		return false;
	}

	bool pRet = false;

	if (isInitCostData())
	{
		const PayLimit* pPayLimit = sgTableManager()->getPayLimit();
		if (!pPayLimit)
		{
			return false;
		}

		CVTime pNowTime(getNowTime());
		CVTime pCostTime(mPlayerData.mcostdata().costday());
		if (mPlayerData.mcostdata().costmonth() == pNowTime.GetMonth() && mPlayerData.mcostdata().costmonthmoney()+money>pPayLimit->l_month)
		{
			return true;
		}

		if (mPlayerData.mcostdata().costday() ==0|| pNowTime.GetDay()!=pCostTime.GetDay())
		{
			pRet = false;
		}else
		{
			if (mPlayerData.mcostdata().costmoney()+money>pPayLimit->l_day)
			{
				pRet = true;
			}else
			{
				pRet = false;
			}
		}
	}
	return pRet;
}

std::string UserObj::getPlayerDataStr()
{
	std::string s1 = ""; 
	if(mIsNewSavaData == false) 
	mPlayerData.SerializeToString(&s1);
	return s1;
}

void UserObj::setPlayerDataStr(std::string s1)
{
	mPlayerData.Clear();
	mPlayerData.ParseFromString(s1);
	loadPlayerData();
	sgMessageManager()->sendMessage(EMT_MONEY_CHANGE, "");	
	sgMessageManager()->sendMessage(EMT_CHANGE_NAME,"1"); 
	setSaveDataOnLine(true);
}

bool UserObj::checkNoSavaData()
{
	return mIsNewSavaData;
}

void UserObj::setSaveDataOnLine(bool value)
{
	mIsNewSavaData = !value; 
}

void UserObj::setSaveDataOffLine(bool value)
{
	mIsNewSavaData = !value;
}

void UserObj::onGetSaveDataFinish(cocos2d::Ref* ref)
{ 
	 
}

bool UserObj::syncSaveDataServer(fgame::UserData data)
{
		return true;
}

int UserObj::getCurSaveDataVersion()
{
	if(mPlayerData.has_version())
		return mPlayerData.version();
	return 1;
}

void UserObj::setCurSaveDataVersion(int value)
{
	if(mPlayerData.has_version())
		mPlayerData.set_version(value);
}

////////////////////////////////////////////////////////////////网游模式

void UserObj::sendGetCheckPtData()
{
	mUserProxy.sendGetCheckPtData();
}

void UserObj::sendGetNewBieList()
{
	mUserProxy.sendGetNewBieList();
}

void UserObj::sendEnterPoint(int pointid)
{
	mUserProxy.sendEnterPoint(pointid);
}

void UserObj::sendUpdateCheckPoint()
{
	//上报每日任务相关数据
	sendDailyMissionData(false);

	int timePoints = sgBalanceControl()->getTimeScore();
	int propPoints = sgBalanceControl()->getPropScore();
	const char* pScript = String::createWithFormat("sgBthControl():sendSaveMissionRequest(%d, %d, %d, %d, %d, 0)", mNowPassPt, mNowPassStar, mIsWin ? 1 : 0, mNowPassTime, timePoints + propPoints)->getCString();
	LuaEngine::getInstance()->executeString(pScript);

	//mUserProxy.sendUpdateCheckPoint(mNowPassPt, mNowPassStar, mNowPassTime, mIsWin, mOpenPt);
}

void UserObj::sendDailyMissionData(bool isPvp)
{
	Camp* pCamp = sgBattleControl()->getTargetCamp(sgBattleControl()->getOwnCamp());
	int killPeopleCount = 0;
	int occupyBuildCount = 0;
	int usePropCount = 0;
	if (pCamp)
	{
		killPeopleCount = pCamp->getKillPeople();
		occupyBuildCount = pCamp->getOccupyBuild();
		usePropCount = sgPropsControl()->getUsePropNum();
	}

	const char* pScript1 = "";
	if (isPvp)
		pScript1 = String::createWithFormat("sgMissionControl():sendMissionProress(%d, %d, %d, %d, %d, %d)", 7, killPeopleCount, 8, occupyBuildCount, 9, usePropCount)->getCString();
	else
		pScript1 = String::createWithFormat("sgMissionControl():sendMissionProress(%d, %d, %d, %d, %d, %d)", 5, killPeopleCount, 6, occupyBuildCount, 9, usePropCount)->getCString();
	LuaEngine::getInstance()->executeString(pScript1);
}

void UserObj::doGetCheckPtData(const ::fgame::CSGetCheckPtDataResp& resp)
{
	//if (resp.has_opencheckptid())
	//{
	//	mOpenCheckPtId = resp.opencheckptid();
	//	mNowPassPt = mOpenCheckPtId;
	//	//发送消息给结算界面，通知新手引导执行
	//	std::string param = StringUtils::format("%d %d", fgame::CS_CMD_PVE_CHECK_POINT_LIST, 0);
	//	sgMessageManager()->sendMessage(EMT_RQST_RESULT, param);
	//}
	//bool bflash = false;
	//if (mCheckPtDataMap.size() <= 0)
	//	bflash = true;
	//mCheckPtDataMap.clear();
	//for (int i = 0; i<resp.cpdatas_size(); i++)
	//{
	//	mCheckPtDataMap.insert(std::make_pair(resp.cpdatas(i).id(),resp.cpdatas(i)));
	//	CCLOG("[trace] doGetCheckPtData::LevelId = %d", resp.cpdatas(i).id());
	//}
	//if (!sgBattleControl()->getIsPvp())
	//{
	//	if (bflash)
	//	{
	//		sgMessageManager()->sendMessage(EMT_REFRESH_STAR, "1");

	//	}
	//	else
	//	{
	//		sgMessageManager()->sendMessage(EMT_REFRESH_STAR, "0");
	//	}
	//}

	//if (sgPageFunction()->getCurrentUIType() == MSG_FUC_LOGIN)
	//{
	//	sgMessageManager()->sendMessage(EMT_GAMESERVERLOGIN_SUCCESS, "");
	//}
}
void UserObj::doSaveCheckPtData(const fgame::CSSaveCheckPtDataResp& dataResp)
{
	/*if (!mIsWin)
	{
		sgPageFunction()->gotoFunction(MSG_FUC_WIN_FAIL);
		return;
	}
	auto it = mCheckPtDataMap.find(mNowPassPt);
	if (it != mCheckPtDataMap.end())
	{
		fgame::CheckPtData &lptData = it->second;
		if (mNowPassStar>lptData.stars())
		{
			lptData.set_stars(mNowPassStar);
		}

		lptData.set_reward(dataResp.reward());
		lptData.set_fullreward(dataResp.fullreward());
	}
	else
	{
		fgame::CheckPtData lptData;
		lptData.set_id(mNowPassPt);
		lptData.set_stars(mNowPassStar);
		lptData.set_reward(false);
		lptData.set_fullreward(false);
		mCheckPtDataMap.insert(std::make_pair(lptData.id(), lptData));
	}
	bool bflash = false;
	if (mCheckPtDataMap.size() <= 0)
		bflash = true;
	if (bflash)
	{
		sgMessageManager()->sendMessage(EMT_REFRESH_STAR, "1");

	}
	else
	{
		sgMessageManager()->sendMessage(EMT_REFRESH_STAR, "0");

	}
	sgPageFunction()->gotoFunction(MSG_FUC_WIN_RESULT);*/
}


void UserObj::doGetNewBieList(const ::fgame::CSGetNewBieListResp& resp)
{
	mGetNewBieListResp = resp;  
	if (sgPageFunction()->getCurrentUIType() == MSG_FUC_LOGIN)
	{
		sgMessageManager()->sendMessage(EMT_GAMESERVERLOGIN_SUCCESS, "");
	}
}

void UserObj::doGetNewBieReward(const ::fgame::CSPveCheckAwardResp& resp)
{
	if (resp.haveaward())
		GiftAwardDialog::showDialogByGiftId(resp.giftid());
}

void UserObj::doPropertyUpdate(const fgame::CSPropertyUpdateResp& resp)
{
	for (int i = 0; i< resp.updatedata_size(); i++)
	{
		const fgame::PropertyUpdateData& pUpdate = resp.updatedata(i);
		switch (pUpdate.uptype())
		{
		case PT_DIAMOND:
		{
			int src = 0;
			if (pUpdate.has_src())
				src = pUpdate.src();
			setDiamond(pUpdate.value(), src);
		}
			break;
		case PT_PVEPOWER:
			changePvePower(pUpdate.value());

			break;
		case PT_PVPPOWER:
			changePvpPower(pUpdate.value());

			break;
		case PT_ITEM:
			{
				setProp(pUpdate.value2(),pUpdate.value());
				sgMessageManager()->sendMessage(EMT_PROP_USER,__String::createWithFormat("%d",pUpdate.value2())->getCString());
			}
			break;
		case POWER_SOLT:
			//setPvePowerLimit(pUpdate.value());
			m_stPersonalInfo.set_powerslot(pUpdate.value());
			break;
		case ENERGY_SOLT:
			//setPvpPowerLimit(pUpdate.value());
			m_stPersonalInfo.set_energyslot(pUpdate.value());
			break;
		case MONTH_CARD:
			//setMonthCard(pUpdate.value());
			m_stPersonalInfo.set_monthcarddays(pUpdate.value());
			break;
		case TITLE_ADD:
			//addBadge(pUpdate.value());
			addBadgeItem(pUpdate.extra());
			break;
		case ROOM_TICKET:
			//setRoomTicketCount(pUpdate.value());
			m_stPersonalInfo.set_roomticket(pUpdate.value());
			break;
		case PLAYER_EXP:
			updatePlayerExp(pUpdate.extra());
			break;
		case Honor:
			//setHonorCount(pUpdate.value());
			m_stPersonalInfo.set_honorcount(pUpdate.value());
			sgMessageManager()->sendMessage(EMT_HONOR_CHANGE, "");
			break;
		case POINT:
			setScore(pUpdate.value());
			break;
		case CHAT:
			sgChattingControl()->chatDataReset(pUpdate.value(),pUpdate.value2());
			break;
		default:
			break;;
		}

	}

}

void  UserObj::doEnterPoint()
{
	sgBattleRecordsManager()->mBattleRecords.clear();
	sgPageFunction()->setCurrentUIType(MSG_FUC_MINI_LEVEL);
	PageFunction::Get()->gotoFunction(MSG_FUC_PLAYING);
	Window*  pLevelDialog = sgPageFunction()->getWindowByPageName("LevelDialog.csb");
	if (pLevelDialog && pLevelDialog->isShow())
	{
		pLevelDialog->setShow(false);
	}
	int checkpt = sgGamePrecedure()->getCurMapInfo().checkpt;
	char buff[32];
	memset(buff, 0, 32);
	unsigned int uin = getUin();
	sprintf(buff, "%d_%d%s", uin, checkpt, "tryCount");
	int tryCount = USER_DEFAULT_INT(buff);
	SAVE_USER_DEFAULT_INT(buff, tryCount + 1);
}

int UserObj::getCheckPointStar(int lv)
{
	auto it = mCheckPtDataMap.find(lv);
	if(it != mCheckPtDataMap.end())
	{
		return (*it).second.stars();
	}
	return 0;
}

int UserObj::getTotalStars()
{
	LuaEngine* scriptEngine = dynamic_cast<LuaEngine*>(ScriptEngineManager::getInstance()->getScriptEngine());
	LuaStack* stack = scriptEngine->getLuaStack();

	int score = 0;
	auto callback = [&score](lua_State* tolua_S, int argc){
		if (argc >= 1)
		{
			if (lua_isnumber(tolua_S, -1))
				score = lua_tointeger(tolua_S, -1);
		}
	};

	auto luaFuncHandler = sgLuaFuncHandlerManager()->getHandler(LuaFuncHandlerManager::ModuleType::kCustom, (int)LuaFuncHandlerManager::CustomHandlerType::kGetBthTotalScore);
	if (luaFuncHandler > 0)
	{
		auto tolua_S = stack->getLuaState();

		LuaStatePrint(tolua_S);
		auto rlt = stack->executeFunction(luaFuncHandler, 0, 1, callback);
		LuaStatePrint(tolua_S);
		stack->clean();
	}

	return score;

	/*int starNum = 0;
	auto iter = mCheckPtDataMap.begin();
	for (; iter != mCheckPtDataMap.end(); iter++)
	{
		starNum+=iter->second.stars();
	}
	return starNum;*/
}

int UserObj::getMaxOpenCheckPoint(bool realTime)
{
	if(realTime)
	{
		if (mCheckPtDataMap.size()>0)
		{

			auto it = mCheckPtDataMap.end();
			it--;
			return it->second.id() > mOpenCheckPtId ? it->second.id() : mOpenCheckPtId;
		}
	}
		
	int pRet = 0;
	if (mCheckPtDataMap.size()>0)
	{

		auto it = mCheckPtDataMap.end();
		it--;
		int gateID = it->second.id();
		const CheckPoint* pData = sgTableManager()->getCheckPointByID(gateID);
		if (!pData || pData->levelType == E_Level_Trans)
			return 0; 
		return gateID;
	}
	if (mOpenCheckPtId>0)
	{
		return mOpenCheckPtId;
	}
	return pRet;
}

void UserObj::updateCheckPoint(int id, int star, int passTime ,int openCheckPt)
{
	/*auto	iter = mCheckPtDataMap.find(id);
	if(iter != mCheckPtDataMap.end())
	{
		if(iter->second.stars() >= star)
		{
			return;
		}

	}*/
	mNowPassPt = id;
	mNowPassStar = star;
	mNowPassTime = passTime;
	mOpenPt = openCheckPt;
	mIsWin = true;
	if (mNowPassStar < 0)
	{
		mNowPassStar = 0;
		mIsWin = false;
		sendUpdateCheckPoint();
	}
}

int UserObj::getCheckPointState(int id)
{
	eCheckPointState mCheckPtState = E_CHECKPOINTSTATE_CLOSE;
	auto iter = mCheckPtDataMap.find(id);
	if(iter != mCheckPtDataMap.end())
	{
		mCheckPtState = E_CHECKPOINTSTATE_PASS;
	}else
	{
		const CheckPoint* lCheckPt = sgTableManager()->getCheckPointByID(id);
		auto lCheckPtit = mCheckPtDataMap.find(lCheckPt->preCheckPt);
		if (lCheckPtit!= mCheckPtDataMap.end())
		{
			mCheckPtState = E_CHECKPOINTSTATE_OPEN;
		}
	}

	return mCheckPtState;

}


int UserObj::getNowPassCheckPoint()
{
	return mNowPassPt;
}



int UserObj::getNextCheckPt()
{
	return getMaxOpenGateID();
	/*int allStar = sgUserObj()->getTotalStars();
	std::vector<int> openList = sgTableManager()->getNextCheckPoints(getMaxOpenCheckPoint(), allStar);
	for (unsigned int i = 0; i < openList.size(); i++)
	{
		int checkPoint = openList.at(i);
		int state = sgUserObj()->getCheckPointState(checkPoint);
		if (state == E_CHECKPOINTSTATE_OPEN && checkPoint % 10 == 0)
		{
			return checkPoint;
		}
	}
	return -1;*/
}

int UserObj::getGateState(int id)
{
	std::map<int, int>::iterator iter = mGuideGateDataMap.find(id);
	if (iter != mGuideGateDataMap.end())
		return iter->second;
	return E_CHECKPOINTSTATE_CLOSE;
}

void UserObj::setGateState(int id, int state)
{
	std::map<int, int>::iterator iter = mGuideGateDataMap.find(id);
	if (iter != mGuideGateDataMap.end())
		iter->second = state;
	else
		mGuideGateDataMap.insert(std::make_pair(id, state));
}

void UserObj::clearGateState()
{
	setMaxOpenGateID(1010);
	mGuideGateDataMap.clear();
}

int UserObj::getHasSkipCount()
{
	LuaEngine* scriptEngine = dynamic_cast<LuaEngine*>(ScriptEngineManager::getInstance()->getScriptEngine());
	LuaStack* stack = scriptEngine->getLuaStack();

	int skipCount = 0;

	auto callback = [&skipCount](lua_State* tolua_S, int argc){
		if (argc >= 1)
		{
			if (lua_isnumber(tolua_S, -1))
				skipCount = lua_tointeger(tolua_S, -1);
		}
	};

	auto luaFuncHandler = sgLuaFuncHandlerManager()->getHandler(LuaFuncHandlerManager::ModuleType::kCustom, (int)LuaFuncHandlerManager::CustomHandlerType::kGetMissionSkipCount);
	if (luaFuncHandler > 0)
	{
		auto tolua_S = stack->getLuaState();

		LuaStatePrint(tolua_S);
		auto rlt = stack->executeFunction(luaFuncHandler, 0, 1, callback);
		LuaStatePrint(tolua_S);
		stack->clean();
	}

	return skipCount;
}

void UserObj::onSkip(int id)
{
	int hasSkipCount = getHasSkipCount();
	const CheckPoint* p_Info = sgTableManager()->getCheckPointByID(id);
	int skipSize = p_Info->skipValueList.size();
	if (hasSkipCount < skipSize)
	{
		int diamond = getPersonalInfo()->diamond();
		int value = atoi(p_Info->skipValueList.at(hasSkipCount).c_str());
		if (diamond < value)
		{
			sgPageFunction()->gotoFunction(MSG_FUC_WIN_SHOP);
			sgMessageManager()->sendMessage(EMT_SHOP_SHOW_BUYITEM, "1");
		}
		else
		{
			const char* pScript = String::createWithFormat("sgBthControl():sendSkipMissionRequest(%d)", id)->getCString();
			LuaEngine::getInstance()->executeString(pScript);
		}
	}
	else
	{
		Window::showTextTip(sgTextHelper(TEXT_BTH_TITLE_SKIP).c_str());
	}
}

bool UserObj::getReceiveAward(int pointId, int type)
{
	auto it = mCheckPtDataMap.find(pointId);
	bool bHas = true;
	if (it != mCheckPtDataMap.end())
	{
		switch (type)
		{
		case 1:
			bHas = it->second.reward();
			break;
		case 2:
			bHas = it->second.fullreward();
			break;
		}
	}
	return bHas;
}

void UserObj::setReceiveAward(int pointId,int type)
{ 
	mUserProxy.sendGetGift(pointId, type);
}

void UserObj::getNewBieReward()
{
	mUserProxy.sendGetNewBieReward();
}
int UserObj::getRankGroup() const
{
	//return (int)(sgRankingControl()->getPlayerGroupType());

	auto pGradeData = sgTableManager()->getGradeData(m_stPersonalInfo.score());//mPvpPoint);
	if (pGradeData)
		return pGradeData->getGroup();

	return RANKINGGROUP_DEFAULT;
}

void UserObj::setScore(int val)
{
	m_stPersonalInfo.set_score(val);
	//if (m_stPersonalInfo.score() < 1200)
		//m_stPersonalInfo.set_score(1200);

	sgMessageManager()->sendMessage(EMT_POINT_CHANGE, "");

}

void UserObj::initBadges(const fgame::CSSycPlayerInfoResp& resp)
{
// 	mBadgeGetVec.clear();
// 	for (int i = 0; i < resp.titlelist_size(); i++)
// 	{
// 		mBadgeGetVec.push_back(resp.titlelist(i));
// 	}

	mObtainedBadges.clear();
	for (int i = 0; i < resp.titleitems_size(); ++i)
	{
		BadgeItem item;
		const fgame::CSTitleItem& netData = resp.titleitems(i);
		item.setType(netData.type());
		item.setTime(netData.time());
		mObtainedBadges.emplace(std::make_pair(item.getType(), std::move(item)));
	}
}

void UserObj::addBadge(int iBadge)
{
// 	bool bfound = isHaveBadge(iBadge);
// 	if (!bfound)
// 	{
// 		mBadgeGetVec.push_back(iBadge);
// 		sgMessageManager()->sendMessage(EMT_UPDATE_PLAYER_BADGES, "");
// 
// 	}
}


void UserObj::addBadgeItem(std::string strJason)
{
	rapidjson::Document json;
	rapidjson::StringStream stream(strJason.c_str());
	if (strJason.size() >= 3) {
		// Skip BOM if exists
		const unsigned char* c = (const unsigned char *)strJason.c_str();
		unsigned bom = c[0] | (c[1] << 8) | (c[2] << 16);

		if (bom == 0xBFBBEF)  // UTF8 BOM
		{
			stream.Take();
			stream.Take();
			stream.Take();
		}
	}

	json.ParseStream<0>(stream);
	if (json.HasParseError()) 
	{
		CCLOG("[error]UserObj::addBadgeItem ParseStream error :%s\n", json.GetParseError());
		return;
	}

	BadgeItem item;
	item.setType(DICTOOL->getIntValue_json(json, "type"));
	item.setTime(DICTOOL->getIntValue_json(json, "time"));

	auto pItem = this->getBadgeItem(item.getType());
	if (pItem)
	{
		pItem->setTime(item.getTime());
	}
	else
	{
		mObtainedBadges.emplace(std::make_pair(item.getType(), item));
	}

	sgMessageManager()->sendMessage(EMT_UPDATE_PLAYER_BADGES, "");
}


std::vector<int> UserObj::getBadges()
{
	std::vector<int> tempVec;
	for (auto & e : mObtainedBadges)
		tempVec.push_back(e.first);
	return tempVec;
}

const std::map<unsigned int, BadgeItem> & UserObj::getObtainedBadges() const
{
	return mObtainedBadges;
}

bool UserObj::isHaveBadge(int iBadge)
{
// 	bool bfound = false;
// 	std::vector<int>::iterator it = mBadgeGetVec.begin();
// 	for (; it != mBadgeGetVec.end(); ++it)
// 	{
// 		if (*it == iBadge)
// 			bfound = true;
// 	}
// 	return bfound;

	return mObtainedBadges.end() != mObtainedBadges.find(iBadge);

}

BadgeItem* UserObj::getBadgeItem(int iBadge)
{
	auto itrFind = mObtainedBadges.find(iBadge);
	if (mObtainedBadges.end() != itrFind)
		return &(itrFind->second);

	return nullptr;
}

void UserObj::initVipTime(std::string str)
{
	mVipTime.clear();


	if (!str.empty())
	{
		
		vector<string> v1 = splitStr(str, "#");
		if (v1.size()>=2)
		{
			for (int i = 0; i < v1.size(); ++i)
			{
				vector<std::string> v2 = splitStr(v1[i], "|");
				if (v2.size() == 2)
				{
					CVTimeSpan time(atol(v2[1].c_str()));
					mVipTime.insert(std::make_pair(atoi(v2[0].c_str()), time));
				}
			}
		}
		else
		{
			vector<string> v2 = splitStr(str, "|");
			if (v2.size() == 2)
			{
				CVTimeSpan time(atol(v2[1].c_str()));
				mVipTime.insert(std::make_pair(atoi(v2[0].c_str()), time));
			}
		}
	}
}

CVTimeSpan UserObj::getVipTime(int id)
{
	CVTimeSpan time(0);
	auto it = mVipTime.find(id);
	if (it != mVipTime.end())
		time = it->second;

	return time;
}

void UserObj::updatePlayerExp(string str)
{
	rapidjson::Document json;
	rapidjson::StringStream stream((const char*)str.c_str());
	if (str.size() >= 3) {
		// Skip BOM if exists
		const unsigned char* c = (const unsigned char *)str.c_str();
		unsigned bom = c[0] | (c[1] << 8) | (c[2] << 16);

		if (bom == 0xBFBBEF)  // UTF8 BOM
		{
			stream.Take();
			stream.Take();
			stream.Take();
		}
	}
	json.ParseStream<0>(stream);
	if (json.HasParseError()) {
		CCLOG("[error]UserObj::updatePlayerExp ParseStream error :%s\n", json.GetParseError());
		return;
	}
		int curLevel = DICTOOL->getIntValue_json(json, "level");
		int curExp = DICTOOL->getIntValue_json(json, "exp");
		int lastLevel = m_stPersonalInfo.explevel();//getPlayerLevel();
		int lastExp = m_stPersonalInfo.exp();//getPlayerCurEX();

		m_stPersonalInfo.set_exp(curExp);//setPlayerCurEX(curExp);
		m_stPersonalInfo.set_explevel(curLevel);//setPlayerLevel(curLevel);

		CCLOG("[trace]UserObj::updatePlayerExp level:%d,%d", curLevel, lastLevel);
		CCLOG("[trace]UserObj::updatePlayerExp exp:%d,%d", curExp, lastExp);
	if (curLevel > lastLevel)
	{
		int addlevel = curLevel - lastLevel;
		int allAddexp = 0;
		for (int i = addlevel; i > 0; i--)
		{
			allAddexp += sgTableManager()->getLevelExp(curLevel - i);
		}
		if (allAddexp > 0)
		{
			allAddexp += curExp;
			allAddexp -= lastExp;
		}
		CCLOG("[trace]UserObj::updatePlayerExp allAddexp:%d", allAddexp);
		m_stPersonalInfo.set_addexp(allAddexp);//setPlayerAddEX(addExp);
		setLvlUpgrade(true);
		sgMessageManager()->sendMessage(EMT_PLAYER_UPGRADE, "");
	}
	else
	{
		m_stPersonalInfo.set_addexp(curExp - lastExp);//setPlayerAddEX(curExp - lastExp);
	}
}

 

void UserObj::doGetGiftReward(const fgame::CSRetriveCheckPtRewardResp& resp)
{
	auto pArrt = sgTableManager()->getCheckPointByID(resp.id());
	if (!pArrt) return;

	sgMessageManager()->sendMessage(EMT_BTN_CONTROL, StringUtils::format("%d %d", resp.id(), 0));
	int giftId = resp.rewardtype()==1  ? pArrt->baseBonus : pArrt->perfectBonus;
	GiftAwardDialog::showDialogByGiftId(giftId);

	auto it = mCheckPtDataMap.find(pArrt->id);
	if (it != mCheckPtDataMap.end())
	{
		fgame::CheckPtData &lptData = it->second;
		if (resp.rewardtype() == 1)
			lptData.set_reward(resp.ret());
		else if (resp.rewardtype() == 2)
			lptData.set_fullreward(resp.ret());
	}
	else
	{
		fgame::CheckPtData lptData;
		lptData.set_id(mNowPassPt);
		lptData.set_stars(mNowPassStar);
		lptData.set_reward(false);
		lptData.set_fullreward(false);
		mCheckPtDataMap.insert(std::make_pair(lptData.id(), lptData));
	}
}

bool UserObj::loadGlyphStuff()
{
	auto luaFuncHandler = sgLuaFuncHandlerManager()->getHandler(LuaFuncHandlerManager::ModuleType::kCustom, (int)LuaFuncHandlerManager::CustomHandlerType::kGetGlyphStuffData);
	if (0 == luaFuncHandler)
	{
		CCLOG("[error]UserObj::load : No register lua handler");
		return false;
	}

	LuaEngine* scriptEngine = dynamic_cast<LuaEngine*>(ScriptEngineManager::getInstance()->getScriptEngine());
	if (nullptr == scriptEngine)
	{
		CCLOG("[error]UserObj::load : Script engine is null");
		return false;
	}

	LuaStack* stack = scriptEngine->getLuaStack();
	if (nullptr == stack)
	{
		CCLOG("[error]UserObj::load : Lua stack is null");
		return false;
	}

	auto tolua_S = stack->getLuaState(); 

	LuaStatePrint(tolua_S);
	auto rlt = stack->executeFunction(luaFuncHandler, 0, 1, std::bind(&UserObj::onLuaGetGlyphStuff, this, std::placeholders::_1, std::placeholders::_2));
	LuaStatePrint(tolua_S);

	if (rlt == 0)
		lua_settop(tolua_S, 0);
	else
		lua_pop(tolua_S, 1);
}

void UserObj::onLuaGetGlyphStuff(lua_State* tolua_S, int argc)
{  
	mGlyphStuffList.clear();

	if (lua_istable(tolua_S, -1))
	{  
		lua_pushnil(tolua_S); 
		while (lua_next(tolua_S, -2))
		{ 
			int key;
			if (lua_isnumber(tolua_S, -2))
				key = lua_tointeger(tolua_S, -2); 
			else
			{
				lua_pop(tolua_S, 1);
				continue;
			} 
			int value = 0;
			if (lua_isnumber(tolua_S, -1))
				value = lua_tointeger(tolua_S, -1);

			mGlyphStuffList.insert(std::make_pair(key, value));

			lua_pop(tolua_S, 1);
		}
	}  
}

int UserObj::getGlyphStuffNum(int stuffID)
{
	std::map<int, int>::iterator iter = mGlyphStuffList.find(stuffID);
	if (iter != mGlyphStuffList.end())
		return iter->second;
	return 0;
}

std::string UserObj::getPersonalString()
{
	m_stPersonalInfo.SerializePartialToString(&m_szPersonalString);
	return m_szPersonalString;
}

bool UserObj::isOpenGamePassTip()
{
	int data = mPlayerData.opengamepasstip();
	return data == 1;
}

void UserObj::changeGamePassTip(bool bOpen)
{
	int data = bOpen ? 1 : 0;
	mPlayerData.set_opengamepasstip(data);
}

void UserObj::showGiftInfoById(int gift_id)
{
	sgUserObj()->setShowGiftId(gift_id);
	sgPageFunction()->gotoFunction(MSG_FUC_LUA_WIN, "GiftInfoDialog.csb", 1);
}

void UserObj::addDailyMissionTips(int type, Point pos)
{
	mDailyMissionTipList.insert(std::make_pair(type, pos));
}

Point UserObj::getDailyMissionTipPointByType(int type)
{
	auto tip = mDailyMissionTipList.find(type);
	if (tip != mDailyMissionTipList.end())
	{
		return tip->second;
	}
	return Point(0.0, 0.0);
}
void UserObj::addLoginSrv2List(int channelId, const std::string& address)
{
/*	bool bNeedAdd = true;
	typedef ::google::protobuf::RepeatedPtrField<::std::string> ProtoString;

	::fgame::LoginData* pLoginData = mPlayerData.mutable_logindata();
	ProtoString* pLoginList = pLoginData->mutable_loginlist();

	for (int i = 0; i != pLoginList->size(); ++i)
	{
		const std::string szItem = pLoginList->Get(i);

		if (szItem.compare(szLogin) == 0)
		{
			bNeedAdd = false;
			break;
		}
	}

	if (bNeedAdd)
		pLoginData->add_loginlist(szLogin);*/

	bool bNeedAdd = true;

	::fgame::LoginData* pLoginData = mPlayerData.mutable_logindata();
	auto pLoginSrvList = pLoginData->mutable_loginsrvlist();

	for (int i = 0; i < pLoginSrvList->size(); ++i)
	{
		auto& loginSvr = pLoginSrvList->Get(i);
		if (loginSvr.channelid() == channelId && loginSvr.address() == address)
		{
			bNeedAdd = false;
			break;
		}
	}

	if (bNeedAdd)
	{
		auto pNewData = pLoginSrvList->Add();
		if (pNewData)
		{
			pNewData->set_channelid(channelId);
			pNewData->set_address(address);
		}
	}
}

void UserObj::dltLoginSrvFromList(int channelId, const std::string& address)
{
/*
	bool bNeedDlt = false;

	typedef ::google::protobuf::RepeatedPtrField<::std::string> ProtoString;

	::fgame::LoginData* pLoginData = mPlayerData.mutable_logindata();
	ProtoString* pLoginList = pLoginData->mutable_loginlist();

	int iIndex = 0;
	for (iIndex = 0; iIndex != pLoginList->size(); ++iIndex)
	{
		const std::string szItem = pLoginList->Get(iIndex);

		if (szItem.compare(szLogin) == 0)
		{
			bNeedDlt = true;
			break;
		}
	}

	if (bNeedDlt)
		pLoginList->DeleteSubrange(iIndex, 1);*/

	bool bNeedDlt = false;

	::fgame::LoginData* pLoginData = mPlayerData.mutable_logindata();
	auto pLoginSrvList = pLoginData->mutable_loginsrvlist();

	int iIndex = 0;
	for (iIndex = 0; iIndex < pLoginSrvList->size(); ++iIndex)
	{
		auto& loginSvr = pLoginSrvList->Get(iIndex);
		if (loginSvr.channelid() == channelId && loginSvr.address() == address)
		{
			bNeedDlt = true;
			break;
		}
	}

	if (bNeedDlt)
	{
		pLoginSrvList->DeleteSubrange(iIndex, 1);
	}
}

std::string UserObj::sltLoginSrvRand(int channelId)
{
/*
	::fgame::LoginData* pLoginData = mPlayerData.mutable_logindata();

	int iSize = pLoginData->loginlist_size();
	if (iSize == 0)
		return "";
	
	int iRand = rand() % iSize;
	return pLoginData->loginlist(iRand);*/

	std::string rltSvr;
	std::vector<std::string> srvList;
	::fgame::LoginData* pLoginData = mPlayerData.mutable_logindata();
	auto pLoginSrvList = pLoginData->mutable_loginsrvlist();
	for (int i = 0; i < pLoginSrvList->size(); ++i)
	{
		auto& loginSvr = pLoginSrvList->Get(i);
		if (loginSvr.channelid() == channelId)
			srvList.push_back(loginSvr.address());
	}

	if (!srvList.empty())
	{
		int maxIndex = srvList.size() - 1;
		auto index = cocos2d::random(0, maxIndex);
		rltSvr = srvList[index];
	}
	
	return rltSvr;
}

bool UserObj::getIsGoneNewerProtect()
{ 
	/*bool isGone = false;
	if (getScore() > 0)
	{
	isGone = true;
	}
	else
	{
	if (mPlayerWinCount >= 5 || mPlayerConWinCount >= 3)
	isGone = true;
	}
	return isGone;*/
	int roundNum = sgEnvSet("NEWER_PROTECT_LEVEL") - getPersonalInfo()->pvpround();
	return (roundNum <= 0);
}

void UserObj::resetDailyMissionData()
{
	mIsShowDailyMissionBtn = false;
	mIsAfterAwardOpenAgain = false;
	mIsShowDailyMissionProgress = false;
	mIsOpenDailyMissionFirst = true;
	mIsOpenDailyMissionDialog = false;
	mIsShowDailyMissionHot = false;
	clearDailyMissionTips();

	const char* pScript = "sgMissionControl():resetData()";
	LuaEngine::getInstance()->executeString(pScript);
}
