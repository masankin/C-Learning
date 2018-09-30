
#ifndef __FUNC_FOR_LUA_H__
#define __FUNC_FOR_LUA_H__

#include "Singleton.h"
#include "cocos2d.h"
#include "cocostudio/CocoStudio.h"

USING_NS_CC;
using namespace ui;

class FuncForLua
{
public:
	FuncForLua();
	~FuncForLua();

	bool ConvertToWidget(Node * node);

private:

};

#define sgFuncForLua() Singleton<FuncForLua>::Get();

#endif
