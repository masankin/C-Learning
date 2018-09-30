#pragma  once

#include "cocostudio/ActionTimeline/CSLoader.h"
#include "cocostudio/WidgetCallBackHandlerProtocol.h"
#include "cocostudio/WidgetReader/NodeReader/NodeReader.h"
#include "cocostudio/CCObjectExtensionData.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
#include "core/Singleton.h"
#include "ui/UIHelper.h"

#include "core/Singleton.h"

USING_NS_CC;
using namespace ui;


class PageBase :public Layer
{
public:
	PageBase() :mRoot(0){};
	~PageBase(){};

	virtual bool init();
//
//
//	virtual cocos2d::ui::Widget::ccWidgetTouchCallback onLocateTouchCallback(const std::string &callBackName){ return nullptr; };
//	virtual cocos2d::ui::Widget::ccWidgetClickCallback onLocateClickCallback(const std::string &callBackName);
//	virtual cocos2d::ui::Widget::ccWidgetEventCallback onLocateEventCallback(const std::string &callBackName){ return nullptr; };
//
//
	virtual void onEnterTransitionDidFinish();
	virtual void onExitTransitionDidStart();

	virtual void onLocateClickCallback(Node* parent);

	virtual void onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event);


	virtual void onShow(){};
	virtual void onClose(){};

	CC_SYNTHESIZE(Node*, mRoot, UIRoot);

protected:

	void	_onMenuItemClicked(cocos2d::Ref*);
	void	_onMenuItemTouched(Ref*, Widget::TouchEventType);

	virtual void onMenuItemAction(std::string, cocos2d::Ref*){};
	virtual void onMenuItemAction(std::string, cocos2d::Ref*, Widget::TouchEventType){};
	

};


#define GET_USERDATA(node)\
	((cocostudio::ObjectExtensionData*)node->getUserObject())->getCustomProperty()
