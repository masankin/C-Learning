#include "ActivationDialog.h"
#include "game/ui/PageFunction.h"
#include "channel/ChannelApi.h"
#include "net/UserObj.h"
#include "EnvSet.h"
#include "SoundManager.h"
#include "activation/ActivationControl.h"
#include "core/MessageManager.h"
#include "data/TableManager.h"

#define ACTIVATION_LESS_LENTH 8

void ActivationDialog::onMenuItemAction(std::string cName, cocos2d::Ref* sender)
{
	if (cName == "onCancel")
	{
		mTextFieldCode->setString("");
		mTextDes->setVisible(true);
		this->setShow(false);
		sgPageFunction()->enableOptionButton(true);
	}
	else if (cName == "onGet")
	{
		
		int errCode = 0;

		if (mTextFieldCode->getStringLength() <= 0)
		{
			errCode = TEXT_ACTIVATION_UNINPUT;
		}
		else if (mTextFieldCode->getStringLength() < ACTIVATION_LESS_LENTH)
		{
			errCode = TEXT_ACTIVATION_INPUT_NOTENOUGH;
		}
		if (errCode > 0)
		{
			const TextHelper* pTextHelper = sgTableManager()->getTextHelperByID(errCode);
			if(pTextHelper)
				Window::showTextTip(pTextHelper->text);
			return;
		}

		sgActivationControl()->sendGiftCodeRqst(mTextFieldCode->getString());
	
	}

}

bool ActivationDialog::init()
{
	bool ret = false;
	do
	{
		mRoot = CSLoader::getInstance()->createNode("ui/ActivationDialog.csb");
		mRoot->setAnchorPoint(Vec2(0.5, 0.5));
		CC_BREAK_IF(!mRoot);
		this->addChild(mRoot);

		mTextFieldCode = dynamic_cast<ui::TextField*>(mRoot->getChildByName("TextFieldCode"));
		mTextFieldCode->addEventListenerTextField(this, textfieldeventselector(ActivationDialog::TextFieldCallback));


		mTextDes = dynamic_cast<ui::Text*>(mRoot->getChildByName("TextDes"));
		onLocateClickCallback(mRoot);

		ret = true;
	} while (0);


	return ret;
}

void ActivationDialog::TextFieldCallback(Ref* pSender, TextFiledEventType type)
{
	if (type == TextFiledEventType::TEXTFIELD_EVENT_INSERT_TEXT)
	{
		if (mTextFieldCode->getStringLength() <= 0)
		{
			mTextDes->setVisible(true);
		}
		else
		{
			mTextDes->setVisible(false);
		}
	}
	else if (type == TextFiledEventType::TEXTFIELD_EVENT_DELETE_BACKWARD)
	{
		if (mTextFieldCode->getStringLength() <= 0)
			mTextDes->setVisible(true);
	}
}

void ActivationDialog::onReceiveMassage(GameMessage * message)
{
	if (message->getMessageType() == EMT_ACTIVATION_TIP)
	{
		MessageActivationTip* pMsg = dynamic_cast<MessageActivationTip*>(message);
		showTextTips(pMsg->getStatusId());

	}


}

void ActivationDialog::showTextTips(int tableKey)
{
	const TextHelper* pTextHelper = sgTableManager()->getTextHelperByID(tableKey);
	Window::showTextTip(pTextHelper->text);
}

void ActivationDialog::onShow()
{
	Window::onShow();
}


ActivationDialog::ActivationDialog() :mTextFieldCode(NULL)
{
	sgMessageManager()->regisiterMessageHandler(EMT_ACTIVATION_TIP, this);
}

ActivationDialog::~ActivationDialog()
{
	sgMessageManager()->removeMessageHandler(this);
}
