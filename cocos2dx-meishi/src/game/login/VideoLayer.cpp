
#include "VideoLayer.h"
#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)

#include "VideoManager.h"
#include "update/UpdateScene.h"
#include "ui/PageFunction.h"
#include "update/UpdateMgrImp.h"
#include <string>

bool VideoLayer::init()
{
	//this->retain();

	sgVideoManager()->setVideoLayer(this);
	sgVideoManager()->setVideoState(VideoManager::VIDEO_PLAYING);

	Rect _visibleRect = Director::getInstance()->getOpenGLView()->getVisibleRect();

	auto centerPos = Vec2(_visibleRect.origin.x + _visibleRect.size.width / 2, _visibleRect.origin.y + _visibleRect.size.height / 2);

	auto widgetSize = Director::getInstance()->getWinSize();
	mVideoPlayer = VideoPlayer::create();
	mVideoPlayer->setPosition(centerPos);
	mVideoPlayer->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	int addPixel = 20;
	mVideoPlayer->setContentSize(Size(widgetSize.width*((widgetSize.height+addPixel)/widgetSize.height), widgetSize.height+addPixel));	//height加10个像素，width等级增加
	mVideoPlayer->setFullScreenEnabled(false);
	this->addChild(mVideoPlayer, 1);

	std::string newResPath = 	UpdateMgrImp::getInstance()->GetExtractPath() + "/Resources/" + "piantou.mp4";
	std::string oldResPath = "piantou.mp4";

	if(FileUtils::getInstance()->isFileExist(newResPath))
	{
		mVideoPlayer->setFileName(newResPath);
	}
	else
	{
		mVideoPlayer->setFileName(oldResPath);
	}

	mVideoPlayer->play();
 	Scene * scene = Director::getInstance()->getRunningScene();
	mVideoPlayer->addEventListener(CC_CALLBACK_2(VideoLayer::videoEventCallback, this));

	//layer
	CCSize size = Director::getInstance()->getWinSize();

	auto Layer = LayerColor::create(Color4B(0xFF, 0x00, 0x00, 0x80), size.width, size.height);
	Layer->setPosition(0, 0);
	//Layer->setOpacity(0);
	//mVideoPlayer->addChild(Layer, 1000);
	Layer->setGlobalZOrder(10000);
	//add touch event
	auto _eventDispatcher = Director::getInstance()->getEventDispatcher();
	auto listener = EventListenerTouchOneByOne::create();

	listener->setSwallowTouches(false);
	listener->onTouchBegan = [](Touch* touch, Event* event){

		return true;
	};

	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, Layer);

	return true;
}

void VideoLayer::videoEventCallback(cocos2d::Ref* sender, cocos2d::experimental::ui::VideoPlayer::EventType eventType)
{	
	CCLOG("xfx video state  %d ", eventType);

	switch (eventType) {
	case VideoPlayer::EventType::PLAYING:
		CCLOG("xfx video playing");
		sgVideoManager()->setVideoState(VideoManager::VIDEO_PLAYING);

#ifdef MGAME_PLATFORM_MPLUS
		PageFunction::Get()->gotoFunction(MSG_FUC_LOGIN);
#else
		//if (UpdateMgrImp::getInstance()->getIsUpdateSceneShow() == false)
		//{
		//	CCLOG("xfx replaceScene UpdateScene");
		//	Director::getInstance()->replaceScene(UpdateScene::create());
		//	UpdateMgrImp::getInstance()->setIsUpdateSceneShow(true);
		//}
#endif

		break;
	case VideoPlayer::EventType::PAUSED:
		mVideoPlayer->removeFromParent();
		sgVideoManager()->setVideoState(VideoManager::VIDEO_COMPLETED);	//暂停后直接结束播放
		break;
	case VideoPlayer::EventType::COMPLETED:
		mVideoPlayer->removeFromParent();
		sgVideoManager()->setVideoState(VideoManager::VIDEO_COMPLETED);
		break;
	default:

		break;
	}
}

#endif
