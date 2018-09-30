
#include "LabelRotation.h"

LabelRotation::LabelRotation()
{
    m_pText = NULL;
}

LabelRotation::~LabelRotation()
{
    
}

LabelRotation * LabelRotation::create(ui::Text* label)
{
    LabelRotation* pRet = LabelRotation::create();
    
    std::string strLab = label->getString();
    int num = atoi(strLab.c_str());
    if (num == 0) {
        pRet->setFontName(label->getFontName());
        pRet->setFontSize(label->getFontSize());
        pRet->setAnchorPoint(label->getAnchorPoint());
        pRet->setColor(label->getColor());
        pRet->setString("0");
        return pRet;
    }
    int count = getNumberDigit(num);
    int space = 0;
    for (int i = 0; i < count; i++) {
        int singleNum = num%10;
        num /= 10;
		Text* singleLab = Text::create();
        singleLab->setFontName(label->getFontName());
//        singleLab->setAnchorPoint(label->getAnchorPoint());
        singleLab->setFontSize(label->getFontSize());
        singleLab->setColor(label->getColor());
        singleLab->setString(StringUtils::format("%d",singleNum));
        space = singleLab->getContentSize().width;
        singleLab->setPositionX(-i*singleLab->getContentSize().width + space*(count-1));
//        CCActionInterval* action = CCSequence::create(
//                                                      CCDelayTime::create(0.2*i),
//                                                      CCRotateBy::create(0.2f, 360),
//                                                      NULL);
        
        singleLab->setOpacity(0);
        ActionInterval* action1 = CCSequence::create(
                                                       CCDelayTime::create(0.05*i),
//                                                        CCMoveBy::create(0.0f, ccp(10,0)),
                                                           CCFadeIn::create(0.5),
                                                           NULL);
        ActionInterval* action2 = CCSequence::create(
                                                       CCDelayTime::create(0.05*i),
                                                           CCMoveBy::create(0.5f, ccp(60,0)),
                                                           CCDelayTime::create(0.05*(count-1-i)*2),
                                                           CCMoveBy::create(0.2f, ccp(-60,0)),
                                                           NULL);
        singleLab->runAction(action1);
        singleLab->runAction(action2);
        
//        singleLab->runAction(action);
        //   -1.05*i*singleLab->getContentSize().width +i*singleLab->getContentSize().width
        pRet->addChild(singleLab);
    }
//    pRet->setPositionX(-space*(count-1));
    return pRet;
}

int LabelRotation::getNumberDigit(int num)
{
    int count = 0;
    int tpmNum = num;
    while (tpmNum != 0)
    {
        count++;
        tpmNum /= 10;
    }
    return count;
}
