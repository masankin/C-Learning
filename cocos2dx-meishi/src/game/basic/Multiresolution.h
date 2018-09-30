#ifndef Multiresolution_h__
#define Multiresolution_h__

#include "cocos2d.h"
USING_NS_CC;

class Multiresolution
{
public:
	Multiresolution();
	~Multiresolution();

	static void setDesignResolutionSize(float fWidth, float fHeight);

	static const Size& getInnerSize();

	static Size getInnerSizeInPixels();

	static Point getInnerOrigin();

private:

};

#endif // Multiresolution_h__
