#include "ActionWindow.h"

void ActionWindow::setShow(bool b)
{
	if (bIsShow != b)
	{
		if (b)
		{
			Window::Add(this);
			this->bIsShow = true;
			this->onActionShow();
			this->onShow();
			mAnimationFinish = false;
			if (mArmatureBg && mIsPlayBorn)
			{
				mArmatureBg->getAnimation()->play("born");
			}
		}
		else
		{
			this->onActionClose();
			if (mArmatureBg && mIsPlayEndOnClose)
			{
				cocostudio::MovementData* pMovementData = mArmatureBg->getAnimation()->getAnimationData()->getMovement("end");
				if (pMovementData != NULL)
				{
					mArmatureBg->getAnimation()->play("end");
					return;
				}
			}
			Window::Remove(this);
			this->bIsShow = false;
			this->onClose();
			Window::afterClose();

			cleanUp();
		}
	}
	else
	{
		if (b)
		{
			this->bIsShow = true;
			this->onActionShow();
			this->onShow();
			mAnimationFinish = false;
			if (mArmatureBg && mIsPlayBorn)
			{
				mArmatureBg->getAnimation()->play("born");
			}
		}
	}

}

void ActionWindow::windowAniCallback(cocostudio::Armature * pAr, cocostudio::MovementEventType type, const std::string& aniName)
{

	aniCallback(pAr, type, aniName);

	if (cocostudio::COMPLETE == type)
	{
		if ("end" == aniName)
		{
			Window::Remove(this);
			this->bIsShow = false;
			this->onClose();

			Window::afterClose();

			pAr->cleanup();
			this->cleanUp();
		}
		else if ("born" == aniName)
		{
			cocostudio::MovementData* pMovementData = pAr->getAnimation()->getAnimationData()->getMovement(mStandName);
			if (pMovementData != NULL)
			{
				pAr->getAnimation()->play(mStandName);
			}
		}

	}
}

void ActionWindow::onWindowFrameEvent(cocostudio::Bone *bone, const std::string& evt, int originFrameIndex, int currentFrameIndex)
{
	onFrameEvent(bone, evt, originFrameIndex, currentFrameIndex);
}

void ActionWindow::aniCallback(cocostudio::Armature * pAr, cocostudio::MovementEventType type, const std::string& aniName)
{
	if (getLuaType())
	{
		sgCppCallLuaHelper()->aniCallback(this, pAr, type, aniName);
	}
};

void ActionWindow::onFrameEvent(cocostudio::Bone *bone, const std::string& evt, int originFrameIndex, int currentFrameIndex)
{
	if (getLuaType())
	{
		sgCppCallLuaHelper()->onFrameEvent(this, bone, evt, originFrameIndex, currentFrameIndex);
	}
};

bool ActionWindow::setAnimation(std::string name)
{
	if (mRoot == 0) return false;

	mArmatureBg = cocostudio::Armature::create(name);
	mArmatureBg->getAnimation()->setFrameEventCallFunc(this, frameEvent_selector(ActionWindow::onWindowFrameEvent));
	mArmatureBg->getAnimation()->setMovementEventCallFunc(this, movementEvent_selector(ActionWindow::windowAniCallback));
	mCenter = mRoot->getChildByName("mCenter");
	if (mCenter)
	{
		mCenter->addChild(mArmatureBg);
	}
	else
	{
		this->addChild(mArmatureBg);
	}

	return true;
}

void ActionWindow::onMenuItemAction(std::string cbName, cocos2d::Ref* sender,Widget::TouchEventType type)
{
	cocos2d::ui::Widget* widget = dynamic_cast<cocos2d::ui::Widget*>(sender);
	if (!widget)
		return;

	if (type == Widget::TouchEventType::BEGAN)
	{
		std::string aniName = GET_USERDATA(widget);
		mSender = widget;
		if (!mArmatureBg) return;

		std::string stBone = mArmatureBg->getName().substr(4, mArmatureBg->getName().length() - 1) + "_" + aniName;
		cocostudio::Bone * pBone = mArmatureBg->getBone(stBone);

		if (pBone)
			pBone->getChildArmature()->getAnimation()->play("play");
	}

	if (type == Widget::TouchEventType::ENDED)
	{
		std::string cbName = mSender->getCallbackName();
		onMenuItemAction(cbName, sender);
	}
}

void ActionWindow::changeBone(cocostudio::Armature* pAr, std::string stBone, Node* display,int index)
{
	cocostudio::Bone* pBone = pAr->getBone(stBone);
	if (pBone)
	{
		pBone->addDisplay(display, index);
	}
}

void ActionWindow::changeBoneByDefaultArmature(std::string stBone, Node* display, int index /* = 0 */)
{ 
	cocostudio::Bone* pBone = mArmatureBg->getBone(stBone);
	if (pBone)
	{
		pBone->addDisplay(display, index);
	}
}

bool ActionWindow::_initWindow(ePriority priority, Widget* showFrom)
{
	mStandName = "stand";
	if (m_eType == E_POP_TYPE_SMALL)
	{
		setAnimation("fla_xiaomingban");
		changeBone(mArmatureBg, "xiaomingban_czxczxcz", mRoot);
	}
	else if (m_eType == E_POP_TYPE_BIG)
	{
		setAnimation("fla_damianban");
		changeBone(mArmatureBg, "damianban_ddfsdfww", mRoot);
	}

	return Window::_initWindow(priority, showFrom);
}


cocostudio::Armature* ActionWindow::getArmatureBg()
{
	if (mArmatureBg)
		return mArmatureBg;
	return NULL;
}

Point ActionWindow::getBonePosition(std::string boneName, Point offPos, cocostudio::Armature* ani)
{
	if (NULL == ani)
		ani = mArmatureBg;
	cocostudio::Bone* pBone = mArmatureBg->getBone(boneName);
	if (pBone)
		return ccp(pBone->getWorldInfo()->x + offPos.x, pBone->getWorldInfo()->y + offPos.y);
	return offPos;
}

void ActionWindow::setPopType(ActionWindowPopType type)
{
	m_eType = type;
}

void ActionWindow::playAnimationByName(std::string name)
{
	mArmatureBg->getAnimation()->play(name);
}

void ActionWindow::setStandName(std::string name)
{
	mStandName = name;
}
