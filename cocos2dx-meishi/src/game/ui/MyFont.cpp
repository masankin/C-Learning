#include "MyFont.h"
#include "ui/UIHelper.h"

#include "cocostudio/ActionTimeline/CSLoader.h"

#define  PI 3.1415926

MyFont* MyFont::create(std::string str)
{
	MyFont* pret = new MyFont();
	if (pret && pret->init(str))
	{
		return pret;
	}
	CC_SAFE_DELETE(pret);
	return NULL;
}

MyFont* MyFont::create()
{
	MyFont* pret = new MyFont();
	if (pret && pret->init())
	{
		return pret;
	}
	CC_SAFE_DELETE(pret);
	return NULL;
}

bool MyFont::init(std::string str)
{
	//CCSpriteFrameCache::getInstance()->addSpriteFramesWithFile("uiAtlas.plist");

	mRoot = CSLoader::getInstance()->createNode("MyFont.csb");
	if (!mRoot)
	{
		CCLOG("[error] ArrowWidget::initUI load csb error!");
		return false;
	}

	mRoot->setAnchorPoint(Vec2(0.5, 0.5));

	mShade = dynamic_cast<ui::TextBMFont*>(mRoot->getChildByName("mShade"));
	mFont = dynamic_cast<ui::TextBMFont*>(mRoot->getChildByName("mFont"));
	mString = mFont->getString();

	setText(str);

	this->addChild(mRoot);
	return true;
}

bool MyFont::init()
{
	//CCSpriteFrameCache::getInstance()->addSpriteFramesWithFile("uiAtlas.plist");

	mRoot = CSLoader::getInstance()->createNode("MyFont.csb");
	if (!mRoot)
	{
		CCLOG("[error] ArrowWidget::initUI load csb error!");
		return false;
	}

	mRoot->setAnchorPoint(Vec2(0.5, 0.5));

	mShade = dynamic_cast<ui::TextBMFont*>(mRoot->getChildByName("mShade"));
	mFont = dynamic_cast<ui::TextBMFont*>(mRoot->getChildByName("mFont"));
	mString = mFont->getString();

	this->addChild(mRoot);
	return true;
}

void MyFont::setText(std::string str)
{
	if (!mShade || !mFont) return;

	mShade->setText(str);
	mFont->setText(str);
	mString = str;
}

void MyFont::setScale(float scale)
{
	if (!mShade || !mFont) return;

	mShade->setScale(scale);
	mFont->setScale(scale);
}

void MyFont::setShadeColor(cocos2d::Color3B color)
{
	if (!mShade) return;
	mShade->setColor(color);

}

std::string& MyFont::getText()
{
	return mString;
}

void MyFont::setTextColor(cocos2d::Color3B color)
{
	if (!mFont) return;
	mFont->setColor(color);
}
