#include "NoticeProxy.h"
#include "net/UserObj.h"
#include "net/NetManager.h"
#include "EnvSet.h"

#include "ui/dialogs/GenerlDialog.h"
#include "channel/ChannelApi.h"
#include "game/ui/PageFunction.h"

#include "NoticeControl.h"

NoticeProxy::NoticeProxy()
{
	sgNetManager()->RegCmdListener(fgame::CS_CMD_NOTICE, this);
	sgNetManager()->RegCmdListener(fgame::CS_CMD_STOP_KICK_PLAYLER, this);
}

NoticeProxy::~NoticeProxy()
{

}

bool NoticeProxy::fault(const fgame::CSErrorMsg &err)
{
	if (err.errcode() == 10001)
	{
		CCLOG("[trace]NoticeProxy::fault !");
	}
	return true;
}

void NoticeProxy::DoRsp(NetPack& stRsp)
{
	unsigned long msgID = stRsp.getMsgHead()->mMsgID;
	if (stRsp.getS2CMsg()->has_error_msg())
	{
		CCLOG("[trace]NoticeProxy::DoRsp error!");		
	}
	else if(stRsp.getS2CMsg()->has_notice_req())
	{
		const ::fgame::CSNoticeReq& noticeRsp = stRsp.getS2CMsg()->notice_req();
	
		NoticeInfo* pInfo = new NoticeInfo();
		pInfo->autorelease();

		pInfo->setIsUrgent(noticeRsp.serious());
		pInfo->setNoticeContent(noticeRsp.content());

		if (noticeRsp.has_count())
		{
			pInfo->setNoticeCount(noticeRsp.count());
		}

		if (noticeRsp.has_timegap())
		{
			pInfo->setTimeGap(noticeRsp.timegap());
		}

		if (noticeRsp.has_title())
		{
			pInfo->setTitle(noticeRsp.title());
		}

		sgNoticeControl()->addNotice(noticeRsp.noticetype(),pInfo);
	}
	else if (stRsp.getS2CMsg()->has_stopkickplayer_req())
	{
		sgNoticeControl()->addStopServerNotice();
		
	}
}
