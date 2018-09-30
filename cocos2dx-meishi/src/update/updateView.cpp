#include "updateView.h"
#include "HttpProxy.h"
#include "UpdateHelper.h"

#include "EnvSet.h"

#include "GameJniHelper.h"
#include "game/ui/PageFunction.h"

#include "CCFileUtils.h"
#include "cocostudio/CocoStudio.h"
#include "cocostudio/ActionTimeline/CSLoader.h"

#include "data/TableManager.h"
#include "net/NetManager.h"
#include "ResManager.h"


using namespace cocostudio;
USING_NS_CC;

#define LoadingBg_Width (958)
#define LoadingAni_Tag (1000)

UpdateView::UpdateView() :m_fPercentage(0), m_bIsAllReady(false), m_bIsPlayLoading(false)//, mConfirmLayout(NULL)
{
	cocostudio::ArmatureDataManager::getInstance()->addArmatureFileInfo("ui/update/fla_loadingRes.xml");
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("ui/update/fla_loadingRes.plist");
};

UpdateView::~UpdateView()
{
	cocostudio::ArmatureDataManager::getInstance()->removeArmatureFileInfo("ui/update/fla_loadingRes.xml");
	CCTextureCache::getInstance()->removeTextureForKey("ui/update/fla_loadingRes.png");
	SpriteFrameCache::getInstance()->removeSpriteFramesFromFile("ui/update/fla_loadingRes.plist");
};

UpdateView* UpdateView::create(const std::string& csbFile)
{
	UpdateView* view = new (std::nothrow)UpdateView();
	if (view&& view->init(csbFile)){
		view->autorelease();
	}
	else
	{
		CC_SAFE_DELETE(view);
	}
	return view;
}

void UpdateView::checkUpdate(float dt) 
{
	std::string sErr;
	bool bNeedRestart = false;


	auto stat = UpdateMgrImp::getInstance()->GetStatus();
	switch (stat.stage)
	{
	case UpdateMgr::CHECK_VERSION:
		//CCLOG("[trace]UpdateResScreen::checkUpdate, CHECK_VERSION.");
		setNoticeWords(m_sErrString[eUpdateCheck].c_str());
		break;
	case UpdateMgr::WAIT_CONFIRM:

		showUpdateConfirm();
		break;
	case UpdateMgr::DOWNLOAD_RESOURCE:
	{
		CCLOG("[trace]UpdateMgr::DownloadFile, DOWNLOAD_RESOURCE. progress %d", stat.nowProgress);
		setPercentage(stat.nowProgress);
		setNoticeWords(m_sErrString[eUpdateDownload].c_str());
		playLoadingAni();
	}
		break;
	case UpdateMgr::EXTRACT_RESOURCE:
	{
		CCLOG("[trace]UpdateResScreen::checkUpdate, EXTRACT_RESOURCE.");
		setPercentage(stat.nowProgress);
		setNoticeWords(m_sErrString[eUpdateExtract].c_str());
	}
		break;
	case UpdateMgr::UPDATE_FAIL:
	{
		UpdateHelper::IntToString(sErr, stat.errorCode);
		sErr = m_sErrString[eUpdateFail] + sErr;
		setNoticeWords(sErr.c_str());
		stopLoadingAni();
	}
	break;
	case UpdateMgr::RESTART:
	{
		 if (!m_rootView->getChildByTag(1001))
		 {
			 Sprite *bg = Sprite::createWithSpriteFrameName("ui/Loading/loadingBg.png");
			 bg->setTag(1001);
			 bg->setPosition(m_rootView->getContentSize().width / 2, m_rootView->getContentSize().height / 2);
			 bg->setAnchorPoint(Point::ANCHOR_MIDDLE);
			 Sprite *text = Sprite::createWithSpriteFrameName("ui/Loading/lodaingText.png");
			 bg->addChild(text);
			 text->setPosition(bg->getContentSize().width / 2, bg->getContentSize().height / 2);
			 m_rootView->addChild(bg);
			 setPercentage(stat.nowProgress);
		 }

		 static float time = 0;
		 time += 0.1;
		 if (time >1)
	     {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
		   GameJniHelper::getInstance()->restartGame();
		   return;
#endif
		 }
	}

		break;
	case UpdateMgr::LOAD:
		{
			float progress = sgResManager()->getProgress();
			if (progress >= 1.0f)
			{
				//sgResManager()->doLoadOver();
				this->unschedule(schedule_selector(UpdateView::checkUpdate));
			}
			else
			{
				//setPercentage(progress * 100);
				//sErr = m_sErrString[eUpdateLoadRes] + sErr;
				//setNoticeWords(sErr.c_str());
				playLoadingAni();
			}
		}
		break;
	case UpdateMgr::DONE:
		{
			bNeedRestart = stat.needRestartGame;
			if (0 != stat.errorCode) {
				//出现错误了
				CCLOG("[warn]UpdateResScreen::checkUpdate, update done. error ret [%d], file [%s]", stat.errorCode, stat.filename.c_str());
			}

			CCLOG("[trace]UpdateResScreen::checkUpdate, update done. need restart ? %d", bNeedRestart);
			setNoticeWords(m_sErrString[eUpdateSuccess].c_str());

			//setPercentage(100);

			if (0 == stat.totalStep) {
				//没有东西要更新
				CCLOG("[trace]UpdateResScreen::checkUpdate, nothing to update.");
			}

			//this->unschedule(schedule_selector(UpdateView::checkUpdate));
			this->unschedule(schedule_selector(UpdateView::updateViewText));


			std::thread* pUpdateThread = UpdateMgrImp::getInstance()->getUpdateThread();
			if (pUpdateThread && pUpdateThread->joinable())
				pUpdateThread->detach();

			do 
			{

				UpdateMgrImp::getInstance()->SetStage(UpdateMgr::LOAD);
				// 触发资源加载，加载完成后跳转到主界面
				reloadAllCfg();
			} while (0);
		}
		break;
	case  UpdateMgr::DOWNLOAD_APK:
		playLoadingAni();
		setNoticeWords(m_sErrString[eUpadateAPK].c_str());
		break;
	case  UpdateMgr::DOWNLOAD_APKFAIL:
		setNoticeWords(m_sErrString[eUpadateAPKFail].c_str());
		break;
	case  UpdateMgr::RELOAD_APK:
		setNoticeWords(m_sErrString[eUpadateReload].c_str());
		break;
	default:
		break;
	}
}

void UpdateView::reloadAllCfg() 
{
	cocos2d::FileUtils::getInstance()->purgeCachedEntries();

	sgResManager()->syncLoadLua();
	sgResManager()->asyncLoadCsvData();
	sgResManager()->syncLoadData();
}

void UpdateView::onClickDialogRestartBtn() 
{
	CCLOG("[trace]now restart game");


}

void UpdateView::onClickDialogEnterBtn() 
{
	CCLOG("[trace]now enter game");


}

bool UpdateView::init(const std::string& csbFile)
{
	if (! Layout::init()){
		return false;
	}

	//加载界面显示配置
	loadTextInfo();


	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("update/update.plist");

	//ArmatureDataManager::getInstance()->addArmatureFileInfo("update/fla_loading.png", "update/fla_loading.plist", "update/fla_loading.xml");

	m_rootView = CSLoader::createNode(csbFile);
	if (NULL == m_rootView) return false;
	this->addChild(m_rootView);

	Sprite* loadingBack = dynamic_cast<Sprite*>(m_rootView->getChildByName("loadingBack"));
	loadingBack->setVisible(false);
	m_loadingText = dynamic_cast<Text*>(loadingBack->getChildByName("loadingText"));
	m_loadingImage = dynamic_cast<ui::ImageView*>(loadingBack->getChildByName("loadingFontImg"));
	m_statusText = dynamic_cast<Text*>(m_rootView->getChildByName("statusText"));
	dynamic_cast<Sprite*>(loadingBack->getChildByName("Sprite_1"))->setVisible(false);
	//m_animationNode = dynamic_cast<Node*> (m_rootView->getChildByName("animationNode"));
	//m_logoSpirte1 = dynamic_cast<Sprite*> (m_rootView->getChildByName("logo01"));
	//m_logoSpirte2 = dynamic_cast<Sprite*> (m_rootView->getChildByName("logo02"));
	//m_logoSpirte3 = dynamic_cast<Sprite*> (m_rootView->getChildByName("logo03"));

	//m_logoSpirte1->setVisible(true);
	//m_logoSpirte2->setVisible(false);
	//m_logoSpirte3->setVisible(false);

	//Armature *armature = Armature::create("fla_loading");
	//armature->getAnimation()->playWithIndex(0);
	//m_animationNode->addChild(armature);

	m_animationNode = dynamic_cast<Node*> (m_rootView->getChildByName("loadingAni"));

	return true;
}

bool UpdateView::loadTextInfo()
{
	std::string strFilePath = "update.json";
	Data data = FileUtils::getInstance()->getDataFromFile(strFilePath);
	unsigned char* pBytes = data.getBytes();
	size_t size = data.getSize();

	std::string load_str = std::string((const char*)pBytes, size);
	rapidjson::Document json;
	json.Parse<0>(load_str.c_str());
	int ver = 0;
	if (json.HasParseError()) {
		CCLOG("[error]json file %s,parse error %s", strFilePath.c_str(), json.GetParseError());
		return false;
	}
	else {
		const rapidjson::Value& subVal = DICTOOL->getSubDictionary_json(json, "message");

		m_sErrString[eUpdateCheck] = DICTOOL->getStringValue_json(subVal, "check");
		m_sErrString[eUpdateDownload] = DICTOOL->getStringValue_json(subVal, "download");
		m_sErrString[eUpdateExtract] = DICTOOL->getStringValue_json(subVal, "extract");
		m_sErrString[eUpdateFail] = DICTOOL->getStringValue_json(subVal, "fail");
		m_sErrString[eUpdateSuccess] = DICTOOL->getStringValue_json(subVal, "success");
		m_sErrString[eUpdateLoadRes] = DICTOOL->getStringValue_json(subVal, "loadRes");
		m_sErrString[eUpadateAPK] = DICTOOL->getStringValue_json(subVal, "downloadAPK");
		m_sErrString[eUpadateAPKFail] = DICTOOL->getStringValue_json(subVal, "downloadAPKFail");
		m_sErrString[eUpadateReload] = DICTOOL->getStringValue_json(subVal, "reloadAPK");
		m_sErrString[eUpdatePrompt] = DICTOOL->getStringValue_json(subVal, "downloadprompt");
		CCLOG("[trace]load json file %s", strFilePath.c_str());
		return true;
	}
}


void UpdateView::updateViewText(float time)
{
	setPercentage(UpdateMgrImp::getInstance()->GetStatus().nowProgress);
}


void UpdateView::setPercentage(float fPercentage) 
{
	m_fPercentage = fPercentage;

	if (m_bIsAllReady) { 
		m_loadingImage->setContentSize(Size(LoadingBg_Width*(fPercentage < 5 ? 5 : fPercentage)/100, m_loadingImage->getContentSize().height));
		
		std::string strPercent = "";
		UpdateHelper::IntToString(strPercent, (int) fPercentage);
		strPercent =   strPercent + "%";
		m_loadingText->setString(strPercent.c_str());
	}
}

void UpdateView::setNoticeWords(const char *pszNoticeWords) 
{
	if (m_statusText) 
	{
		m_statusText->setString(pszNoticeWords);
	}
}
void UpdateView::onChangeScene()
{
	//reloadAllCfg();
	//PageFunction::Get()->gotoFunction(MSG_FUC_SAVEDATA);
}

void UpdateView::onFail()
{
	//reloadAllCfg();
	setNoticeWords(m_sErrString[eUpdateFail].c_str());
}

void UpdateView::onRestart()
{

}

void UpdateView::onEnter()
{
	Layout::onEnter();

	m_bIsAllReady = true;
	setPercentage(m_fPercentage);

	this->schedule(CC_SCHEDULE_SELECTOR(UpdateView::checkUpdate), 0.2f);
	this->schedule(CC_SCHEDULE_SELECTOR(UpdateView::updateViewText), 0.3f);
}

void UpdateView::onExit()
{
	Layout::onExit();
	this->unschedule(CC_SCHEDULE_SELECTOR(UpdateView::checkUpdate));
	this->unschedule(CC_SCHEDULE_SELECTOR(UpdateView::updateViewText));

	removeAllChildrenWithCleanup(true);
	m_bIsAllReady = false;
	m_fPercentage = 0.0f;

}

bool UpdateView::ccTouchBegan(class cocos2d::Touch *, class cocos2d::Event *)
{
	return true;
}

void UpdateView::playLoadingAni()
{
	if (m_bIsPlayLoading)
		return;
	m_bIsPlayLoading = true;
	m_loadingText->getParent()->setVisible(false);//只设置为false，而不考虑是否需要设置为true,因为加载完资源就会切换场景了
	Armature* loadingAni = cocostudio::Armature::create("fla_loadingRes");
	loadingAni->getAnimation()->playWithIndex(0);
	loadingAni->setTag(LoadingAni_Tag);
	m_animationNode->addChild(loadingAni);
}

void UpdateView::stopLoadingAni()
{
	if (m_bIsPlayLoading == true)
	{
		m_bIsPlayLoading = false;
		m_animationNode->removeChildByTag(LoadingAni_Tag);
	}
}

void UpdateView::showUpdateConfirm()
{
	int nConfirm = UpdateMgrImp::getInstance()->getUpdateConfirm();
	if (nConfirm > 0)
		return;

	ConfirmLayout* pConfirm = dynamic_cast<ConfirmLayout*>(this->getChildByName("confirm"));
	if (!pConfirm)
	{
		pConfirm = ConfirmLayout::create();
		this->addChild(pConfirm, 999, "confirm");

		pConfirm->setContent(m_sErrString[eUpdatePrompt]);

		Size win = Director::getInstance()->getWinSize();
		pConfirm->setPosition(Vec2(win.width / 2, win.height / 2));
	}
}

bool ConfirmLayout::init()
{
	Node* pRoot = CSLoader::getInstance()->createNode("GenerlDialog.csb");
	if (!pRoot)
		return false;

	this->addChild(pRoot);

	ui::Widget* pSureBtn = dynamic_cast<ui::Widget*>(pRoot->getChildByName("m_sure"));
	ui::Widget* pCancelBtn = dynamic_cast<ui::Widget*>(pRoot->getChildByName("m_cancel"));

	pCancelBtn->setVisible(true);
	pSureBtn->setPositionX(pSureBtn->getPositionX() + 130);

	pSureBtn->addClickEventListener(CC_CALLBACK_1(ConfirmLayout::confirmDownloadCallback, this));
	pCancelBtn->addClickEventListener(CC_CALLBACK_1(ConfirmLayout::cancelDownloadCallback, this));
	return true;
}

void ConfirmLayout::cancelDownloadCallback(cocos2d::Ref* sender)
{
	UpdateMgrImp::getInstance()->setUpdateConfirm(1);
	this->removeFromParent();

	PageFunction::Get()->gotoFunction(MSG_FUC_LOGIN);
	sgNetManager()->DisConnect();
}

void ConfirmLayout::confirmDownloadCallback(cocos2d::Ref* sender)
{
	UpdateMgrImp::getInstance()->setUpdateConfirm(2);
	this->removeFromParent();
}

void ConfirmLayout::setContent(std::string szContent)
{
	Text* pContent = Text::create(szContent, "font.ttf", 26);
	if (!pContent)
		return;

	this->addChild(pContent);
	this->setAnchorPoint(Vec2(0.5, 0.5));

	pContent->setColor(Color3B(126, 65, 24));
	Size size = this->getBoundingBox().size;
	pContent->setPosition(Vec2(0, 0));
}
