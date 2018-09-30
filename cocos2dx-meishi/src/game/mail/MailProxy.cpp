#include "MailProxy.h"
#include "data/MailData.h"
#include "net/NetManager.h"
#include "MessageManager.h"

MailProxy::MailProxy(void):mMailPanel(NULL)
{
	sgNetManager()->RegCmdListener(fgame::CS_CMD_GET_MAIL, this);
	sgNetManager()->RegCmdListener(fgame::CS_CMD_DEL_MAIL, this);
	sgNetManager()->RegCmdListener(fgame::CS_CMD_NOTICE_MAIL, this);

}


MailProxy::~MailProxy(void)
{
	CC_SAFE_RELEASE(mMailPanel);
}


 void MailProxy::DoRsp(NetPack& stRsp)
 {
	 if (stRsp.getS2CMsg()->has_error_msg())
	 {
		 CCLOG("[trace]ActProxy::DoRsp error!");

	 }
	 else if (stRsp.getS2CMsg()->has_getmail_resp())
	 {
		doGetMail(stRsp.getS2CMsg()->getmail_resp());
	}else if (stRsp.getS2CMsg()->has_delmail_resp())
	{
		doDeleMail(stRsp.getS2CMsg()->delmail_resp());
	}else if (stRsp.getS2CMsg()->has_noticemail_req())
	{
		doNoticeMail(stRsp.getS2CMsg()->noticemail_req());
	}
 }

void MailProxy::sendGetMail(int getNum)
{
	unsigned int uCmd = fgame::CS_CMD_GET_MAIL;
	NetPack msgPack;

	MsgHead& msgHead = *(msgPack.getMsgHead());
	sgNetManager()->GenRqstMsgHead(msgHead, uCmd);

	fgame::CSGetMailReq* pRqst = msgPack.getC2SMsg()->mutable_getmail_req();
	pRqst->set_getnum(getNum);
	sgNetManager()->SendPkg(msgPack);
}

void MailProxy::sendDelMail(std::vector<int> idVec)
{
	mDelMailVec = idVec;
	unsigned int uCmd = fgame::CS_CMD_DEL_MAIL;
	NetPack msgPack;

	MsgHead& msgHead = *(msgPack.getMsgHead());
	sgNetManager()->GenRqstMsgHead(msgHead, uCmd);

	fgame::CSDelMailReq* pRqst = msgPack.getC2SMsg()->mutable_delmail_req();
	for (auto it = idVec.begin(); it!=idVec.end(); it++)
	{
		pRqst->add_id(*it);
	}
	sgNetManager()->SendPkg(msgPack,true,false);
}

void MailProxy::doGetMail(const fgame::CSGetMailResp& getMail)
{
	if (getMail.has_newmail() == false && sgMailData()->getMailNo() != getMail.mailno())
	{
		sgMailData()->setMailNo(getMail.mailno());
	}else
	if (getMail.has_newmail() && getMail.newmail()&&sgMailData()->getMailNo()!=getMail.mailno())
	{
		sgMailData()->addNewMail(getMail);
	}
	

	if (sgMailData()->getMailSize() >0)
	{
		if (mMailPanel && mMailPanel->isShow())
		{
			mMailPanel->updateInfo();
		}
		sgMailData()->setNewMail(true);
		sgMessageManager()->sendMessage(EMT_HONGDIAN_TIP,StringUtils::format("%d 1",MSG_FUC_MAIL));
	}else
	{
		sgMailData()->setNewMail(false);
		sgMessageManager()->sendMessage(EMT_HONGDIAN_TIP,StringUtils::format("%d 0",MSG_FUC_MAIL));
	}
}

void MailProxy::doDeleMail(const fgame::CSDelMailResp& delMail)
{	
	//remove mail
	sgMailData()->delMail(mDelMailVec);
	mDelMailVec.clear();
	//领取邮件表现///领取的内容
	std::vector<int> pDelStateVec;
	for (int i = 0; i<delMail.ret_size(); i++)
	{
		pDelStateVec.push_back(delMail.ret(i));
	}
	mMailPanel->delCell(pDelStateVec);
}

void MailProxy::doNoticeMail(const fgame::CSNoticeMailReq& NoticeMail)
{
	sgMessageManager()->sendMessage(EMT_HONGDIAN_TIP,StringUtils::format("%d 1",MSG_FUC_MAIL));
	sgMailData()->setNewMail(true);
	if (mMailPanel && mMailPanel->isShow())
	{
		sendGetMail(sgMailData()->getMailNo());
	}
}

void MailProxy::checkMailTip()
{
	if (sgMailData()->getMailSize()>0)
	{
		sgMessageManager()->sendMessage(EMT_HONGDIAN_TIP, StringUtils::format("%d 1", MSG_FUC_MAIL));
	}
	else
	{
		sgMessageManager()->sendMessage(EMT_HONGDIAN_TIP, StringUtils::format("%d 0", MSG_FUC_MAIL));
	}
}
