/**
* @file     MoraleManager.h
* @brief    士气系统
* @details  
* @author   Alvin
* @date     2015-8-26
*/
#ifndef __MORALEMANAGER_H__
#define __MORALEMANAGER_H__
#include "cocos2d.h"
#include "message.h"
#include "data/Tables.h"
#include "data/TableManager.h"
USING_NS_CC; 
class MoraleRefreshInfo
{
public:
	float m_RefreshTime;
	float m_MoraleValue;
	float m_MoraleLevel;
	bool m_ResetFlag;
};
class MoraleManager: public MessageHandler
{
public: 
	MoraleManager();
	~MoraleManager(); 
	void clearMorale();
	void cleanMoraleManager();
	void registMoraleMessage(eCampType camp, float value, int type, int srcID = 0);
	void unRegistMoraleMessage();
	void onReceiveMassage(GameMessage * message);
	void update(float delta);
	float getMoraleValueByCamp(eCampType camp);
private: 
	void addMorale(eCampType camp, float value, int type);
	void createMoraleValue(eCampType camp, float value, int type);
	bool addMoraleValue(eCampType camp, float value, int type);


	void addMoraleBuff(eCampType camp, float value); //处理士气增益
	void removeMoraleBuff(eCampType camp);

	void onShowMorale(eCampType camp, float value);

	void onShowMoraleEffect(eCampType camp, float value);

	void createMoraleTimer(eCampType camp, float time);
	void updateCampMorale();
	void updateMorale(eCampType camp); 
private: 
	int mSrcID;
	typedef std::map<eCampType, MoraleRefreshInfo*> MoraleValueMap; 
	MoraleValueMap m_MoraleValueMap; //存储不同阵营士气值  
};

#define sgMoraleManager() Singleton<MoraleManager>::Get()
#endif
