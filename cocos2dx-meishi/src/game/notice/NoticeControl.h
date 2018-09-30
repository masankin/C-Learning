#pragma once
#include "cocos2d.h"
#include "Constant.h"
#include "NoticeDialog.h"
#include "ActInform.h"
#include "NoticeTips.h"
#include "NoticeProxy.h"

#include "cocos2d.h"
#include "extensions/cocos-ext.h"
#include "network/HttpClient.h"
#include "data/Tables.h"

USING_NS_CC;
USING_NS_CC_EXT;
using namespace network;

class NoticeDialog;
class NoticeInfo:public Ref
{
public:
	NoticeInfo();
	~NoticeInfo();

protected:
	CC_SYNTHESIZE(unsigned short, mCount, NoticeCount);//广播次数
	CC_SYNTHESIZE(std::string, mContent, NoticeContent);
	CC_SYNTHESIZE(unsigned char, mIsUrgent, IsUrgent);//1 紧急
	CC_SYNTHESIZE(unsigned short, mTimeGap, TimeGap);//1 广播间隔秒
	CC_SYNTHESIZE(std::string, mTitle, Title);
	CC_SYNTHESIZE(unsigned short, mDeyTime, DeyTime);//间隔持续时间
};

class NoticeControl:public Node
{
public:
	NoticeControl(void);
	~NoticeControl(void);

	NoticeProxy* getNoticeProxy() { &mNoticeProxy; };
	NoticeDialog* getNoticeDialog() { return mNoticeDialog;}
	CC_SYNTHESIZE(ActInform*, mActInformDialog, ActInformDialog);
	void request(const char* url, const char* tag, void* userData=NULL,
		Ref* pTarget=NULL, SEL_CallFuncND callbackFunc=NULL);
	void onRequestSuccess(Node *sender, void *data);

	void updateNotice(float dt);

	void addNotice(int _type,NoticeInfo* pInfo);

	bool showNotice(bool force = false);

	bool showActInform();

	void showNoticeTip();

	void addLocalNotice();

	void requestNotice(std::string name);

	void onCallBack(cocos2d::Ref* ref);

	void onStopCallBack(cocos2d::Ref* ref);

	void addStopServerNotice();
private:
	NoticeProxy mNoticeProxy;
	NoticeTips* mNoticeTips;
	NoticeDialog* mNoticeDialog;
	CC_SYNTHESIZE(bool,mNewNotice,NewNotice); //有新的普通公告在主界面弹出 紧急公告会及时弹出
	CC_SYNTHESIZE(bool, mNewActInform, NewActInform); //有新的普通公告在主界面弹出 紧急公告会及时弹出

	CC_SYNTHESIZE_READONLY(Array*,mNoticeArray,NoticeArray);
	CC_SYNTHESIZE_READONLY(Array*,mNoticeLampArray,NoticeLampArray);
	CC_SYNTHESIZE_READONLY(Array*,mNoticeLampGap,NoticeLampGap);

};

#define sgNoticeControl() Singleton<NoticeControl>::Get()
