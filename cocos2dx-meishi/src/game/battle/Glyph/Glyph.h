#ifndef _GLYPH_H
#define _GLYPH_H

#include "cocos2d.h"
#include "Constant.h"


class GlyphBaseData;

class Glyph: public cocos2d::Ref {
public: 
	enum eGlyphType
	{
		E_GLYPH_MIN,

		//攻击雕文ID
		E_GLYPH_ATK_BEG = 100,

		E_GLYPH_ATK_POWER = 101,				//练兵 部队攻击力提升%d
		E_GLYPH_ATK_INSTITUTE = 102,			//暴击 每个研究所攻击力提高%d
		E_GLYPH_ATK_TIME = 103,					//锋利 每%d秒攻击力增加%d%
		E_GLYPH_ATK_TURRET = 104,				//蓄锐 炮塔攻速提升%d%%
		E_GLYPH_ATK_KILL = 105,					//---- 
		
		E_GLYPH_ATK_END,

		//防御雕文ID
		E_GLYPH_DEF_BEG = 200,

		E_GLYPH_DEF_DEFENSE = 201,				//铁壁 部队防御力提升%d
		E_GLYPH_DEF_INSTITUTE = 202,			//韧性 每个研究所防御力提高%d%%
		E_GLYPH_DEF_TIME = 203,					//坚守 每%d秒防御增加%d
		E_GLYPH_DEF_DEAD = 204,					//---- 

		E_GLYPH_DEF_END,

		//速度雕文ID
		E_GLYPH_SPD_BEG = 300,

		E_GLYPH_SPD_MOVE = 301,					//疾行 部队移动速度提升%d
		E_GLYPH_SPD_UP = 302,					//建设 建筑升级速度提升%d%%
		E_GLYPH_SPD_PRODUCE = 303,				//募集 兵营产兵速度提升%d%%
		E_GLYPH_SPD_CREEPS = 304,				//出猎 向野怪移动速度提升%d%%
		E_GLYPH_SPD_ASSAULT = 305,				//奇袭 向敌方兵力少于%d的建筑移动时，移动速度提升%d%%
		E_GLYPH_SPD_ASSIST = 306,				//驰援 向友方兵力少于%d的建筑移动时，移动速度提升%d%%

		E_GLYPH_SPD_END,

		//兵力雕文ID
		E_GLYPH_PPL_BEG = 400,

		E_GLYPH_PPL_INIT = 401,					//支援 初始随机%d个建筑兵力增加%d个
		E_GLYPH_PPL_MAX = 402,					//强军 建筑产兵上限增加%d
		E_GLYPH_PPL_HOLDLIMIT = 403,			//扩营 马厩的容量提升N
		E_GLYPH_PPL_UP = 404,					//受降 升级回复%d个小兵
		E_GLYPH_PPL_PRODUCE = 405,				//众望 每%d秒增加%d个小兵(每次产兵有%d%%增加%d个小兵)
		E_GLYPH_PPL_OCCUPY = 406,				//舰仓 占领建筑后，该建筑回复%d个兵
		
		E_GLYPH_PPL_END,

		//士气雕文ID
		E_GLYPH_MRL_BEG = 500,

		E_GLYPH_MRL_ADD = 501,					//激励 每次获取士气提升%d%%
		E_GLYPH_MRL_DEC = 502,					//鼓舞 士气衰减减少%d%%
		E_GLYPH_MRL_TIME = 503,					//稳军 每%d秒士气增加%d点

		E_GLYPH_MRL_END,

		E_GLYPH_MAX,
	};

public:
	static Glyph* create(eGlyphType type, GlyphBaseData* data);

	eGlyphType getType();
    GlyphBaseData* getData();

	bool isSpiritAffected() const;
	float getDataMainAttr() const;
	float getDataViceAttr() const;
	bool isAffectedByBuildLevel() const;
	float getDataBuildAffectAttr(int buildLevel) const;
	std::string getDataBattleTip() const;

	float getValue() const;
	float getValueAsAddition() const{ return mValue; }
	float getValueAsPercent() const { return mValue / 100.0f; }

	void onSpiritChange(float spirit);
	void onIntervalTimeout(eCampType campType);

protected:
	Glyph();
	virtual ~Glyph();

	bool init(eGlyphType type, GlyphBaseData* data);

protected: 
	eGlyphType mType;
    GlyphBaseData* mData;

	mutable float mValue;
};

#endif //_GLYPH_H
