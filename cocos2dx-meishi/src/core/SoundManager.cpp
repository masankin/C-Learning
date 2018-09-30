
#include "SoundManager.h"
#include "SimpleAudioEngine.h"
#include "data/TableManager.h"
#include "TimeCalculator.h"
#include "game/utils/GameDefault.h"
SoundManager::SoundManager() :mLastEffectState(true), mbPlayBattleEffect(true)
{	
	mBgMusicType = -1;
	bPlayMusicEffect = true;
	bPlayMusicBg = true;

	bCountDown_waitBegin = false;
	bCountDown_waitEnd = false;

	mElapsedTime = 0.0f;
	mTime_waitBegin = 0;
	mTime_waitEnd = 0;

	m_bCanPlayMusic = false;

	mDelayEffectIndex = 1;
	mLastMusicBgType = 0;
}

SoundManager::~SoundManager()
{

}

unsigned int SoundManager::playMusicEffect(eMusicType type, bool isLoop)
{
	unsigned int soundId = 0;
	if (getIsPlayMusicEffect() == true)
	{
		//是否播放战斗音效的逻辑特殊处理
		if (mbPlayBattleEffect == false)
		{
			int battleEffectArray[] = { -200, -199, -198, -197, -196, -195, -194, -188, -187, -186, -185, -184, -183, -182, -181, -180, -179, -178, -177, -176, -175, -174 };

			for (int i = 0; i < sizeof(battleEffectArray) / sizeof(battleEffectArray[0]); i++)
			{
				if (battleEffectArray[i] == type)
				{
					return soundId;
				}
			}
		}

		const MusicInfo* pAttr = sgTableManager()->getMusicInfoByID(type);
		if (pAttr)
		{
			std::string strFileName = pAttr->fileName;
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
			strFileName = strFileName.substr(0, strFileName.find_last_of(".")).append(".aac");
#endif
			soundId = CocosDenshion::SimpleAudioEngine::getInstance()->playEffect(strFileName.c_str(), isLoop);
		}	
	}

	return soundId;
}

void SoundManager::pauseMusicEffect(unsigned int soundId)
{
	CocosDenshion::SimpleAudioEngine::getInstance()->pauseEffect(soundId);
}

void SoundManager::resumeMusicEffect(unsigned int soundId)
{
	CocosDenshion::SimpleAudioEngine::getInstance()->resumeEffect(soundId);
}

void SoundManager::stopMusicEffect(unsigned int soundId)
{
	CocosDenshion::SimpleAudioEngine::getInstance()->stopEffect(soundId);
}

void SoundManager::stopAllMusicEffects()
{
	CocosDenshion::SimpleAudioEngine::getInstance()->stopAllEffects();
}

void SoundManager::delayPlayEffect(eMusicType type, float delayTime)
{
	std::string key = StringUtils::format("delayEffect%d", mDelayEffectIndex);
	mDelayEffectIndex += 1;

	sgTimeCalculator()->createTimeCalcultor(key, delayTime);

	delayEffectData data(type, key);
	delayEffectList.push_back(data);
}

void SoundManager::setEffectToLastState()
{
	setIsPlayMusicEffect(mLastEffectState);
}

void SoundManager::pauseAllEffects()
{
	CocosDenshion::SimpleAudioEngine::getInstance()->pauseAllEffects();
}

void SoundManager::resumeAllEffects()
{
	CocosDenshion::SimpleAudioEngine::getInstance()->resumeAllEffects();
}

void SoundManager::stopBattleEffect()
{
	mbPlayBattleEffect = false;
};

void SoundManager::beganBattleEffect()
{ 
	mbPlayBattleEffect = true; 
};

void SoundManager::playMusicBg(int type, int levelId)
{
	CocosDenshion::SimpleAudioEngine::getInstance()->stopBackgroundMusic(false);

	if (getIsPlayMusicBg() == true)
	{
		const MusicInfo* pAttr = sgTableManager()->getMusicInfoByID(type);
		if (!pAttr)
		{
			CCLOG("[ERROR]GamePrecedure::playMusicBg checkPoint:%d no bg music!", (int)type);
			return;
		}

		mLastMusicBgType = mBgMusicType;

		mTime_waitEnd = pAttr->durationTime;
		mTime_waitBegin = pAttr->interval;

		mElapsedTime = pAttr->wait;
		mMusicName = pAttr->fileName;
		mBgMusicType = type;

		//音量
		setBackgroundMusicVolume(pAttr->volume);

		bCountDown_waitBegin = true;
		bCountDown_waitEnd = false;
	}

}

void SoundManager::playLastMusicBg()
{
	playMusicBg(mLastMusicBgType);
}

void SoundManager::stopMusicBg()
{
	CocosDenshion::SimpleAudioEngine::getInstance()->stopBackgroundMusic();
}

void SoundManager::setIsPlayMusicEffect(bool isPlay)
{
	SAVE_USER_DEFAULT_BOOL("MusicEffect", isPlay);

	mLastEffectState = bPlayMusicEffect;

	if (false == isPlay)
	{
		stopAllMusicEffects();
	}
	bPlayMusicEffect = isPlay;

	SAVE_USER_DEFAULT_BOOL("isSetedMusicEffect", true);
}

bool SoundManager::getIsPlayMusicEffect()
{
	bool bSetedMusicEffect = USER_DEFAULT_BOOL("isSetedMusicEffect", false);
	if (bSetedMusicEffect)
	{
		bPlayMusicEffect = USER_DEFAULT_BOOL("MusicEffect", false);
	}
	return bPlayMusicEffect;
}

void  SoundManager::setIsPlayMusicBg(bool isPlay)
{
	SAVE_USER_DEFAULT_BOOL("MusicBg", isPlay);

	bPlayMusicBg = isPlay;

	if (isPlay == false)
	{
		bCountDown_waitBegin = false;
		bCountDown_waitEnd = false;
	}

	SAVE_USER_DEFAULT_BOOL("isSetedMusicBg", true);
}

bool  SoundManager::getIsPlayMusicBg()
{
	bool bSetedMusicBg = USER_DEFAULT_BOOL("isSetedMusicBg", false);
	if (bSetedMusicBg)
	{
		bPlayMusicBg = USER_DEFAULT_BOOL("MusicBg", false);
	}
	return bPlayMusicBg;
}

float SoundManager::getBackgroundMusicVolume()
{
	return CocosDenshion::SimpleAudioEngine::getInstance()->getBackgroundMusicVolume();
}

void SoundManager::setBackgroundMusicVolume(float volume)
{
	CocosDenshion::SimpleAudioEngine::getInstance()->setBackgroundMusicVolume(volume);
}

float SoundManager::getEffectsVolume()
{
	return CocosDenshion::SimpleAudioEngine::getInstance()->getEffectsVolume();
}

void SoundManager::setEffectsVolume(float volume)
{
	CocosDenshion::SimpleAudioEngine::getInstance()->setEffectsVolume(volume);
}

void SoundManager::update(float delta)
{
	//背景音乐计时
	if (bCountDown_waitBegin == true)	//首次播放等待时间
	{
		mElapsedTime = mElapsedTime - delta;

		if (mElapsedTime <= 0)
		{
			if (mTime_waitEnd == -1)	//-1表示循环播放，不用计算结束时间
			{
				CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic(mMusicName.c_str(), true);

				bCountDown_waitBegin = false;
				bCountDown_waitEnd = false;
			}
			else
			{
				CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic(mMusicName.c_str(), false);
				mElapsedTime = mTime_waitEnd;

				bCountDown_waitBegin = false;
				bCountDown_waitEnd = true;
			}
		}
	}
	else if (bCountDown_waitEnd == true)		//倒计时，计算播放结束
	{
		mElapsedTime = mElapsedTime - delta;

		if (mElapsedTime <= 0)
		{
			CocosDenshion::SimpleAudioEngine::getInstance()->stopBackgroundMusic(false);
			mElapsedTime = mTime_waitBegin;

			bCountDown_waitEnd = false;
			bCountDown_waitBegin = true;
		}
	}

	//延迟播放音效计时
	for (std::list<delayEffectData>::iterator iter = delayEffectList.begin(); iter != delayEffectList.end();)
	{
		if (sgTimeCalculator()->getTimeLeft(iter->mKey) == 0)
		{
			//出兵特效，做一个若干秒内只能播一次的限制
			if ((iter->mType >= SOUND_SOLDIER_WALK_1 && iter->mType <= SOUND_SOLDIER_WALK_3) || (iter->mType >= SOUND_SOLDIER_STABLES_WALK_1 && iter->mType <= SOUND_SOLDIER_STABLES_WALK_3))
			{
				if (!sgTimeCalculator()->hasKey("soldier_walk_sound"))
				{
					playMusicEffect(iter->mType);

					sgTimeCalculator()->createTimeCalcultor("soldier_walk_sound", 3.0f);
				}
				else if (sgTimeCalculator()->getTimeLeft("soldier_walk_sound") == 0)
				{
					playMusicEffect(iter->mType);

					sgTimeCalculator()->resetTimeCalcultor("soldier_walk_sound");
				}

				sgTimeCalculator()->removeTimeCalcultor(iter->mKey);
				iter = delayEffectList.erase(iter);
				continue;
			}
			else
			{
				playMusicEffect(iter->mType);
				sgTimeCalculator()->removeTimeCalcultor(iter->mKey);
				iter = delayEffectList.erase(iter);
				continue;
			}
		}

		iter++;
	}
}

