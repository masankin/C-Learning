#pragma once

#include "cocos2d.h"

USING_NS_CC;

class LogoScene: public cocos2d::Scene
{
public:

	CREATE_FUNC(LogoScene);

   virtual bool init();

   void playLogo();

   void playFinish(Node *node);

   void onEnter();
   void onExit();
private:
	LogoScene();
	~LogoScene();

	Node* mRoot;

};

