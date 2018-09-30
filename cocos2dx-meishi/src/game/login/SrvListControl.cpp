#include "SrvListControl.h"
#include "MessageManager.h"
#include "game/ui//PageFunction.h"
#include "stl_helper.h"
#include "game/utils/GameDefault.h"
SrvListControl::SrvListControl(void)
{	

}


SrvListControl::~SrvListControl(void)
{
	
}
void SrvListControl::request(const char* url, const char* tag, void* userData,
							Ref* pTarget, SEL_CallFuncND callbackFunc)
{
	HttpRequest* request = new HttpRequest();
	request->setUrl(url);
	CCLOG("%s", url);
	request->setRequestType(HttpRequest::Type::GET);
	if(pTarget == NULL){
		request->setResponseCallback(this, callfuncND_selector(SrvListControl::onRequestSuccess));
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
void SrvListControl::onRequestSuccess(cocos2d::CCNode *sender, void *data)
{ 
	FileUtils::getInstance()->addSearchPath(FileUtils::sharedFileUtils()->getWritablePath().c_str(), true);

	HttpResponse *response = (HttpResponse*)data;
	int code = response->getResponseCode();
	std::string urlName = response->getHttpRequest()->getTag();
	if (urlName.compare(REQ_SERVER_LIST) == 0)
	{
		if (code == 200){
			std::vector<char> *buffer = response->getResponseData();
			string strBuffer = "";
			for (unsigned int i = 0; i < buffer->size(); i++)
			{
				strBuffer += (*buffer)[i];
			}
			rapidjson::Document json;
			json.Parse<0>(strBuffer.c_str());

			int ver = 0;
			if (json.HasParseError())
			{
				return;
			}
			else
			{
				mSrvList.clear();

				for (auto iter = json.MemberonBegin(); iter != json.MemberonEnd(); ++iter)
				{
					SrvInfo* pSrvInfo = new SrvInfo();
					if (iter->name.IsString())
						pSrvInfo->mName = iter->name.GetString();
					if (iter->value.IsString())
						pSrvInfo->mIp = iter->value.GetString();

					mSrvList.push_back(pSrvInfo);

				}

				std::string curSrvName = USER_DEFAULT(USER_SERVER_NAME);
				std::string curSrvIp = USER_DEFAULT(USER_SERVER_IP);
				if (curSrvName.empty()) {

					sgPageFunction()->gotoFunction(MSG_FUC_CHAGE_SERVER);
				}
				else if (checkIpInSrvList(curSrvIp) < 0)
				{
					sgPageFunction()->gotoFunction(MSG_FUC_CHAGE_SERVER);
				}
				else
				{
					// 设置当前服务器名字 发消息
					int idx = checkIpInSrvList(curSrvIp);
					if (idx >= 0)
						sgMessageManager()->sendMessage(EMT_CHANGE_SERVER, StringUtils::format("%d", idx));

				}
			}
		}
	}

}


void SrvListControl::requestSrvList()
{
	request(SRV_LIST_URL, REQ_SERVER_LIST);
}

int SrvListControl::checkIpInSrvList(std::string strIp)
{
	int ret = 0;
	vector<SrvInfo*>::iterator iter = mSrvList.begin();

	for (; iter != mSrvList.end(); iter++)
	{
		if ((*iter)->mIp.compare(strIp) == 0)
		{
			return ret;
		}
		ret++;

	}
	return -1;
}

void SrvListControl::deleteSrvList()
{
	if (mSrvList.size() > 0)
		SAFE_RELEASE_VECTOR(mSrvList);
}
