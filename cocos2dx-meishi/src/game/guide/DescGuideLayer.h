#ifndef __DESCGUIDELAYER_H__
#define __DESCGUIDELAYER_H__
#include "BaseGuideLayer.h"
enum eDescGuideType
{
	E_DESCTYPE_MORALE = 134,//士气说明
	E_DESCTYPE_LEVELUP = 62,//升级说明
	E_DESCTYPE_TURRET = 341, //炮塔说明
	E_DESCTYPE_INSTITUTE = 478, //研究所说明
};
class DescGuideLayer: public BaseGuideLayer
{
public:
	DescGuideLayer();
	~DescGuideLayer();
	CREATE_FUNC(DescGuideLayer); 
	void onEnter();
	void onExit();
	bool onTouchBegan(Touch *touch, Event *unused_event);
	void onTouchMoved(Touch *touch, Event *unused_event);
	void onTouchEnded(Touch *touch, Event *unused_event); 
	
private:
	void onGuideCallback(bool isFinish);
	void initUI();
	void onCloseDialogCallback(); 
	void loadGuideType(eDescGuideType type); 
	void showDesc(); //显示文字
	
private:
	Node* mUIDialogNode;
	Node* mUIContentNode; 
	ExRichText* mDescGuideText;
	ui::Text* mDescTips;
	bool mCanControl;
};
 
 
#endif
