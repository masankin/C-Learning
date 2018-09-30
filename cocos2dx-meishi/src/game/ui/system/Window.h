
#ifndef _H_WINDOW_
#define _H_WINDOW_

#include "cocos2d.h"
#include "ui/UILayout.h"
#include "ui/UIWidget.h"

#include "game/ui//system/PageBase.h"

#include "cocostudio/WidgetCallBackHandlerProtocol.h"
#include <vector>
#include <map>

#include "Constant.h"

USING_NS_CC;
using namespace ui;
using namespace std;

//优先级
enum ePriority
{
	PRIORITY_WINDOW,
	PRIORITY_DIALOG,
	PRIORITY_DIALOG_HIGH,
	PRIORITY_DIALOG_MSG,
	PRIORITY_DIALOG_SYSTEM,
	PRIORITY_INFO,
};

class Window :public PageBase
{
public:

public:
	Window();
	Window(ePriority priority);
	Window(Widget* belongTo);
	Window(ePriority priority, Widget* showFrom);

	CREATE_FUNC(Window);

	virtual void onShow();
	virtual void onClose();
	virtual void afterClose();
	virtual void setBgOpacity(int opacity);
	Layout* getBgPanel();
private:
	static bool initOnceFlag;
	static void initOnce();
public:
	virtual void staticWindow(){};
	virtual void normalClose(){};

	ePriority getPriority();
	Widget* getShowFromLayer();

	static Window* Create(std::string pageName, ePageType pageType = MSG_FUC_BEGIN, const int luaType = 0);
	static Window* createWithName(std::string pageName, ePageType pageType, const int luaType );
	static void setAllWindowHide();
	static Window* getWindow(std::string pageName);
	static int getWindowSize();
	static int getShowingWindowSize();

	virtual void setShow(bool b);
	virtual bool isShow();


	void setWindowLayoutEnable(bool enable);

	bool isWindowLayoutEnable();

	virtual void addChildCenter(Node* node);

	static void showTextTip(const std::string, Point pPoint = Point::ZERO, Size pSize = Size(0.0, 0.0));//文字提示样式1

	//add by weihua
	void setPageType(ePageType pageType);
protected:
	virtual bool _initWindow(ePriority priority, Widget* showFrom);
	virtual void cleanUp();

	static void Add(Window* ui);
	static void Remove(Window* ui);

	static map<std::string, Window*>  CACHE_WINDOWS;
public:
	//static Widget* LAYER_WINDOW;
	//static Widget* LAYER_DIALOG;
	//static Widget* LAYER_INFO;
	static Widget* LAYER_UIMANAGER_LAYER;
protected:
	bool bIsShow;
	Node* pWindowLayout;
	ePriority mPriority;
	Widget* pShowFrom;
	ePageType mPageType;

	CC_SYNTHESIZE(int, mLuaType, LuaType);
	CC_SYNTHESIZE(std::string, mPageName, PageName);

	bool mIsCanClean;//关闭的时候是否释放自己
	bool mIsNoRelease;//是否永驻内存中
};

#endif
