#ifndef _GLYPHBASEDATA_H
#define _GLYPHBASEDATA_H

#include "cocos2d.h"

class GlyphBaseData : public cocos2d::Ref 
{
public:
	static GlyphBaseData* create();

	bool mIsSpiritAffected;
	float mMainAttr;
	float mViceAttr;
	int mLevel;
	std::vector<float> mBuildLevelAffectedAttrs;
	std::string mBattleTip;

protected:
	GlyphBaseData();
	virtual ~GlyphBaseData();
};

#endif
