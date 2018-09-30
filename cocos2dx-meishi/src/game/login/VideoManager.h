/* VideoManager.h
* binks
* 2015_10_09
*/

#ifndef __VIDEOMANAGER_H__
#define __VIDEOMANAGER_H__

#include "Singleton.h"
#include "cocos2d.h"

USING_NS_CC;

class VideoManager
{
public:

	VideoManager();
	~VideoManager();

public:

	enum eVideoState
	{
		VIDEO_PLAYING,
		VIDEO_PAUSED,
		VIDEO_STOPPED,
		VIDEO_COMPLETED,
	};

	void setVideoState(eVideoState state);
	eVideoState getVideoState();

	void setVideoLayer(Layer* layer);
	Layer* getVideoLayer();

	void removeVideoLayer();
private:
	eVideoState mVideoState;
	Layer * mVideoLayer;
};

#define sgVideoManager() Singleton<VideoManager>::Get()

#endif
