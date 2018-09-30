#include "ActivationControl.h"
#include "net/UserObj.h"
#include "net/NetManager.h"
#include "core/MessageManager.h"
#include "ui/dialogs/GiftAwardDialog.h"


ActivationProxy::ActivationProxy()
{
	sgNetManager()->RegCmdListener(fgame::CS_CMD_USE_GIFT_CODE, this);
}

ActivationProxy::~ActivationProxy()
{

}

bool ActivationProxy::fault(const fgame::CSErrorMsg &err)
{
	if (err.errcode() == 10001)
	{
		CCLOG("[trace]ActivationProxy::fault !");
	}
	return true;
}

void ActivationProxy::DoRsp(NetPack& stRsp)
{
	unsigned long msgID = stRsp.getMsgHead()->mMsgID;
	if (stRsp.getS2CMsg()->has_error_msg())
	{
		CCLOG("[trace]ActivationProxy::DoRsp error!");		
	}
	else if(stRsp.getS2CMsg()->has_usegiftcode_resp())
	{
		//ÁìÈ¡
		const ::fgame::CSUseGiftCodeResp& pRsp = stRsp.getS2CMsg()->usegiftcode_resp();
		if (pRsp.ret() == 0)
		{
			if (pRsp.reward() > 0)
				GiftAwardDialog::showDialogByGiftId(pRsp.reward());
			else if (pRsp.reward() == 0)
				sgMessageManager()->sendMessage(EMT_ACTIVATION_TIP, StringUtils::format("%d", TEXT_ACTIVATION_EMAIL_SUCCESS));
		}
		else
		{
			sgMessageManager()->sendMessage(EMT_ACTIVATION_TIP, StringUtils::format("%d", pRsp.ret()));
		}

	}
}


void ActivationProxy::sendGiftCodeRqst(std::string code)
{

	unsigned int uCmd = fgame::CS_CMD_USE_GIFT_CODE;
	NetPack msgPack;

	MsgHead& msgHead = *(msgPack.getMsgHead());
	sgNetManager()->GenRqstMsgHead(msgHead, uCmd);

	::fgame::CSUseGiftCodeReq* pWinRqst = msgPack.getC2SMsg()->mutable_usegiftcode_req();

	pWinRqst->set_giftcode(code);

	sgNetManager()->SendPkg(msgPack);
}


ActivationControl::ActivationControl(void)
{
	mActivationDialog = NULL;
	Node::onEnter();
	
}


ActivationControl::~ActivationControl(void)
{
	Node::onExit();
}


void ActivationControl::sendGiftCodeRqst(std::string code)
{
	mActivationProxy.sendGiftCodeRqst(code);
}

