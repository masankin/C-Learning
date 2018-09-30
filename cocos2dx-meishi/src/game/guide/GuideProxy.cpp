#include "GuideProxy.h"
#include "net/NetManager.h"
#include "net/UserObj.h"
GuideProxy::GuideProxy()
{
	sgNetManager()->RegCmdListener(fgame::CS_CMD_SET_NEWBIE_OSS,this);
}

GuideProxy::~GuideProxy()
{

}

void GuideProxy::DoRsp(NetPack& stRsp)
{
	if (stRsp.getS2CMsg()->has_error_msg())
	{
		CCLOG("[trace]ActProxy::DoRsp error!");

	}
	else if(stRsp.getS2CMsg()->has_setnewbieoss_resp())
	{
		CCLOG("[trace]");
	}

}

bool GuideProxy::fault(const fgame::CSErrorMsg &err)
{
	return true;
	//do nothing
}

void GuideProxy::sendGuide2Oss(int id)
{ 
	unsigned int uCmd = fgame::CS_CMD_SET_NEWBIE_OSS;
	NetPack msgPack;

	MsgHead& msgHead = *(msgPack.getMsgHead());
	sgNetManager()->GenRqstMsgHead(msgHead, uCmd);

	fgame::CSSetNewBieOssReq* pRegRqst = msgPack.getC2SMsg()->mutable_setnewbieoss_req();

	pRegRqst->set_newbieid(id);

	sgNetManager()->SendPkg(msgPack);
}

