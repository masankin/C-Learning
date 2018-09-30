#include "HeadIconUtils.h"
#include "Comm.h"
#if CC_PLATFORM_ANDROID ==  CC_TARGET_PLATFORM
#include "platform/android/jni/ChannelJni.h"
#endif
using namespace FriendSystem;

HeadIconUtils::HeadIconUtils()
{
#if CC_PLATFORM_ANDROID ==  CC_TARGET_PLATFORM
	Director::getInstance()->getEventDispatcher()->
		addCustomEventListener(CHANNEL_LOADIMAGE_SUCCESS, CC_CALLBACK_1(HeadIconUtils::loadImgCallback, this));
#endif
}


HeadIconUtils::~HeadIconUtils()
{
	Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(this);
}

void HeadIconUtils::changeIcon(Node* iconNode, const std::string iconId, int type)
{
	int iIcon = std::atoi(iconId.c_str());

	if (iIcon == 0 && iconId.size() > 1)//urlͼƬ
	{
		std::string liconStr = getPicMd5ByUrl(iconId, type);
		CCLOG("[trace] HeadIconUtils::changeIcon getPicMd5ByUrl iconStr = %s", liconStr.c_str());
		std::string fullpath = FileUtils::getInstance()->fullPathForFilename(liconStr);
		if (fullpath.size() == 0)
		{
			updateIcon(iconNode, PlayerIconName[0].c_str(),  false);//��
			CC_SAFE_RETAIN(iconNode);
			mChangeIconMap.insert(std::make_pair(iconId, iconNode));
#if CC_PLATFORM_ANDROID ==  CC_TARGET_PLATFORM
			onLoadUrlImage(iconId);
#endif
			return;
		}
		updateIcon(iconNode, liconStr);
		return;
	}
	if (iIcon <= HeadMaxCount)
	{
		std::string liconStr = PlayerIconName[iIcon].c_str();
		updateIcon(iconNode, liconStr, false);
	}else
		updateIcon(iconNode, PlayerIconName[0].c_str(), false);//��
}

void HeadIconUtils::updateIcon(Node* iconNode, const std::string iconStr, bool isUrlIcon)
{
	Sprite* castSp = dynamic_cast<Sprite*>(iconNode);
	if (castSp)
	{
		if (isUrlIcon)
		{
			castSp->setTexture(iconStr);
		}else
			castSp->setSpriteFrame(iconStr);
		return;
	}

	ImageView* castImg = dynamic_cast<ImageView*>(iconNode);
	if (castImg)
	{
		if (isUrlIcon)
		{
			castImg->loadTexture(iconStr);
		}else
			castImg->loadTexture(iconStr, Widget::TextureResType::PLIST);
		return;
	}

	Button* castBtn = dynamic_cast<Button*>(iconNode);
	if (castBtn)
	{
		if (isUrlIcon)
		{
			castBtn->loadTextureNormal(iconStr);
		}else
			castBtn->loadTextureNormal(iconStr, Widget::TextureResType::PLIST);
	}
	
}

void HeadIconUtils::loadImgCallback(cocos2d::EventCustom* customData)
{
	for (auto it = mChangeIconMap.begin(); it != mChangeIconMap.end();)
	{
		std::string fullpath = FileUtils::getInstance()->fullPathForFilename(it->first);
		if (fullpath.size() != 0)
		{
			updateIcon(it->second, it->first);
			CC_SAFE_RELEASE(it->second);
			it = mChangeIconMap.erase(it);
		}
		else
			it++;
	}
}
