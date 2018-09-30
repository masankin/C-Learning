#ifndef __BUFFVIEW_H__
#define __BUFFVIEW_H__
#include "MessageManager.h"
#include "cocos2d.h"
#include "Constant.h"
USING_NS_CC; 

 
class BuffView: public Node
{
public:
	 BuffView();
	 ~BuffView();
	 static BuffView* create(); 
	 void updateBuffView(float value);
	 void showBuffView();
	 void hideBuffView(); 
	 bool initBuffView(eBuffType buffType);  
private:
	
	void initBg(std::string fileName);
	void initProgress(std::string fileName);
private:
	Sprite* mBuffBg;
	ProgressTimer* mBuffProgress;
	
};
//////////////////////////////////

class BuffIconNode
{
public:
	BuffIconNode();
	~BuffIconNode(); 
	void setPosition(Point pos);
	Point getPoistion();
	BuffView* getBuffView(){return mBuffView;}
	void update();
	void start(eCampType camp, eBuffType buffType,float time); 
	void clearNode();
private:
	void createBuffTimer(float time);
	void updateBuffTimer(); 
	void removeBuffTimer();

	CC_SYNTHESIZE(eCampType, mCampType, CampType);
	CC_SYNTHESIZE(eBuffType, mBuffType, BuffType)
	CC_SYNTHESIZE(bool, mLock, Lock); 
	BuffView* mBuffView;
	float mTotalTime; 
};
 
#endif
