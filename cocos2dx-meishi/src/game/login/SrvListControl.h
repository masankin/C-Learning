#pragma once
#include "cocos2d.h"
#include "Constant.h"

#include "cocos2d.h"
#include "extensions/cocos-ext.h"
#include "network/HttpClient.h"
#include "data/Tables.h"

USING_NS_CC;
USING_NS_CC_EXT;
using namespace network;

#define REQ_SERVER_LIST "ServerList"

class SrvListDialog;
class SrvInfo
{
public: 
	SrvInfo()
	{
		mName = "";
		mIp = "";
		mState = 0;
	}
	std::string mName;
	std::string mIp;
	int mState;
};
class SrvListControl:public Ref
{
public:
	SrvListControl(void);
	~SrvListControl(void);

	void request(const char* url, const char* tag, void* userData=NULL,
		Ref* pTarget=NULL, SEL_CallFuncND callbackFunc=NULL);
	void onRequestSuccess(Node *sender, void *data);

	void requestSrvList();

	void onCallBack(cocos2d::Ref* ref);

	int checkIpInSrvList(std::string strIp);

	void deleteSrvList();
	vector<SrvInfo*> getSrvList() { return mSrvList; }
	vector<SrvInfo*> mSrvList;

};

#define sgSrvListControl() Singleton<SrvListControl>::Get()
