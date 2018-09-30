#include "PlayerUpgradeDialog.h"
#include "game/net/UserObj.h"
#include "game/ui/PageFunction.h"
PlayerUpgradeDialog::PlayerUpgradeDialog()
{
}

PlayerUpgradeDialog::~PlayerUpgradeDialog()
{

}

bool PlayerUpgradeDialog::init()
{

	bool ret = false;
	do
	{
		mRoot = CSLoader::getInstance()->createNode("ui/upgradeNode.csb");
		CC_BREAK_IF(!mRoot);
		mRoot->setAnchorPoint(Vec2(0.5, 0.5));
		setAnimation("fla_ShengJidh");

		setPopType(E_POP_TYPE_NONE);
		this->addChild(mRoot,99999);

		mNodeLevel = dynamic_cast<TextBMFont*>(mRoot->getChildByName("level"));

		mNodeLevel->setPositionY(40);
		changeBone(mArmatureBg, "guge_dengji", mNodeLevel);
		
		ret = true;
	} while (0);

	return ret;
}

void PlayerUpgradeDialog::onShow()
{
	CCLOG("[trace]PlayerUpgradeDialog::onShow, onShow");

	sgUserObj()->setLvlUpgrade(false);
	int level = sgUserObj()->getPersonalInfo()->explevel();
	mNodeLevel->setString(StringUtils::format("%d", level));

	DelayTime *delay = DelayTime::create(1.5);
	CallFunc *func = CallFunc::create(CC_CALLBACK_0(PlayerUpgradeDialog::closeCallBack,this));
	Sequence *seq = Sequence::create(delay, func, NULL);
	this->runAction(seq);
}


void PlayerUpgradeDialog::onClose()
{

	int state = sgUserObj()->getPlayerUpgradeState();
	if (state != 0)
	{
		sgUserObj()->setPlayerUpgradeState(0);
		sgPageFunction()->gotoFunction((ePageType)state);
	}
}

void PlayerUpgradeDialog::closeCallBack()
{
	this->setShow(false);
}



