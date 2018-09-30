
#ifndef __SOUND_MAMAGER__
#define __SOUND_MAMAGER__

#include <list>
#include "Constant.h"
#include <string>

class SoundManager
{
public:
	SoundManager();
	~SoundManager();

	//Effect
	unsigned int playMusicEffect(eMusicType type, bool isLoop = false);

	void	pauseMusicEffect(unsigned int soundId);

	void	resumeMusicEffect(unsigned int soundId);

	void	stopMusicEffect(unsigned int soundId);

	void	stopAllMusicEffects();

	void	delayPlayEffect(eMusicType type, float delayTime);

	void	setEffectToLastState();

	void    pauseAllEffects();

	void    resumeAllEffects();

	//battle Effect
	void	stopBattleEffect();

	void	beganBattleEffect();

	//bg
	void	playMusicBg(int type, int levelId = 0);

	void    playLastMusicBg();

	void	stopMusicBg();

	//state
	void	setIsPlayMusicEffect(bool isPlay);

	bool	getIsPlayMusicEffect();

	void	setIsPlayMusicBg(bool isPlay);

	bool	getIsPlayMusicBg();

	//volume
	float	getBackgroundMusicVolume();

	void	setBackgroundMusicVolume(float volume);

	float	getEffectsVolume();

	void	setEffectsVolume(float volume);

	//update
	void	update(float delta);
private:

	bool bCountDown_waitEnd;
	bool bCountDown_waitBegin;
	float mElapsedTime;
	std::string mMusicName;

	int mTime_waitBegin;
	int mTime_waitEnd;
	int mWaitTime;
	int mBgMusicType;

	bool bPlayMusicEffect;
	bool bPlayMusicBg;

	struct delayEffectData
	{
		eMusicType mType;
		std::string mKey;

		delayEffectData(eMusicType type, std::string key)
		{
			mType = type;
			mKey = key;
		}
	};

	int mDelayEffectIndex;
	std::list<delayEffectData> delayEffectList;

	CC_SYNTHESIZE(bool, m_bCanPlayMusic, CanPlayMusic);

	bool mLastEffectState;

	int mLastMusicBgType;

	bool mbPlayBattleEffect;
};

#define sgSoundManager() Singleton<SoundManager>::Get()

#endif
