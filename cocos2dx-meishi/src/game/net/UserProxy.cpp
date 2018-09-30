#include "UserProxy.h"
#include "net/NetManager.h"
#include "net/UserObj.h"
#include "guide/GuideManager.h"
#include "login/LoginProxy.h"
#include "ui/dialogs/GenerlDialog.h"
#include "ui/PageFunction.h"

UserProxy::UserProxy(void)
{
	//sgNetManager()->RegCmdListener(fgame::CS_CMD_PVE_CHECK_POINT_SAVE, this);
	//sgNetManager()->RegCmdListener(fgame::CS_CMD_PVE_CHECK_POINT_LIST, this);
	sgNetManager()->RegCmdListener(fgame::CS_CMD_SET_NEWBIE,this);
	sgNetManager()->RegCmdListener(fgame::CS_CMD_GET_NEWBIE_LIST,this);
	sgNetManager()->RegCmdListener(fgame::CS_CMD_PROPERTY_UPDATE,this);
	//sgNetManager()->RegCmdListener(fgame::CS_CMD_ENTER_POINT,this);
	sgNetManager()->RegCmdListener(fgame::CS_CMD_SHOP_GUIDE, this);
	sgNetManager()->RegCmdListener(fgame::CS_CMD_GLYPH_GUIDE_ADDSTUFF, this);
	sgNetManager()->RegCmdListener(fgame::CS_CMD_PVE_CHECK_POINT_RETRIVE_REWARD, this);
	sgNetManager()->RegCmdListener(fgame::CS_CMD_PVE_CHECK_AWARD, this);   //新手训练营奖励
}


UserProxy::~UserProxy(void)
{

}

bool UserProxy::fault(const fgame::CSErrorMsg &err)
{
	return true;
}

void UserProxy::DoRsp(NetPack& stRsp)
{
	unsigned long msgID = stRsp.getMsgHead()->mMsgID;
	if (stRsp.getS2CMsg()->has_error_msg())
	{
		CCLOG("[trace]LoginProxy::DoRsp error!");

	}/*else if (stRsp.getS2CMsg()->has_savecheckptdata_resp())
	{
		sgUserObj()->doSaveCheckPtData(stRsp.getS2CMsg()->savecheckptdata_resp());
		
	}
	else if (stRsp.getS2CMsg()->has_retrivecheckptreward_resp())
	{
		sgUserObj()->doGetGiftReward(stRsp.getS2CMsg()->retrivecheckptreward_resp());
	}
	else if (stRsp.getS2CMsg()->has_getcheckptdata_resp())
	{
		sgUserObj()->doGetCheckPtData(stRsp.getS2CMsg()->getcheckptdata_resp());
	}*/else if (stRsp.getS2CMsg()->has_getnewbielist_resp())
	{
		sgUserObj()->doGetNewBieList(stRsp.getS2CMsg()->getnewbielist_resp());
	}else if (stRsp.getS2CMsg()->has_setnewbie_resp())
	{

	}else if (stRsp.getS2CMsg()->has_propertyupdate_resp())
	{
		sgUserObj()->doPropertyUpdate(stRsp.getS2CMsg()->propertyupdate_resp());
	}/*else if (stRsp.getS2CMsg()->has_enterpoint_resp())
	{
		sgUserObj()->doEnterPoint(stRsp.getS2CMsg()->enterpoint_resp());
	}*/
	else if(stRsp.getS2CMsg()->has_shopguide_resp())
	{
		if(stRsp.getS2CMsg()->shopguide_resp().ret() == 0)
		{
			sgUserObj()->doAddGuideDiamond(stRsp.getS2CMsg()->shopguide_resp().diamond());  
			sgUserObj()->addShopGuideNum(1); 
		} 
	}
	else if (stRsp.getS2CMsg()->has_glyphguide_resp())
	{
		if (stRsp.getS2CMsg()->glyphguide_resp().ret() == 0)
		{
			sgUserObj()->doAddGuideGlyphStuff(stRsp.getS2CMsg()->glyphguide_resp().giftid());
			sgUserObj()->addGlyphStuffGuideNum(1);
		}
	}
	else if (stRsp.getS2CMsg()->has_pvecheckaward_resp())
	{
		if (stRsp.getS2CMsg()->pvecheckaward_resp().has_haveaward())
		{
			sgUserObj()->doGetNewBieReward(stRsp.getS2CMsg()->pvecheckaward_resp());
		}
	}
}

void UserProxy::sendUpdateCheckPoint(int id, unsigned star, int passTime, bool isWin ,int openCheckPt)
{
	unsigned int uCmd = fgame::CS_CMD_PVE_CHECK_POINT_SAVE;
	NetPack msgPack;

	MsgHead& msgHead = *(msgPack.getMsgHead());
	sgNetManager()->GenRqstMsgHead(msgHead, uCmd);

	fgame::CSSaveCheckPtDataReq* pRqst = msgPack.getC2SMsg()->mutable_savecheckptdata_req();
	if (openCheckPt != 0)
	{
		pRqst->set_opencheckptid(openCheckPt);
	}
	fgame::CheckPtData* pCheckPtData = pRqst->mutable_cpdata();
	pCheckPtData->set_id(id);
	pCheckPtData->set_stars(star);
	pCheckPtData->set_battletime(passTime);
	pCheckPtData->set_win(isWin);
	sgNetManager()->SendPkg(msgPack,true,true);

}

void UserProxy::sendGetGift(int id,int type)
{
	unsigned int uCmd = fgame::CS_CMD_PVE_CHECK_POINT_RETRIVE_REWARD;
	NetPack msgPack;

	MsgHead& msgHead = *(msgPack.getMsgHead());
	sgNetManager()->GenRqstMsgHead(msgHead, uCmd);

	fgame::CSRetriveCheckPtRewardReq* pRqst = msgPack.getC2SMsg()->mutable_retrivecheckptreward_req();

	pRqst->set_id(id);
	pRqst->set_rewardtype(type);
	sgNetManager()->SendPkg(msgPack,true,true);
}

void UserProxy::sendGetNewBieReward()
{
	unsigned int uCmd = fgame::CS_CMD_PVE_CHECK_AWARD;
	NetPack msgPack;

	MsgHead& msgHead = *(msgPack.getMsgHead());
	sgNetManager()->GenRqstMsgHead(msgHead, uCmd);

	fgame::CSPveCheckAwardReq* pRqst = msgPack.getC2SMsg()->mutable_pvecheckaward_req();
	sgNetManager()->SendPkg(msgPack);
}

void UserProxy::sendGetCheckPtData()
{
	unsigned int uCmd = fgame::CS_CMD_PVE_CHECK_POINT_LIST;
	NetPack msgPack;

	MsgHead& msgHead = *(msgPack.getMsgHead());
	sgNetManager()->GenRqstMsgHead(msgHead, uCmd);

	fgame::CSGetCheckPtDataReq* pRqst = msgPack.getC2SMsg()->mutable_getcheckptdata_req();

	sgNetManager()->SendPkg(msgPack);
}
	
void UserProxy::sendEnterPoint(int pointId)
{
	unsigned int uCmd = fgame::CS_CMD_ENTER_POINT;
	NetPack msgPack;

	MsgHead& msgHead = *(msgPack.getMsgHead());
	sgNetManager()->GenRqstMsgHead(msgHead, uCmd);

	fgame::CSEnterPointReq* pRqst = msgPack.getC2SMsg()->mutable_enterpoint_req();
	pRqst->set_pointid(pointId);
	sgNetManager()->SendPkg(msgPack,true,false);
}

void UserProxy::sendGetNewBieList()
{
	unsigned int uCmd = fgame::CS_CMD_GET_NEWBIE_LIST;
	NetPack msgPack;

	MsgHead& msgHead = *(msgPack.getMsgHead());
	sgNetManager()->GenRqstMsgHead(msgHead, uCmd);

	fgame::CSGetNewBieListReq* pRqst = msgPack.getC2SMsg()->mutable_getnewbielist_req();

	sgNetManager()->SendPkg(msgPack);
}

void UserProxy::sendFinishGuideID(int id, bool isComplete)
{ 
	unsigned int uCmd = fgame::CS_CMD_SET_NEWBIE;
	NetPack msgPack; 
	MsgHead& msgHead = *(msgPack.getMsgHead());
	sgNetManager()->GenRqstMsgHead(msgHead, uCmd); 
	fgame::CSSetNewBieReq* pReq = msgPack.getC2SMsg()->mutable_setnewbie_req();
	pReq->add_newbieid(id);
	if (isComplete)
	{
		pReq->set_iscomplete(true);
	}
	sgNetManager()->SendPkg(msgPack, true, false, true);
}


void UserProxy::sendFinishGuideIDList(std::vector<int > idList, bool isComplete)
{
	unsigned int uCmd = fgame::CS_CMD_SET_NEWBIE;
	NetPack msgPack;
	MsgHead& msgHead = *(msgPack.getMsgHead());
	sgNetManager()->GenRqstMsgHead(msgHead, uCmd);
	fgame::CSSetNewBieReq* pReq = msgPack.getC2SMsg()->mutable_setnewbie_req();

	std::vector<int >::iterator iter = idList.begin();
	for (; iter != idList.end(); iter++)
	{
		int id = *iter;
		pReq->add_newbieid(id);
	} 
	if (isComplete)
	{
		pReq->set_iscomplete(true);
	}
	sgNetManager()->SendPkg(msgPack, true, false, true);
}


void UserProxy::sendGuideDiamonRqst(int count)
{
	unsigned int uCmd = fgame::CS_CMD_SHOP_GUIDE;
	NetPack msgPack; 
	MsgHead& msgHead = *(msgPack.getMsgHead());
	sgNetManager()->GenRqstMsgHead(msgHead, uCmd); 
	fgame::CSShopGuideReq* pReq = msgPack.getC2SMsg()->mutable_shopguide_req(); 
	pReq->set_diamond(count); 
	sgNetManager()->SendPkg(msgPack, true, true);
}


void UserProxy::sendGuideGlyphStuffRqst()
{
	unsigned int uCmd = fgame::CS_CMD_GLYPH_GUIDE_ADDSTUFF;
	NetPack msgPack;
	MsgHead& msgHead = *(msgPack.getMsgHead());
	sgNetManager()->GenRqstMsgHead(msgHead, uCmd);
	fgame::CSGlyphGuideReq* pReq = msgPack.getC2SMsg()->mutable_glyphguide_req(); 
	sgNetManager()->SendPkg(msgPack, true, true);
}
