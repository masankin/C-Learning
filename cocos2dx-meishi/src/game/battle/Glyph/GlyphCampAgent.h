/**
 * Project Untitled
 */


#ifndef _GLYPHCAMPAGENT_H
#define _GLYPHCAMPAGENT_H

#include "cocos2d.h"
#include "Constant.h"
#include "Glyph.h"

class Camp;
class GlyphBattleManager;

class GlyphCampAgent: public cocos2d::Ref 
{
public:
	typedef std::map<Glyph::eGlyphType, Glyph*> GlyphList;
	typedef std::map<std::string, Glyph::eGlyphType> GlyphTimerList;

public: 
	static GlyphCampAgent* create(Camp* owner);
	
    bool load();
	void clean();
	void update(float dt);

	eCampType getCamp();
	Glyph* getGlyph(Glyph::eGlyphType type);

	int getAtkLevel() const { return mAtkLvCounter; }
	int getDefLevel() const { return mDefLvCounter; }
	int getSpdLevel() const { return mSpdLvCounter; }
	int getPplLevel() const { return mPplLvCounter; }
	int getMrlLevel() const { return mMrlLvCounter; }

	void onSpiritChange(float spirit);

protected:
	GlyphCampAgent();
	virtual ~GlyphCampAgent();

	virtual bool init(Camp* owner);

	void onLuaGetGlyphComplete(int glyphId, int &lvCounter, lua_State* tolua_S, int argc);

protected: 
    Camp* mOwner;

	GlyphList mGlyphList;
	GlyphTimerList mIntervalTimerList;

	int mAtkLvCounter;
	int mDefLvCounter;
	int mSpdLvCounter;
	int mPplLvCounter;
	int mMrlLvCounter;
};

#endif //_GLYPHCAMPAGENT_H
