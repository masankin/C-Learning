#pragma once

#include "cocos2d.h"
#include "cocos-ext.h"

#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
using namespace ui;
using namespace cocostudio;

USING_NS_CC;
USING_NS_CC_EXT;

class LabelRotation:public ui::Text
{
public:
    LabelRotation();
    virtual ~LabelRotation();

    CREATE_FUNC(LabelRotation);
    
	ui::Text* m_pText;
    static LabelRotation * create(ui::Text* label);
    
    static int getNumberDigit(int num);//获取数字位数

};
