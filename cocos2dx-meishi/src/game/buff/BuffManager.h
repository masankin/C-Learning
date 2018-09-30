/*68888886866668688888886866686
68666666866668686666666866866
68666666866668686666666868666
68888666866668686666666886666
68666666866668686666666868666
68666666866668686666666866866
68666666888888688888886866686
66666666666666666666666666666
68666668688888686668666666666
66866686686668686668666666666
66686866686668686668666666666
66668666686668686668666666666
66668666686668686668666666666
66668666688888688888666666666
/**
* @file     BuffManager.h
* @brief    buff管理器
* @details  
* @author   Alvin
* @date     2015-8-26
*/
#ifndef __BUFFMANAGER_H__
#define __BUFFMANAGER_H__
#include "data/Tables.h"
#include "data/TableManager.h"
#include "Message.h" 
enum eTargetType
{
	E_TARGET_CAMP = 0, //全阵营作用
	E_TARGET_BUILD = 1 //特定建筑作用
};

class BuffNode
{
public:
	BuffNode();
	void resetValue();
	float attackSpeed;
	float moveSpeed;
	float attack;
	float defence;
	float productSpeed;
	bool  peopleFull;

	float attackSpeedRate;
	float moveSpeedRate;
	float attackRate;
	float defenceRate;
	float productSpeedRate;
};
 
typedef std::map<int, int> BuffIDMap;
class BuffManager: public MessageHandler
{
public:
	BuffManager();  
	void clearBuff();
	void update(float deleta);
	void onReceiveMassage(GameMessage * message);  

	
	const BuffIDMap* getBuffListByCamp(eCampType camp);

	float getBuffAttackSpeed(eCampType camp, float baseValue);
	float getBuffMoveSpeed(eCampType camp, float baseValue);
	float getBuffAttack(eCampType camp, float baseValue);
	float getBuffDefence(eCampType camp, float baseValue);
	float getBuffProduceSpeed(eCampType camp, float baseValue);
	void  setCampPeopleToFull(eCampType camp, int buffID); 
	bool  getExistBuff(eCampType camp, int buffID); 
private:
	//新增buff
	void addBuff(eCampType camp, int buffID);

	//记录当前buff
	void recordBuff(eCampType camp, int buffID);

	//移除Buff
	void removeBuff(eCampType camp, int buffID); 

	//更新阵营buff数值
	void addCampBuff(eCampType camp, int buffID, int addType);  

	void initCampBuffValue(BuffNode* buffNode);
	void updateCampBuffValue(eCampType camp, BuffNode* buffNode, int buffID,  int addType);
	void resetCampBuff(eCampType camp);
	
	float getAddBuffValue(float srcValue, float addValue, int addType);
	float getResetBuffValue(float setValue);

	//创建buff计时器，如果存在该计时器则重置时间
	void createBuffTimer(eCampType camp, int buffID, float time);
	//创建buff计时器
	void createBuffTimer(eCampType camp, int keyID, int buffID, float time);
	//移除buff计时器
	void removeBuffTimer(eCampType camp, int keyID, int buffID);

	//查询Buff是否有效 
	bool checkBuffActive(eCampType camp, int keyID, int buffID);

	//显示Buff效果
	void showBuffEffect(eCampType camp, int buffID, int buffSrc);

	void resetBuff(eCampType camp, int keyID, int buffID);

	 
	//查询Buff是否存在
	bool checkBuffIDExist(eCampType type, int buffID);

	//移除当前所有BUFF
	void removeCurBuff(eCampType type);
	void removeCurBuffView(eCampType type);
	 
	typedef std::map<eCampType, BuffIDMap*> CampBuffListMap; 
	CampBuffListMap m_CampBuffListMap;//用于标明相关阵营拥有的buffID列表

	

	typedef std::map<eCampType, BuffNode*> CampBuffValueMap; 
	CampBuffValueMap m_CampBuffValueMap;//用于标明相关阵营拥有的buff效果

	static int mBuffKey;
	int createBuffKey();
	 
};

 #define sgBuffManager() Singleton<BuffManager>::Get()
 
#endif
