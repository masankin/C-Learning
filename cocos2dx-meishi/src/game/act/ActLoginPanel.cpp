#include "ActLoginPanel.h"
#include "ActLoginLayout.h"

USING_NS_CC;
ActLoginPanel::ActLoginPanel(void)
{
}


ActLoginPanel::~ActLoginPanel(void)
{
}

bool ActLoginPanel::init()
{
	if (!Window::init())
	{
		return false;
	}
	//SpriteFrameCache::getInstance()->addSpriteFramesWithFile("ui/actSheet.plist");
	//SpriteFrameCache::getInstance()->addSpriteFramesWithFile("ui/general_plist.plist");
	//SpriteFrameCache::getInstance()->addSpriteFramesWithFile("ui/uiAtlas2.plist");
	//SpriteFrameCache::getInstance()->addSpriteFramesWithFile("ui/ranking_plist.plist");

	mRoot = CSLoader::getInstance()->createNode("ui/ActLoginPanel.csb");
	mRoot->setAnchorPoint(Vec2(0.5, 0.5));

	mActLayout = ActLoginLayout::create();	
	dynamic_cast<Node*>(mRoot->getChildByName("Node_2"))->addChild(mActLayout);
	
	mRoot->setPositionY(-70);
	this->addChild(mRoot);

	setPopType(E_POP_TYPE_BIG);
	onLocateClickCallback(mRoot);
	
	return true;
}

void ActLoginPanel::onEnter()
{
	Window::onEnter();
	Node* tNode = dynamic_cast<ActLoginLayout* >(mActLayout)->mGuideNode;
	tNode->convertToWorldSpace(mRoot->getPosition()); 
} 

void ActLoginPanel::onMenuItemAction(std::string name, cocos2d::Ref* ref)
{
	if (name == "onClose")
	{
		this->setShow(false);
	}

}

void ActLoginPanel::updateUI()
{
	mActLayout->updateInfo();
}
