#pragma once
#include "ui/system/Window.h"

class ExpressionPanel:public Window{
public:
    
	ExpressionPanel();
	~ExpressionPanel();

	virtual bool init();

	virtual void onMenuItemAction(std::string, cocos2d::Ref*);

	void onCreateExpression();

    CREATE_FUNC(ExpressionPanel);

	void callBack(cocos2d::Ref*);
private:
	Node* mNode_1;
	int mExpressionID;
};
