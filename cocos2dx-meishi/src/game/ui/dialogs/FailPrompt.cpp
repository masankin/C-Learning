#include "FailPrompt.h"
#include "GamePrecedure.h"
#include "ui/PageFunction.h"
#include "net/UserObj.h"
#include "pvp/PvpControl.h"
#include "pvp/PvpProxy.h"
#include "SoundManager.h"

FailPromptDialog::FailPromptDialog()
{

}

FailPromptDialog::~FailPromptDialog()
{

}

bool FailPromptDialog::init()
{
	bool ret = false;
	do
	{
		mRoot = CSLoader::getInstance()->createNode("ui/FailPrompt.csb");
		CC_BREAK_IF(!mRoot);

		mRoot->setAnchorPoint(Vec2(0.5, 0.5));
		this->addChild(mRoot);

		onLocateClickCallback(mRoot);

		ret = true;
	} while (0);

	return ret;
}

void FailPromptDialog::onMenuItemAction(std::string cName, cocos2d::Ref*)
{
	if (cName == "onExit")
	{
		PageFunction::Get()->gotoFunction(MSG_FUC_WIN_PVP);

		if (sgUserObj()->getIsHost())
		{
			sgPvpControl()->getPvPProxy()->sendLeave();
		}
	}

	this->setShow(false);
}

