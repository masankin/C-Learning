/**
 * Project Untitled
 */


#ifndef _GLYPHBATTLEMANAGER_H
#define _GLYPHBATTLEMANAGER_H

#include "Constant.h"
#include "battle/Glyph/Glyph.h"


class GlyphCampAgent;
class GlyphBattleManager 
{
public:
	typedef std::map<eCampType, GlyphCampAgent*> CampGlyphList;

public: 
	GlyphBattleManager();
	virtual ~GlyphBattleManager();

    void load(int checkptId);  
	void clean();
    void update(float dt);

    bool registAgent(GlyphCampAgent* agent);
	void unregistAgent(GlyphCampAgent* agent);
    
	Glyph* getCampGlyph(eCampType camp, Glyph::eGlyphType type);
	float getSpirit() const { return mSpirit; }

protected: 
	static const std::string TimeKeySpirit;

    float mSpirit;
	float mSpiritInc;
    CampGlyphList mCampGlyphList;
};

#endif //_GLYPHBATTLEMANAGER_H
