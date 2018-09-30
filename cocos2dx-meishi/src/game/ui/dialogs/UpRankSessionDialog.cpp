#include "UpRankSessionDialog.h"
#include "data/TableManager.h"
#include "ui/PageFunction.h"
UpRankSessionDialog::UpRankSessionDialog(void) : mArmItem(nullptr), mArmAdditive(nullptr), mNodeDes(nullptr), mTextTips(nullptr)
{

}


UpRankSessionDialog::~UpRankSessionDialog(void)
{
	cocostudio::ArmatureDataManager::getInstance()->removeArmatureFileInfo("fla_chengjiuUP.xml");
}

void UpRankSessionDialog::onMenuItemAction(std::string cName, cocos2d::Ref* sender)
{
	if (cName == "onClose")
	{
		this->setShow(false);
	}
}

UpRankSessionDialog* UpRankSessionDialog::showUpRankSessionDialog(int iPrevSession, int iCurSession)
{
	UpRankSessionDialog* pDialog = dynamic_cast<UpRankSessionDialog*>(sgPageFunction()->createWindow(
			MSG_FUC_RANKSESSION_UPGRADE, "UpRankSessionNode.csb"));
	if (pDialog)
	{
		pDialog->mPrevRankSession = iPrevSession;
		pDialog->mCurRankSession = iCurSession;

		pDialog->updateUI();
		return pDialog;
	}
	return nullptr;
	
	
}
bool UpRankSessionDialog::init()
{
	if (!Window::init())
	{
		return false;
	}
	cocostudio::ArmatureDataManager::getInstance()->addArmatureFileInfo("fla_chengjiuUP.xml");

	mRoot = CSLoader::getInstance()->createNode("ui/UpRankSessionNode.csb");
	mRoot->setAnchorPoint(Vec2(0.5, 0.5));
	this->addChild(mRoot);

	mNodeDes = mRoot->getChildByName("NodeDes");
	mTextTips = dynamic_cast<ui::Text*>(mRoot->getChildByName("TextTips"));

	auto nodeAni = mRoot->getChildByName("NodeAni");
	mArmItem = cocostudio::Armature::create("fla_chengjiuUP");
	mArmAdditive = cocostudio::Armature::create("fla_chengjiuUP_ADDITIVE");
	mArmAdditive->setBlendFunc(BlendFunc::COMADDITIVE);
	nodeAni->addChild(mArmItem);
	nodeAni->addChild(mArmAdditive);
	onLocateClickCallback(mRoot);
	return true;
}

void UpRankSessionDialog::updateUI()
{
	playUpgradeAni();
	
	if (mPrevRankSession == 0)
	{
		setFirstTipsShow(true);
	}
	else
	{
		setFirstTipsShow(false);
	}
}

void UpRankSessionDialog::playUpgradeAni()
{
	auto bone = mArmItem->getBone("chengjiuUP_new");
	std::string strIcon = "";
	

	strIcon = StringUtils::format("ui/Icon2/ranking_Emblem-%02d_big.png", mCurRankSession);
	auto spriteCurRankSession = Sprite::createWithSpriteFrameName(strIcon);
	if (mPrevRankSession != 0)
		strIcon = StringUtils::format("ui/Icon2/ranking_Emblem-%02d_big.png", mPrevRankSession);

	auto spritePrevRankSession = Sprite::createWithSpriteFrameName(strIcon);
	if (mPrevRankSession != 0)
		bone->addDisplay(spritePrevRankSession, 0);
	bone->addDisplay(spriteCurRankSession, 1);
	bone->changeDisplayByIndex(0, true);
	mArmItem->getAnimation()->setFrameEventCallFunc([this](cocostudio::Bone *bone, const std::string& frameEventName, int originFrameIndex, int currentFrameIndex)
	{
		if (frameEventName == "tk")
		{
			auto bone = mArmItem->getBone("chengjiuUP_new");
			if (bone)
				bone->changeDisplayByIndex(1, true);
		}

	});
	/*mArmItem->getAnimation()->setMovementEventCallFunc([this](cocostudio::Armature * pAr, cocostudio::MovementEventType type, const std::string& aniName)
	{
		if (type == cocostudio::COMPLETE)
		{
			this->setShow(false);
		}
	});*/
	mArmItem->getAnimation()->play("play");
	mArmAdditive->getAnimation()->play("play");
}

void UpRankSessionDialog::setFirstTipsShow(bool bShow)
{
	mNodeDes->setVisible(bShow);
	mTextTips->setVisible(bShow);
}
