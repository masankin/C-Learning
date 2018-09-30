
#include "VideoManager.h"

VideoManager::VideoManager()
{	
	mVideoLayer = nullptr;
	mVideoState = VIDEO_COMPLETED;
}

VideoManager::~VideoManager()
{

}

void VideoManager::setVideoState(eVideoState state)
{
	mVideoState = state;
}

VideoManager::eVideoState VideoManager::getVideoState()
{
	return mVideoState;
}

void VideoManager::setVideoLayer(Layer* layer)
{
	mVideoLayer = layer;
}

Layer* VideoManager::getVideoLayer()
{
	return mVideoLayer;
}

void VideoManager::removeVideoLayer()
{	
	
	if (mVideoLayer && mVideoLayer->getReferenceCount() == 1)
	{
		int count = mVideoLayer->getReferenceCount();

		mVideoLayer->removeFromParent();
		mVideoLayer = nullptr;
	}
	
	
}
