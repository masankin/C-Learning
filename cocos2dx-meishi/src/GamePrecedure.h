#pragma once


#include "Constant.h"
#include "Singleton.h"

#include "cocos2d.h"
#include "game/battle/SoliderView.h"

USING_NS_CC;
 
struct GameInfo
{
	int checkpt;// ¹Ø¿¨id
	std::string mapName;
	int mapId;
	std::string mapBgName;
	std::string mapObstacleName;
	bool bUpArrow;  //??????
	bool bEnemyInfo;// ????????
	bool bHelpInfo;// ????????
	bool bTipsProp;

}; 


class GamePrecedure
{
public:

	GamePrecedure();
	~GamePrecedure();

	bool init();
	virtual void update(float delta);


	const GameInfo&	getCurMapInfo()
	{
		return mCurrentInfo;
	}

	bool	preEnterGame();

	void	preEndGame();

	void	enterGame();

	void	destroy();

	bool	selectLevel(int levelId);

	bool	selectPvpMap( int mapID);

	bool	nextLevel();

	void	setGameIng(bool bIng);

	bool	getGameIng() { return isGameIng; }

	void	enableGameRunning(bool open);

	void	addDieEffect(SoliderView * soliderView);

	void	animationCallback(cocostudio::Armature * pAr, cocostudio::MovementEventType type, const std::string& aniName);

	void	setEnterGame(bool isEnter);
	bool	getEnterGame();
protected:

	GameInfo mCurrentInfo;

	bool isGameIng;
	bool mIsClean;
	bool mIsEnter;

	CC_SYNTHESIZE(bool, mInCheckPoint, IncheckPoint);
};

#define sgGamePrecedure() Singleton<GamePrecedure>::Get()
