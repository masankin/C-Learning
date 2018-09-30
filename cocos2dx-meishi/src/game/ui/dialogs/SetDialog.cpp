#include "SetDialog.h"
#include "game/ui/PageFunction.h"
#include "game/data/Tables.h"
#include "channel/ChannelApi.h"
#include "game/data/TableManager.h"
#include "net/UserObj.h"
#include "EnvSet.h"
#include "game/net/UserObj.h"
#include "SoundManager.h"
#include "game/utils/Comm.h"
#include "utils/GameDefault.h"
#ifdef MGAME_PLATFORM_MPLUS
#include "game/utils/GameDefault.h"
#include "mplus/MplusControl.h"
#endif

void SetDialog::onMenuItemAction(std::string cName, cocos2d::Ref* sender)
{
	if (cName == "onFailClose")
	{
		this->setShow(false);
		sgPageFunction()->enableOptionButton(true);
		sgMessageManager()->sendMessage(EMT_SELECTWORD_SCHEDULE, "");
		return;
	}
	else if (cName == "onMusic")
	{
		onMusic();
	}
	else if (cName == "onVoice")
	{
		onVoice();
	}
	else if (cName == "onExit")
	{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)		
		if(sgChannelApi()->getChannelId() != ChannelApi::eChannelID::ChannelID_Default)
			sgChannelApi()->channelLogout();
		else
			Director::sharedDirector()->end();
#else
		Director::sharedDirector()->end();
#endif	

		
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)			
		exit(0);
#endif
	}
	else if (cName == "onEvaluation")
	{

	}
	else if (cName == "onActive")
	{
		this->setShow(false);
		Window* pPage = Window::Create("ActivationDialog.csb");
		pPage->setShow(true);
	}
#ifdef MGAME_PLATFORM_MPLUS
	else if (cName == "onPopAd")
	{
		sgPageFunction()->gotoFunction(MSG_FUC_FREEDIAMOND);
	}
	else if (cName == "onCustomService")
	{
		sgPageFunction()->gotoFunction(MSG_FUC_CUSTOMER_SERVER);

		//Window* pPage = Window::Create("About.csb");
		//pPage->setShow(true);
	}
	else if (cName == "onAbout")
	{
		//前往官网
		std::string url = sgChannelApi()->getShareConfig("BaHaURL");
		sgChannelApi()->callGameInfo(TYPE_REQUEST_HTTP, url);
		return;
	}
	else if (cName == "onInvite")
	{
		sgPageFunction()->gotoFunction(MSG_FUC_INVITECODE);
	}
	else if (cName == "onBind")
	{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)			
		if(sgChannelApi()->getChannelId() != ChannelApi::eChannelID::ChannelID_Default)
		{
			sgChannelApi()->channelLogout();
			this->setShow(false);
		}
#endif
	}
	else if (cName == "onBaHa")
	{
		//前往M+巴哈姆特
		std::string url = sgChannelApi()->getShareConfig("AcgGamerURL");
		sgChannelApi()->callGameInfo(TYPE_REQUEST_HTTP, url);
		return;
	}
#endif
	else if (cName == "onAbout")
	{
		Window* pPage = Window::Create("About.csb");
		pPage->setShow(true);
	}
	else if (cName == "onCancel")
	{
		SAVE_USER_DEFAULT_INT("USER_LoginType", 0);
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
		sgChannelApi()->channelLogout();
		sgChannelApi()->setChannelInfo(0, 1);
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
		//sgNetManager()->DisConnect();
		sgLoginProxy()->sendLogOut();
		PageFunction::Get()->gotoFunction(MSG_FUC_LOGIN);
#endif	
	}
}

bool SetDialog::init()
{
	bool ret = false;
	do
	{
		mRoot = CSLoader::createNode("ui/SetDialog.csb");
		mRoot->setAnchorPoint(Vec2(0.5, 0.5));
		CC_BREAK_IF(!mRoot);
		//mRoot->setPosition(Vec2(0, -50));
		this->addChild(mRoot);

		//setAnimation("fla_shezhi");

		mVoiceNo = dynamic_cast<ui::Button*>(mRoot->getChildByName("Button_1_2"));
		mMusicNo = dynamic_cast<ui::Button*>(mRoot->getChildByName("Button_1_2_0"));
		mVoice_5 = dynamic_cast<Sprite*>(mRoot->getChildByName("uiAtlas_Battle_voice_5"));
		mMusic_6 = dynamic_cast<Sprite*>(mRoot->getChildByName("uiAtlas_Login_music_6"));

		mName = dynamic_cast<ui::Text*>(mRoot->getChildByName("mNameLabel"));
		mId = dynamic_cast<ui::Text*>(mRoot->getChildByName("mIdLabel"));
#ifdef MGAME_PLATFORM_MPLUS
		if (mName) {
			mName->setFontName(DEFAULT_FONT);
		}
		mBtnExit = dynamic_cast<ui::Button*>(mRoot->getChildByName("ButtonExit"));
		mBtnBind = dynamic_cast<ui::Button*>(mRoot->getChildByName("ButtonBind"));
#endif
		bool bMusic = sgSoundManager()->getIsPlayMusicBg();
		bool bVoice = sgSoundManager()->getIsPlayMusicEffect();

		if (!bMusic)
		{
			mMusicNo->setBrightStyle(BrightStyle::HIGHLIGHT);
			mMusic_6->setPositionX(mMusic_6->getPositionX() - 150);
		}

		if (!bVoice)
		{
			mVoiceNo->setBrightStyle(BrightStyle::HIGHLIGHT);
			mVoice_5->setPositionX(mVoice_5->getPositionX() - 150);
		}

		// 电信包显示《关于我们》按钮
		ui::Button* pSetBtn = dynamic_cast<ui::Button*>(mRoot->getChildByName("Button_1_0_0_0"));
		if (pSetBtn)
		{
			if (sgChannelApi()->getChannelId() == ChannelApi::eChannelID::ChannelID_DianXin
				|| sgChannelApi()->getChannelId() == ChannelApi::eChannelID::ChannelID_MPlus)
			{
				pSetBtn->setBright(true);
				pSetBtn->setTouchEnabled(true);
			}
			else{
				pSetBtn->setBright(false);
				pSetBtn->setTouchEnabled(false);
			}
		}
		
		setPopType(E_POP_TYPE_BIG);
		onLocateClickCallback(mRoot);
		ret = true;
	} while (0);


	return ret;
}

void SetDialog::onShow()
{
	// 用户昵称显示
	mName->setText(sgUserObj()->getPlayerName());

#ifdef MGAME_PLATFORM_MPLUS
	resetSDKBtnShow();
	mId->setText(sgUserObj()->getOpenID());
#else
	mId->setText(StringUtils::format("%d", sgUserObj()->getNewUin()));
#endif

#ifdef MGAME_TENCENT_PLATFORM

	Sprite* tIconQQ = dynamic_cast<Sprite*>(mRoot->getChildByName("Image_1")->getChildByName("Setting_qq_1"));
	Sprite* tIconWX = dynamic_cast<Sprite*>(mRoot->getChildByName("Image_1")->getChildByName("Setting_weixin_2"));
	if (sgChannelApi()->getChannelId() == ChannelApi::eChannelID::ChannelID_Tencent_ShouQ)
	{
		tIconQQ->setVisible(true);
		tIconWX->setVisible(false);
	}
	else if (sgChannelApi()->getChannelId() == ChannelApi::eChannelID::ChannelID_Tencent_WeiXin)
	{
		tIconQQ->setVisible(false);
		tIconWX->setVisible(true);
	}

	//ui::Text* tChannelPlayerName = dynamic_cast<ui::Text*>(mRoot->getChildByName("Image_1")->getChildByName("mNameLabel_0"));
	if (CC_PLATFORM_WIN32 != CC_TARGET_PLATFORM)
	{
		mName->setText(sgUserObj()->getChannelPlayerName());
		Size _size = mName->getVirtualRendererSize();
		if (_size.width < 1)
		{
			mName->setString("...");
		}
		else
		{
			std::string textStr = mName->getString();
			int length = textStr.length();
			bool flag = false;
			for (unsigned int i = 0; i < length; i++)
			{
				if (textStr.at(i) < 33 || textStr.at(i) > 126){
					flag = true;
					break;
				}
			}
			if (!flag)
			{
				std::string chackStr = "";
				int rt = checkCharacter(textStr, chackStr);
				if (rt > 1){
					chackStr = mName->getString().substr(0, rt);
					mName->setString(chackStr);
				}
			}
			else
			{
				std::u16string utf16String;
				std::u16string newUtf16String;
				if (StringUtils::UTF8ToUTF16(mName->getString(), utf16String))
				{
					int rt = mName->getStringLength();
					if (rt > 6){
						for (int i = 0; i < 6; i++)
						{
							newUtf16String += utf16String[i];
						}
						std::string chackStr = "";
						if (StringUtils::UTF16ToUTF8(newUtf16String, chackStr))
						{
							mName->setString(chackStr.append("..."));
						}
					}
				}
			}
		}
	}
//#else
//	mRoot->getChildByName("Image_1")->setVisible(false);
#endif
	
	bool isplayEffect = sgSoundManager()->getIsPlayMusicEffect();
	bool isPlayBg = sgSoundManager()->getIsPlayMusicBg();
	mVoiceNo->setBrightStyle(isplayEffect ? BrightStyle::NORMAL : BrightStyle::HIGHLIGHT);
	mVoice_5->setPositionX(isplayEffect ? mVoiceNo->getPositionX() + 80 : mVoiceNo->getPositionX() - 90);
	
	mMusicNo->setBrightStyle(isPlayBg ? BrightStyle::NORMAL : BrightStyle::HIGHLIGHT);
	mMusic_6->setPositionX(isPlayBg ? mMusicNo->getPositionX() + 80 : mMusicNo->getPositionX() - 90);

} 

SetDialog::SetDialog() :mVoiceNo(0), mMusicNo(0)
{
	//cocostudio::ArmatureDataManager::getInstance()->addArmatureFileInfo("fla_shezhi.png", "fla_shezhi.plist", "fla_shezhi.xml");

}

SetDialog::~SetDialog()
{
	//cocostudio::ArmatureDataManager::getInstance()->removeArmatureFileInfo("fla_shezhi.xml");

}

void SetDialog::onVoice()
{
	//音效
	bool isplayEffect = sgSoundManager()->getIsPlayMusicEffect();

	if (isplayEffect == true)
	{
		sgSoundManager()->setIsPlayMusicEffect(false);
		sgSoundManager()->stopAllMusicEffects();
		mVoiceNo->setBrightStyle(BrightStyle::HIGHLIGHT);
		mVoice_5->setPositionX(mVoiceNo->getPositionX() - 90);
	}
	else
	{
		sgSoundManager()->setIsPlayMusicEffect(true);
		mVoiceNo->setBrightStyle(BrightStyle::NORMAL);
		mVoice_5->setPositionX(mVoiceNo->getPositionX() + 80);
	}

}

void SetDialog::onMusic()
{
	//背景音乐
	bool isPlayBg = sgSoundManager()->getIsPlayMusicBg();

	if (isPlayBg == true)
	{
		sgSoundManager()->setIsPlayMusicBg(false);
		sgSoundManager()->stopMusicBg();
		mMusicNo->setBrightStyle(BrightStyle::HIGHLIGHT);
		mMusic_6->setPositionX(mMusicNo->getPositionX() - 90);
	}
	else
	{
		sgSoundManager()->setIsPlayMusicBg(true);
		//设置UI只会在主界面出现，所以不用判断当前地图
		//int checkpt = sgGamePrecedure()->getCurMapInfo().checkpt;
		sgSoundManager()->playMusicBg(MUSIC_UI_MAIN);
		mMusicNo->setBrightStyle(BrightStyle::NORMAL);
		mMusic_6->setPositionX(mMusicNo->getPositionX() + 80);

	}
}
#ifdef MGAME_PLATFORM_MPLUS
void SetDialog::resetSDKBtnShow()
{
	mBtnExit->setVisible(true);
	mBtnBind->setVisible(false);

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	if(sgChannelApi()->getChannelId() != ChannelApi::eChannelID::ChannelID_Default)
	{
		if (USER_DEFAULT_INT(CHANNEL_LOGIN_TYPE) == TYPE_LOGIN_GUEST)
		{
			
			mBtnExit->setVisible(false);
			mBtnBind->setVisible(true);
		}
	}
#endif
}
#endif
