/* VideoLayer.h
* binks
* 2015_10_09
*/

#ifndef __VIDEOLAYER_H__
#define __VIDEOLAYER_H__

#include "cocos2d.h"

#if (CC_PLATFORM_ANDROID == CC_TARGET_PLATFORM)
#include "UIVideoPlayer.h"

USING_NS_CC;
using namespace cocos2d::ui;
using namespace cocos2d::experimental::ui;

class VideoLayer : public Layer
{
public:
	virtual bool init();

	CREATE_FUNC(VideoLayer);

	cocos2d::experimental::ui::VideoPlayer* mVideoPlayer;
	void videoEventCallback(cocos2d::Ref* sender, cocos2d::experimental::ui::VideoPlayer::EventType eventType);
private:

};
#endif

#endif
