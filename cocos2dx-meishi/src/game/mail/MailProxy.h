#pragma once
#include "BaseProxy.h"
#include "MailPanel.h"
#include "core/Singleton.h"

using namespace fgame;

class MailProxy: public BaseProxy
{
public:
	MailProxy(void);
	~MailProxy(void);

	CC_SYNTHESIZE(MailPanel*, mMailPanel, MailPanel);
	virtual void DoRsp(NetPack& stRsp);

	void sendGetMail(int getNum);
	void sendDelMail(std::vector<int>);

	void checkMailTip();
private:
	void doGetMail(const fgame::CSGetMailResp& getMail);
	void doDeleMail(const fgame::CSDelMailResp& delMail);
	void doNoticeMail(const fgame::CSNoticeMailReq& NoticeMail);


	std::vector<int> mDelMailVec;
};

#define sgMailProxy() Singleton<MailProxy>::Get()

