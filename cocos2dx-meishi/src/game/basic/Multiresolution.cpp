#include "Multiresolution.h"

static Size s_stInnerSizeInPoints = Size(0, 0);

static float s_fContentScaleFactor = 0.0f;

Multiresolution::Multiresolution()

{
}

Multiresolution::~Multiresolution()
{
}

void Multiresolution::setDesignResolutionSize(float width, float height) {
	s_stInnerSizeInPoints.setSize(width, height);
	GLView *pEGLView = Director::getInstance()->getOpenGLView();
	Size frameSize = pEGLView->getFrameSize();

	float scale = 0.0f;
	float scaleX = frameSize.width / width;
	float scaleY = frameSize.height / height;

	if(scaleX > scaleY) {
		scale = scaleX / scaleY;
	} else {
		scale = scaleY / scaleX;
	}

	s_fContentScaleFactor = scale;
	//pEGLView->setDesignResolutionSize(width * scale, height * scale, ResolutionPolicy::NO_BORDER);


	pEGLView->setDesignResolutionSize(1280, 720, ResolutionPolicy::SHOW_ALL);


}

const Size& Multiresolution::getInnerSize() {
	return s_stInnerSizeInPoints;
}

Size Multiresolution::getInnerSizeInPixels() {

	GLView *pEGLView = Director::getInstance()->getOpenGLView();
	Size frameSize = pEGLView->getFrameSize();
	Size visibleSize = pEGLView->getVisibleSize();

	float scale = frameSize.width / visibleSize.width;

	return Size(s_stInnerSizeInPoints.width * scale, s_stInnerSizeInPoints.height * scale);
}

Point Multiresolution::getInnerOrigin() {
	Size visibleSize = Director::getInstance()->getVisibleSize();
	Point visibleOriginPos = Director::getInstance()->getVisibleOrigin();

	float posX = visibleOriginPos.x + (visibleSize.width - s_stInnerSizeInPoints.width) / 2;
	float posY = visibleOriginPos.y + (visibleSize.height - s_stInnerSizeInPoints.height) / 2;

	return Vec2(posX, posY);
}
