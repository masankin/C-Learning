#include "PageBase.h"
#include "base/CCEventKeyboard.h"
#include "guide/GuideManager.h"
#include "game/net/UserObj.h"
#include "core/SoundManager.h"
bool PageBase::init()
{
	return true;
}

void PageBase::onEnterTransitionDidFinish()
{
	Layer::onEnterTransitionDidFinish(); 
}

void PageBase::onExitTransitionDidStart()
{
	Layer::onExitTransitionDidStart(); 
}

void PageBase::onLocateClickCallback(Node* parent)
{
	Vector<Node*> children = parent->getChildren();

	for (auto child : children)
	{
		Widget* pWidget = dynamic_cast<Widget*>(child);
		if (pWidget && pWidget->getCallbackName() != "")
		{
			std::string stType = pWidget->getCallbackType();
			if (stType == "Click")
				pWidget->addClickEventListener(CC_CALLBACK_1(PageBase::_onMenuItemClicked, this));
			else if (stType == "Touch")
				pWidget->addTouchEventListener(this, toucheventselector(PageBase::_onMenuItemTouched));
		}
		onLocateClickCallback(child);
	}

}

void PageBase::_onMenuItemTouched(Ref* sender, Widget::TouchEventType type)
{
	cocos2d::ui::Widget* widget = dynamic_cast<cocos2d::ui::Widget*>(sender);
	if (!widget)
		return;

	std::string cbName = widget->getCallbackName();

	if (cbName == "onExit" || cbName == "onBack" || cbName == "onCancel" || cbName == "onClose" 
		|| cbName == "onFailClose" || cbName == "onResultBack" || cbName == "onSetProp" || cbName == "onReturn")
	{
		sgSoundManager()->playMusicEffect(SOUND_CLICK_BACK);
	}
	else
	{
		sgSoundManager()->playMusicEffect(SOUND_CLICK_COMMON);
	}
	onMenuItemAction(cbName, sender,type);

}

void PageBase::_onMenuItemClicked(Ref* sender)
{
	cocos2d::ui::Widget* widget = dynamic_cast<cocos2d::ui::Widget*>(sender);
	if (!widget)
		return;

	std::string cbName = widget->getCallbackName();
	if (cbName == "onExit" || cbName == "onBack" || cbName == "onCancel" || cbName == "onClose"
		|| cbName == "onFailClose" || cbName == "onResultBack" || cbName == "onSetProp" || cbName == "onReturn")
	{
		sgSoundManager()->playMusicEffect(SOUND_CLICK_BACK);
	}
	else if (cbName != "onGame")
	{
		sgSoundManager()->playMusicEffect(SOUND_CLICK_COMMON);
	}

	onMenuItemAction(cbName, sender);

}

void PageBase::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event)
{
	if (keyCode == EventKeyboard::KeyCode::KEY_BACK)
	{
		CCLOG("[trace]onKeyReleased keyCode KEY_BACK, save file end!");
	}


}

