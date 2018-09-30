#include "NoticeControl.h"
#include "cocostudio/CocoStudio.h"
#include "battle/BattleControl.h"
#include "ui/dialogs/GenerlDialog.h"
#include "game/ui/PageFunction.h"
#include "guide/GuideManager.h"
#include "data/TableManager.h"

NoticeInfo::NoticeInfo(void)
	:mCount(1),mContent(""),mIsUrgent(0),mTimeGap(0)
{

}

NoticeInfo::~NoticeInfo()
{

}

NoticeControl::NoticeControl(void)
	:mNoticeTips(0),
	mNoticeLampGap(0),
	mNewNotice(false), mNoticeDialog(NULL), 
	mActInformDialog(NULL),
	mNewActInform(true)
{	
	mNoticeArray = Array::create();
	mNoticeArray->retain();

	mNoticeLampArray = Array::create();
	mNoticeLampArray->retain();

	Node::onEnter();

	schedule(schedule_selector(NoticeControl::updateNotice),1.0);
}


NoticeControl::~NoticeControl(void)
{
	if (mNoticeTips)
	{
		mNoticeTips->mTipsLabel->stopAllActions();
	}
	CC_SAFE_RELEASE(mNoticeLampArray);
	CC_SAFE_RELEASE(mNoticeLampGap);
	CC_SAFE_RELEASE(mNoticeArray);
	unschedule(schedule_selector(NoticeControl::updateNotice));
	Node::onExit();
}

void NoticeControl::updateNotice(float dt)
{
	if(!mNoticeLampGap)
	{
		return;
	}
	Ref* pObject = NULL;
	CCARRAY_FOREACH(mNoticeLampGap, pObject)
	{
		NoticeInfo* pObjNo = dynamic_cast<NoticeInfo*>(pObject);
		if (pObjNo == NULL) continue;

		pObjNo->setDeyTime(pObjNo->getDeyTime() + dt);
		if (pObjNo->getTimeGap() > pObjNo->getDeyTime()) continue;

		if(!pObjNo->getIsUrgent() || mNoticeLampArray->count() < 2)
		{
			mNoticeLampArray->addObject(pObjNo);
		}
		else
		{
			mNoticeLampArray->insertObject(pObjNo,1);
		}
		pObjNo->setNoticeCount(pObjNo->getNoticeCount() - 1);
		pObjNo->setDeyTime(0);
		showNoticeTip();
		if (pObjNo->getNoticeCount() < 2)
		{
			mNoticeLampGap->removeObject(pObject);	
			break;
		}
	}
}

void NoticeControl::addNotice(int _type,NoticeInfo* pInfo)
{
	if (_type == 1)
	{
		if (!pInfo->getIsUrgent())
		{
			mNoticeLampArray->addObject(pInfo);
		}
		else
		{
			if (mNoticeLampArray->count() < 2)
				mNoticeLampArray->addObject(pInfo);
			else
				mNoticeLampArray->insertObject(pInfo, 1);		

			if (pInfo->getIsUrgent() == 2)//&& sgBattleControl()->getIsPvp()
			{
				GenerlDialog::show(sgTextHelper(TEXT_COMM_TIPS), pInfo->getNoticeContent(), CC_CALLBACK_1(NoticeControl::onStopCallBack, this));
			}
		}
		if (pInfo->getNoticeCount() > 1)
		{
			if(!mNoticeLampGap)
			{
				mNoticeLampGap = Array::create();
				mNoticeLampGap->retain();
			}
			mNoticeLampGap->addObject(pInfo);
		}
		showNoticeTip();
	}
	else
	{
		mNoticeArray->insertObject(pInfo,0);
		if (pInfo->getIsUrgent())
		{		
			Window* pWin = Window::getWindow("NoticeDialog.csb");
			if (pWin)
				mNoticeDialog = dynamic_cast<NoticeDialog*>(pWin);
			else
				mNoticeDialog = NULL;
			if(mNoticeDialog == NULL)
			{
				mNoticeDialog = dynamic_cast<NoticeDialog*>(Window::Create("NoticeDialog.csb"));
			}
			else
			{
				mNoticeDialog->setContent(pInfo->getTitle(),pInfo->getNoticeContent());
			}
			mNoticeDialog->setShow(true);
		}
		else
		{
			setNewNotice(true);
		}
	}
}

bool NoticeControl::showNotice(bool force)
{
	if(mNoticeArray->count()>0)
	{
		if(sgGuideManager()->getIsRunning())
		{
			return false;
		}

		if (!force && !getNewNotice()) return false;

		Window* pWin = Window::getWindow("NoticeDialog.csb");
		if (pWin)
			mNoticeDialog = dynamic_cast<NoticeDialog*>(pWin);
		else
			mNoticeDialog = NULL;
		if(mNoticeDialog == NULL)
			mNoticeDialog = dynamic_cast<NoticeDialog*>(Window::Create("NoticeDialog.csb"));

		mNoticeDialog->setShow(true);
		sgPageFunction()->enableOptionButton(false);

		if (getNewNotice() && mNoticeArray->count() > 1)
		{
			NoticeInfo* pInfo = dynamic_cast<NoticeInfo*>(mNoticeArray->objectAtIndex(0));
			if (pInfo)
			{
				mNoticeDialog->setContent(pInfo->getTitle(),pInfo->getNoticeContent());
			}
		}	
		setNewNotice(false);
		return true;
	}
	return false;
}


bool NoticeControl::showActInform()
{
	if (mNewActInform == false) return false;

	if (sgGuideManager()->getIsRunning()) return false;
	if (Window::getShowingWindowSize() == 0)
	{
		sgPageFunction()->gotoFunction(MSG_FUC_ACTINFORM, "ActInform.csb");
	}
	

	mNewActInform = false;

	return true;
}

void NoticeControl::showNoticeTip()
{
	if (!mNoticeTips)
	{	
		mNoticeTips = NoticeTips::create();
		Director::getInstance()->getNotificationNode()->addChild(mNoticeTips);
		Size _winSize = Director::getInstance()->getWinSize();
		mNoticeTips->setPosition(ccp(_winSize.width * 0.5,_winSize.height * 0.92));
	}
	mNoticeTips->startNotice();

}

void NoticeControl::addLocalNotice()
{
	sgTableManager()->loadNoticeTable();
	int num  = sgTableManager()->getNoticeDataVec()->size();
	if(num <= 0)  return;

	setNewNotice(true);

	for(int i = 0; i < num ; i++)
	{
		NoticeData* pData = sgTableManager()->getNoticeDataVec()->at(i);
		if(!pData) continue;

		NoticeInfo* pNotice = new NoticeInfo();
		if (!pNotice) continue;
		pNotice->autorelease();
		std::string noticeStr = pData->getNoticeContent();
		//noticeStr.replace("#",' ');
		while(true)   {     
			string::size_type   pos(0);     
			if( (pos=noticeStr.find("#"))!=string::npos )     
				noticeStr.replace(pos,1," ");     
			else   break;     
		}  
		pNotice->setIsUrgent(false);
		pNotice->setNoticeContent(noticeStr);
		pNotice->setTitle(pData->getTitle());
		pNotice->setNoticeCount(pData->getNoticeCount());

		if (pData->getID() == 1)
		{
			mNoticeArray->addObject(pNotice);
		}
		//else
		//{
		//	mNoticeLampArray->addObject(pNotice);
		//}	
	}	
}


void NoticeControl::request(const char* url, const char* tag, void* userData,
							Ref* pTarget, SEL_CallFuncND callbackFunc)
{
	HttpRequest* request = new HttpRequest();
	request->setUrl(url);
	request->setRequestType(HttpRequest::Type::GET);
	if(pTarget == NULL){
		request->setResponseCallback(this, callfuncND_selector(NoticeControl::onRequestSuccess));
	}else{
		request->setResponseCallback(pTarget, callbackFunc);
	}
	request->setTag(tag);
	request->setUserData(userData);
	HttpClient::getInstance()->setTimeoutForConnect(8);
	HttpClient::getInstance()->setTimeoutForRead(8);
	HttpClient::getInstance()->send(request);

	request->release();
}

// 请求返回
void NoticeControl::onRequestSuccess(cocos2d::CCNode *sender, void *data){ 

	FileUtils::getInstance()->addSearchPath(FileUtils::sharedFileUtils()->getWritablePath().c_str(), true);

	HttpResponse *response = (HttpResponse*)data;
	int code = response->getResponseCode();

	std::string fileName = response->getHttpRequest()->getTag();
	/*if (fileName.compare(REQUEST_NOTICE) == 0)
	{
		if (code == 200){

			std::vector<char> *buffer = response->getResponseData();
			char* strBuffer = new char[buffer->size() + 1];
			for (unsigned int i = 0; i < buffer->size(); i++)
			{
				strBuffer[i] = (*buffer)[i];
			}
			strBuffer[buffer->size()] = 0;

			// 写文件

			std::string fileFullPath = FileUtils::sharedFileUtils()->getWritablePath() + REQUEST_NOTICE;

			FILE* f = fopen(fileFullPath.c_str(), "wb");
			fwrite(strBuffer, 1, buffer->size(), f);
			fclose(f);
			delete[] strBuffer;

			addLocalNotice();
		}
		else
		{
			if (FileUtils::getInstance()->isFileExist(REQUEST_NOTICE))
			{
				addLocalNotice();
			}
		}
		requestNotice(REQUEST_ACTINFORM);
	}
	else*/ 
	if (fileName.compare(REQUEST_ACTINFORM) == 0)
	{ 
		if (code == 200){

			std::vector<char> *buffer = response->getResponseData();
			char* strBuffer = new char[buffer->size() + 1];
			for (unsigned int i = 0; i < buffer->size(); i++)
			{
				strBuffer[i] = (*buffer)[i];
			}
			strBuffer[buffer->size()] = 0;

			// 写文件

			std::string fileFullPath = FileUtils::sharedFileUtils()->getWritablePath() + REQUEST_ACTINFORM;

			FILE* f = fopen(fileFullPath.c_str(), "wb");
			fwrite(strBuffer, 1, buffer->size(), f);
			fclose(f);
			delete[] strBuffer;

			sgTableManager()->loadActInformTable();
			sgUserObj()->updateActInformData();
		}
		else
		{
			if (FileUtils::getInstance()->isFileExist(REQUEST_ACTINFORM))
			{
				sgTableManager()->loadActInformTable();
				sgUserObj()->updateActInformData();
			}
		}
	}

}


void NoticeControl::requestNotice(std::string name)
{
	std::string strFilePath = "update.json";
	Data data = FileUtils::getInstance()->getDataFromFile(strFilePath);
	unsigned char* pBytes = data.getBytes();
	size_t size = data.getSize();

	std::string load_str = std::string((const char*)pBytes, size);
	rapidjson::Document json;
	json.Parse<0>(load_str.c_str());
	//int ver = 0;
	if (json.HasParseError()) {
		CCLOG("[error]json file %s,parse error %s", strFilePath.c_str(), json.GetParseError());
		return ;
	}
	else {
		const rapidjson::Value& subVal = cocostudio::DictionaryHelper::getInstance()->getSubDictionary_json(json, "config");

		std::string _url = cocostudio::DictionaryHelper::getInstance()->getStringValue_json(subVal, "url");

		request(__String::createWithFormat("%s%s",_url.c_str(),name.c_str())->getCString(), name.c_str());

	}

}

void NoticeControl::onCallBack(cocos2d::Ref* ref)
{
	GenerlDialog::hide();
	//sgPageFunction()->gotoFunction(MSG_FUC_MAIN_LEVEL);
}

void NoticeControl::addStopServerNotice()
{
	GenerlDialog::show(sgTextHelper(TEXT_COMMTIPS_TIPS), sgTextHelper(TEXT_SERVER_WILL_STOP), CC_CALLBACK_1(NoticeControl::onStopCallBack, this), false, false);
}

void NoticeControl::onStopCallBack(cocos2d::Ref* ref)
{
	Director::getInstance()->getNotificationNode()->onExit();
	Director::getInstance()->end();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)			
	exit(0);
#endif
}
