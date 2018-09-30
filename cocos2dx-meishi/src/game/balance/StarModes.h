
#pragma once
#include "Constant.h"
#include "Message.h"
#include "game/BattleInfo.h"


class BaseStar : public MessageHandler
{
public:

	BaseStar() : mEndSuccess(-1)
	{
	}

	virtual ~BaseStar();

	virtual bool init(std::stringstream& ssparams);
	virtual void update(){}
	virtual eStarTag getStarTag() = 0;
	virtual std::string getStarText() = 0;

	virtual int		getEndTag() { return mEndSuccess; }
	virtual void	setEndTag(int statu);
	virtual void	startGameTime(){};

	virtual std::string getIconString(){ return "ui/LevelDetail/LevelDetail_blei.png"; };
protected:

	void	registerMessageType();
	std::string getStarTextByTag(eTextTagStar);

	int		param1;
	std::string params;
	// 初始：-1  未达成：0  已达成：1
	int		mEndSuccess;
};

class StarFactory
{
public:
	virtual BaseStar* create(std::stringstream& ssparams) = 0;
};


#define DEFINE_STAR_FACTORY(_NAME_)	\
class _NAME_##Factory : public StarFactory	\
{\
public:	\
virtual BaseStar* create(std::stringstream& ssparams){\
_NAME_* pret = new _NAME_();\
if (pret && pret->init(ssparams)){\
return pret;\
}\
return NULL;\
}\
};



// 在N秒内取得胜利
class StarSuccessInN : public BaseStar
{
public:
	~StarSuccessInN();
	virtual bool init(std::stringstream& ssparams);
	virtual void update();
	virtual void	startGameTime();
	virtual eStarTag getStarTag() { return STAR_TAG_SUCCESS_IN_N; }
	virtual void onReceiveMassage(GameMessage * message);
	virtual std::string getStarText();
	virtual std::string getIconString(){ return "ui/LevelDetail/LevelDetail_sl.png"; };

};
DEFINE_STAR_FACTORY(StarSuccessInN);

// 任意敌人占领的建筑数量不高于N
class EnemyBuildsLN : public BaseStar
{
public: 
	virtual std::string getIconString(){ return "ui/LevelDetail/LevelDetail_blei.png"; };
	virtual eStarTag getStarTag() { return STAR_TAG_ENE_BUILD_L_N; }
	virtual void onReceiveMassage(GameMessage * message);
	virtual std::string getStarText();
};
DEFINE_STAR_FACTORY(EnemyBuildsLN);

// 敌方阵营从未占领指定的建筑
class StarBuildNoDestroy : public BaseStar
{
public:
	virtual bool init(std::stringstream& ssparams);
	virtual eStarTag getStarTag() { return STAR_TAG_BUILD_NODESTROY; }
	virtual void onReceiveMassage(GameMessage * message);
	virtual std::string getStarText();

protected:
	int param[5];
};
DEFINE_STAR_FACTORY(StarBuildNoDestroy);


// 剩余N个中立阵营的建筑数量
class StarLastNeutralBuilds : public BaseStar
{
public:
	virtual eStarTag getStarTag() { return STAR_TAG_NEUTRAL_N; }
	virtual void onReceiveMassage(GameMessage * message);
	virtual std::string getStarText();
};

DEFINE_STAR_FACTORY(StarLastNeutralBuilds);

// 任何一方兵力上限不超过N
class StarNoPeopleLN : public BaseStar
{
public:
	virtual std::string getIconString(){ return "ui/LevelDetail/LevelDetail_bl.png"; };
	virtual eStarTag getStarTag() { return STAR_TAG_NO_PEOP_MN; }
	virtual void onReceiveMassage(GameMessage * message);
	virtual std::string getStarText();
};
DEFINE_STAR_FACTORY(StarNoPeopleLN);

// 胜利时，占领指定的建筑
class StarGetTarBuilds : public BaseStar
{
public:
	virtual bool init(std::stringstream& ssparams);
	virtual eStarTag getStarTag() { return STAR_TAG_TAR_BUILD; }
	virtual void onReceiveMassage(GameMessage * message);
	virtual std::string getStarText();

	int param[5];
};

DEFINE_STAR_FACTORY(StarGetTarBuilds);


// 己方兵力超过敌方兵力N
class StarOwnPeopleMEneN : public BaseStar
{
public:
	virtual std::string getIconString(){ return "ui/LevelDetail/LevelDetail_bl.png"; };
	virtual eStarTag getStarTag() { return STAR_TAG_MORE_ENE_N; }
	virtual void onReceiveMassage(GameMessage * message);
	virtual std::string getStarText();

};
DEFINE_STAR_FACTORY(StarOwnPeopleMEneN);

// 己方兵力不超N
class StarOwnPeopleLN : public BaseStar
{
public:
	virtual std::string getIconString(){ return "ui/LevelDetail/LevelDetail_bl.png"; };
	virtual eStarTag getStarTag() { return STAR_TAG_PEOPLE_L_N; }
	virtual void onReceiveMassage(GameMessage * message);
	virtual std::string getStarText();

};
DEFINE_STAR_FACTORY(StarOwnPeopleLN);

// 己方兵力超N
class StarOwnPeopleMN : public BaseStar
{
public:
	virtual std::string getIconString(){ return "ui/LevelDetail/LevelDetail_bl.png"; };
	virtual eStarTag getStarTag() { return STAR_TAG_PEOPLE_M_N; }
	virtual void onReceiveMassage(GameMessage * message);
	virtual std::string getStarText();
};
DEFINE_STAR_FACTORY(StarOwnPeopleMN);


// 敌方兵力不超N
class StarEnePeopleLN : public BaseStar
{
public:
	virtual std::string getIconString(){ return "ui/LevelDetail/LevelDetail_bl.png"; };
	virtual eStarTag getStarTag() { return STAR_ENE_PEOPLE_L_N; }
	virtual void onReceiveMassage(GameMessage * message);
	virtual std::string getStarText();

};
DEFINE_STAR_FACTORY(StarEnePeopleLN);

// 敌方兵力超N
class StarEnePeopleMN : public BaseStar
{
public:
	virtual std::string getIconString(){ return "ui/LevelDetail/LevelDetail_bl.png"; };
	virtual eStarTag getStarTag() { return STAR_ENE_PEOPLE_M_N; }
	virtual void onReceiveMassage(GameMessage * message);
	virtual std::string getStarText();

};
DEFINE_STAR_FACTORY(StarEnePeopleMN);


// 己方初始建筑丢失不大于N
class StarIniBuildLostLN : public BaseStar
{
public:
	StarIniBuildLostLN()
	{
		mInitBuildLost = 0;
	}
	bool init(std::stringstream& ssparams);
	virtual eStarTag getStarTag() { return STAR_TAG_BUILD_LOST; }
	virtual void onReceiveMassage(GameMessage * message);
	virtual std::string getStarText();

protected:

	int		mInitBuildLost;
	MapBulidList mMineBuilds;
};
DEFINE_STAR_FACTORY(StarIniBuildLostLN);

// 己方拥有的建筑数量
class StarOwnBuildMN : public BaseStar
{
public:
	virtual eStarTag getStarTag() { return STAR_TAG_MY_BUILD_M_N; }
	virtual void onReceiveMassage(GameMessage * message);
	virtual std::string getStarText();
};
DEFINE_STAR_FACTORY(StarOwnBuildMN);

// 胜利时己方剩余的兵力
class StarSuccLastPeopleMN : public BaseStar
{
public:
	virtual std::string getIconString(){ return "ui/LevelDetail/LevelDetail_bl.png"; };
	virtual eStarTag getStarTag() { return STAR_TAG_SUCC_LAST_MN; }
	virtual void onReceiveMassage(GameMessage * message);
	virtual std::string getStarText();
};
DEFINE_STAR_FACTORY(StarSuccLastPeopleMN);

// 打野次数
class StarMonsterMN : public BaseStar
{
public:
	bool init(std::stringstream& ssparams);
	virtual eStarTag getStarTag() { return STAR_TAG_MONSTER_TIMES; }
	virtual void onReceiveMassage(GameMessage * message);
	virtual std::string getStarText();

	CC_SYNTHESIZE(int, mMonsterTimes, MonsterTimes);
};
DEFINE_STAR_FACTORY(StarMonsterMN);


// 不升级初始建筑
class StarNoBuildUpgrade : public BaseStar
{
public:
	virtual bool init(std::stringstream& ssparams);
	virtual eStarTag getStarTag() { return STAR_TAG_BUILD_UPGRADE; }
	virtual void onReceiveMassage(GameMessage * message);
	virtual std::string getStarText();

protected:

	MapBulidList mMineBuilds;
};
DEFINE_STAR_FACTORY(StarNoBuildUpgrade);

// 标星建筑不改变阵营时，占领标框的建筑
class StarNoDestroyGetTar : public BaseStar
{
public:
	virtual bool init(std::stringstream& ssparams);
	virtual eStarTag getStarTag() { return STAR_TAG_OCampNoDestroy_GetTar; }
	virtual void onReceiveMassage(GameMessage * message);
	virtual std::string getStarText();

	int param[5];
	int param2[5];
};
DEFINE_STAR_FACTORY(StarNoDestroyGetTar);

// 士气达到指定值
class StarMoraleMN : public BaseStar
{
public:
	virtual eStarTag getStarTag() { return STAR_TAG_MORALE_MN; }
	virtual void onReceiveMassage(GameMessage * message);
	virtual std::string getStarText();
};
DEFINE_STAR_FACTORY(StarMoraleMN);
