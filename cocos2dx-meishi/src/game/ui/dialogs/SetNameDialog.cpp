#include "SetNameDialog.h"
#include "game/ui/PageFunction.h"
#include "channel/ChannelApi.h"
#include "game/data/TableManager.h"
#include "EnvSet.h"

#include "MessageManager.h"
#include "sync/SyncControl.h"
#include "cocos2d.h"
#include "game/net/UserObj.h"
#include "core/WordChecker.h"
#include "SoundManager.h"
#include "utils/Comm.h"
#include "utils/HeadIconUtils.h"
USING_NS_CC;

void NameingDialog::onMenuItemAction(std::string cName, cocos2d::Ref* sender)
{
	if (cName == "onClose")
	{
		this->setShow(false);
	}
	else if(cName == "onRename")
	{
		std::string chackStr = "";
		int ret = checkCharacter(mTextField->getString(), chackStr);
		if (ret != 1)
		{
			if (chackStr.length() > 0)
			{
				mText_Tip->setText(StringUtils::format(sgTextHelper(TEXT_RENAME_ERROR).c_str(), chackStr.c_str()));
			}
			else
			{			
				mText_Tip->setText(sgTextHelper(TEXT_RENAME_LENGHT));
			}
			return;
		}

		if (sgUserObj()->getPlayerName().compare(mTextField->getString().c_str()) == 0 && mSelectArea == sgUserObj()->getPersonalInfo()->area())
		{
			mText_Tip->setText(sgTextHelper(TEXT_NOT_CHANGE));
			return;
		}

		std::string sName = WordChecker::Get()->replaceSensitiveWord(mTextField->getString(), "*", 1);
		if (mTextField->getString().compare(sName)) {
			mText_Tip->setText(sgTextHelper(TEXT_RENAME_LENGHT));
			return;
		}

		if (sgUserObj()->getPersonalInfo()->changenamecount() > 1)
		{
			if (sgUserObj()->getPersonalInfo()->diamond() < NEED_GOLDCOIN)
			{
				mText_Tip->setText(sgTextHelper(TEXT_RENAME_FAILED));
				PageFunction::Get()->gotoFunction(MSG_FUC_WIN_SHOP);
				return;
			}
		}

		sgSyncControl()->getSyncProxy()->sendChangeName(mTextField->getString(),mSelectArea);
	}
	else if (cName == "onRandom")
	{
		mTextField->setText(CommFunc::getRandName());
		mText_Tip->setText("");

		mText_Tip2->setString(sgTextHelper(TEXT_NAMEING_TIP2));
	}
	else if (cName == "onArea")
	{
		mText_Tip2->setString(sgTextHelper(TEXT_NAMEING_TIP1));
		mRoot->getChildByName("Node_Change")->setVisible(false);
		mRoot->getChildByName("Button_Close")->setVisible(false);
		mRoot->getChildByName("Image_Area")->setVisible(true);
	}
	else if (cName == "onAreaSelect")
	{
		mText_Tip2->setString(sgTextHelper(TEXT_NAMEING_TIP2));
		mRoot->getChildByName("Node_Change")->setVisible(true);
		if (sgUserObj()->getPersonalInfo()->changenamecount() != 0)
			mRoot->getChildByName("Button_Close")->setVisible(true);
		mRoot->getChildByName("Image_Area")->setVisible(false);

		int tag = dynamic_cast<Node*>(sender)->getTag();
		if (mSelectArea != tag)
		{
			mSelectArea = tag;
			setBtnAreaBg(mBtnArea, mSelectArea);
		}
	}
}

void NameingDialog::onReceiveMassage(GameMessage * message)
{
	if (message->getMessageType() == EMT_CHANGE_NAME)
	{
		MessageChangeName* pMes = dynamic_cast<MessageChangeName*>(message);
		if (pMes)
		{
			switch (pMes->mResult)
			{
			case 0:
			{
				if (sgUserObj()->getPersonalInfo()->changenamecount() > 0)
				{
					if (sgUserObj()->getPersonalInfo()->changenamecount() > 1)
					{
						std::string param = StringUtils::format("%d %d %d %d %d %s"
							, 1
							, 1
							, eVituralCurrencyType::Diamond
							, -NEED_GOLDCOIN
							, sgUserObj()->getPersonalInfo()->diamond()//sgUserObj()->getGoldCoin()
							, "SetNameDialog::onReceiveMassage");
						sgMessageManager()->sendMessage(EMT_MONEY_CHANGE, param);
					}
					sgMessageManager()->sendMessage(EMT_CHANGE_NAME, "1");			
				}
				else
				{
					sgMessageManager()->sendMessage(EMT_CHANGE_NAME, "2");
				}
				sgUserObj()->getPersonalInfo()->set_changenamecount(sgUserObj()->getPersonalInfo()->changenamecount() + 1);
				mText_Tip1->setText(sgTextHelper(TEXT_RENAME_SPEND));
				sgUserObj()->getPersonalInfo()->set_name(mTextField->getString());
				sgUserObj()->getPersonalInfo()->set_area(mSelectArea);				

				this->setShow(false);
				break;
			}
			case NAME_NOT_CHANGE:
			{
				mText_Tip->setText(sgTextHelper(TEXT_NOT_CHANGE));
				break;
			}
			case NAME_EXIST_ERROR:
			{
				mText_Tip->setText(sgTextHelper(TEXT_RENAME_EXIST));
				break;
			}
			case ITEM_DIAMAON_NOT_ENOUGH_ERROR:
			{
				mText_Tip->setText(sgTextHelper(TEXT_RENAME_FAILED));
				break;
			}
			case NAME_ERROR:
			{
				std::string chackStr = "";
				int ret = checkCharacter(mTextField->getString(), chackStr);
				if (chackStr.length() > 0)
				{
					mText_Tip->setText(StringUtils::format(sgTextHelper(TEXT_RENAME_ERROR).c_str(), chackStr.c_str()));
				}
				break;
			}
			default:
				break;
			}
		}
	}
}
#include "game/utils/GameDefault.h"
bool NameingDialog::init()
{
	bool ret = false;
	do
	{
		mRoot = CSLoader::getInstance()->createNode("ui/NameingDialog.csb");
		CC_BREAK_IF(!mRoot);
		this->addChild(mRoot);

		auto pNode = mRoot->getChildByName("Node_Change");
		mTextField = dynamic_cast<ui::TextField*>(pNode->getChildByName("TextField_1"));
		mTextField->addEventListenerTextField(this, textfieldeventselector(NameingDialog::TextFieldCallback));
#ifdef MGAME_PLATFORM_MPLUS
		if (mTextField){
			mTextField->setColor(Color3B(126, 65, 24));
			mTextField->setFontName(DEFAULT_FONT);
		}
#endif
		mText_Tip1 = dynamic_cast<ui::Text*>(pNode->getChildByName("Text_7"));
		mText_Tip = dynamic_cast<ui::Text*>(pNode->getChildByName("Text_Tip"));
		mText_Tip->setText("");
		mText_Tip2 = dynamic_cast<ui::Text*>(mRoot->getChildByName("Text_4"));
		mBtnArea = dynamic_cast<Button*>(mRoot->getChildByName("Button_Area"));
		onLocateClickCallback(mRoot);
		mTextField->setString("");
		if (sgUserObj()->getPersonalInfo()->changenamecount() == 0 && sgUserObj()->getChannelPlayerName().size() != 0)
		{
			//std::string name = USER_DEFAULT("USER_NAME");
			std::string textStr = sgUserObj()->getChannelPlayerName();
			HelperFunc::replaceAllCharacters(textStr, " ", "");
			mTextField->setString(textStr);
			Size _size = mTextField->getVirtualRendererSize();
			if (_size.width < 1)
			{
				mTextField->setText(CommFunc::getRandName());			
			}
			else
			{
				textStr = mTextField->getString();
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
						chackStr = mTextField->getString().substr(0, rt);
						mTextField->setString(chackStr);
					}
				}
				else
				{
					std::u16string utf16String;
					std::u16string newUtf16String;
					if (StringUtils::UTF8ToUTF16(mTextField->getString(), utf16String))
					{
						int rt = mTextField->getStringLength();
						if (rt > 6){
							for (int i = 0; i < 6; i++)
							{
								newUtf16String += utf16String[i];
							}
							std::string chackStr = "";
							if (StringUtils::UTF16ToUTF8(newUtf16String, chackStr))
							{
								mTextField->setString(chackStr);
							}
						}
					}
				}	
			}
		}
		else
			mTextField->setText(CommFunc::getRandName());

		auto text_tip = dynamic_cast<ui::Text*>(mRoot->getChildByName("Text_4"));
		text_tip->setString(sgTextHelper(TEXT_NAMEING_TIP1));

		auto pAni = cocostudio::Armature::create("fla_QuMingzi");
		mRoot->getChildByName("Node_Ani")->addChild(pAni);
		pAni->getAnimation()->play("play");

		auto pBone = pAni->getBone("guge_tishizi");
		if (pBone)
			pBone->addDisplay(text_tip, 0);

		pBone = pAni->getBone("QuMingzi_diqu");
		if (pBone)
			pBone->addDisplay(mBtnArea, 0);

		Sprite* sp = Sprite::create(); 

#if CC_PLATFORM_ANDROID ==  CC_TARGET_PLATFORM
		if (sgUserObj()->getPersonalInfo()->changenamecount() == 0 && !sgUserObj()->getHeadIconUrl().empty())
		{
			sgUserObj()->getPersonalInfo()->set_iconid(sgUserObj()->getHeadIconUrl());
			sgSyncControl()->getSyncProxy()->sendChangePlayerIcon(1);
		}		
#endif
		sgHeadIconUtils()->changeIcon(sp, sgUserObj()->getPersonalInfo()->iconid());
		if (sp)
		{
			mRoot->getChildByName("Node_Ani")->addChild(sp);
			sp->setScale(0.9);
			sp->setPosition(ccp(-25, 20));
			pBone = pAni->getBone("guge_toux");
			if (pBone)
				pBone->addDisplay(sp, 0);

			sp->setZOrder(-100);
		}

		
		loadAreaName();
		ret = true;
	} while (0);


	return ret;
}

void NameingDialog::TextFieldCallback(Ref* pSender, TextFiledEventType type)
{
	auto text_tip = dynamic_cast<ui::Text*>(mRoot->getChildByName("Text_4"));
	text_tip->setString(sgTextHelper(TEXT_NAMEING_TIP2));
	mText_Tip->setString("");
	std::string textStr = mTextField->getString();
	std::string chackStr = "";
	HelperFunc::replaceAllCharacters(textStr, " ", "");
	mTextField->setString(textStr);
	int ret = checkCharacter(textStr, chackStr);
	if (ret != 1)
	{
		if (chackStr.length() > 0)
		{
			mText_Tip->setText(StringUtils::format(sgTextHelper(TEXT_RENAME_ERROR).c_str(), chackStr.c_str()));
		}
		if (ret > 1){
			chackStr = textStr.substr(0, ret);
			mTextField->setString(chackStr);
			mText_Tip->setText(sgTextHelper(TEXT_RENAME_LENGHT));
		}	
	}	
}

void NameingDialog::onShow()
{
	if (sgUserObj()->getPersonalInfo()->changenamecount() != 0)
	{
		if (sgUserObj()->getPersonalInfo()->changenamecount() != 1)
			mText_Tip1->setText(sgTextHelper(TEXT_RENAME_SPEND));
		else
			mText_Tip1->setText(sgTextHelper(TEXT_RENAME_FREE));

		mRoot->getChildByName("Button_Close")->setVisible(true);
		mRoot->getChildByName("Node_Change")->getChildByName("Sprite_5")->setVisible(true);
		mRoot->getChildByName("Node_Change")->getChildByName("Sprite_5_0")->setVisible(false);
	}
	else
	{
		mText_Tip1->setVisible(false);
		mRoot->getChildByName("Button_Close")->setVisible(false);
		mRoot->getChildByName("Node_Change")->getChildByName("Sprite_5")->setVisible(false);
		mRoot->getChildByName("Node_Change")->getChildByName("Sprite_5_0")->setVisible(true);
	}

	mText_Tip->setText("");

	Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("gDoneScene");
}

void NameingDialog::loadAreaName()
{
	int index = 0;

#ifdef MGAME_PLATFORM_MPLUS
	index = AREA_NAME_COUNT;
#endif
	mSelectArea = sgUserObj()->getPersonalInfo()->area();
	if (!mSelectArea)
		mSelectArea = cocos2d::random(index + 1,index + AREA_NAME_COUNT);
	setBtnAreaBg(mBtnArea, mSelectArea);

	auto pIamge = mRoot->getChildByName("Image_Area");
	for (int i = index + 1; i <= index + AREA_NAME_COUNT; i++)
	{
		int j = i > AREA_NAME_COUNT ? (i - AREA_NAME_COUNT) : i;
		auto pBtn = dynamic_cast<Button*>(pIamge->getChildByName(StringUtils::format("Button_%d", j)));
		setBtnAreaBg(pBtn, i);
		pBtn->setTag(i);
	}
	
}

void NameingDialog::setBtnAreaBg(Button* btn, int areaID)
{
	NameArea* pArea = sgTableManager()->getNameArea(areaID);
	if (pArea)
	{
		btn->setTitleText(pArea->mName);
		btn->loadTextureNormal(StringUtils::format("ui/guide/Area_%d.png", areaID > AREA_NAME_COUNT ? (areaID - AREA_NAME_COUNT) : areaID), TextureResType::PLIST);
	}
}

NameingDialog::NameingDialog() :mTextField(0), mText_Tip(0)
{
	sgMessageManager()->regisiterMessageHandler(EMT_CHANGE_NAME, this);
	srand(time(NULL) - sgLoginProxy()->getServerTime());

	ArmatureDataManager::getInstance()->addArmatureFileInfo("fla_QuMingzi.png", "fla_QuMingzi.plist", "fla_QuMingzi.xml");
}

NameingDialog::~NameingDialog()
{
	sgMessageManager()->removeMessageHandler(this);
	ArmatureDataManager::getInstance()->removeArmatureFileInfo("fla_QuMingzi.xml");
}
