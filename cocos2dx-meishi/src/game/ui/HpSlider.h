#pragma once

#include "Constant.h"

#include "cocos2d.h"
USING_NS_CC;

class HpSlider : public Node
{
public:
	HpSlider(){};
	~HpSlider(){};

	static HpSlider*	create();

	bool init();

	Size minSize;

	void refreshSliderPercent();
	void reset(ui::ImageView* pSended, float percent,float& offsetX);

	void setPeopleWithCamp(int num,eCampType);

	void setHeadCenter();

	void initSlider();

	std::string getTextureName(eCampType type);
	std::string getIconName(eCampType type);

protected:
	typedef std::map<eCampType, ui::ImageView*> SliderType;
	typedef std::map<eCampType, ui::Text*> LableType;

	virtual void onMenuItemAction(std::string, cocos2d::Ref*);

	void updateBar(ui::ImageView* bar, float posX, float width);
	
	Node* mRoot;
	Node* mHoles;
	ui::ImageView* mPlayer1;
	ui::ImageView* mPlayer2;
	Node* mNumNode1;
	Node* mNumNode2;
	LabelAtlas* mNumLabel1;
	LabelAtlas* mNumLabel2;
	LabelAtlas* mNumLabel3;
	LabelAtlas* mNumLabel4;

	SliderType mCampSlider;
	LableType mLabelList;

	int offSetX;
};

//DEFINE_PAGE_READER(ArrowWidget);
