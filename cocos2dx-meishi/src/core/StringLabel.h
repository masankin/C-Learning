//
//  StringLabel.h
//
//  Created by Airel on 15-9-8.
//
//

#ifndef __BombMouse__CPowerStringLabel__
#define __BombMouse__CPowerStringLabel__
#include "cocos2d.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
using namespace ui;
using namespace cocostudio;
USING_NS_CC;


enum PS_TYPE{
	PS_NORMAL_TEXT,
	PS_COLOR,
	PS_IMAGE,
	PS_ANI,
	PS_EFFECT_ANI,
	PS_NLINE
};

class PowerstringBaseType : public Ref{
protected:
	CC_SYNTHESIZE_READONLY(PS_TYPE, mType, iType);
public:
	PowerstringBaseType(PS_TYPE type):mType(type){
	}
};

class PowerString_NormalText : public PowerstringBaseType{
protected:
	CC_SYNTHESIZE_READONLY(std::string, mText,sText);
public:
	PowerString_NormalText(const char* text):PowerstringBaseType(PS_NORMAL_TEXT){
		mText = text;
	}
};

class PowerString_NextLine : public PowerstringBaseType{
protected:
public:
	PowerString_NextLine():PowerstringBaseType(PS_NLINE){
	}
};

class PowerString_Color : public PowerstringBaseType{
protected:
	CC_SYNTHESIZE_READONLY(unsigned int, miColor, Color);
public:
	PowerString_Color(unsigned int color):PowerstringBaseType(PS_COLOR){
		miColor = color;
	}
};


class PowerString_Image : public PowerstringBaseType{
protected:
	CC_SYNTHESIZE_READONLY(std::string, mImageName, ImageName);
public:
	PowerString_Image(const char* imageName):PowerstringBaseType(PS_IMAGE),mImageName(imageName){

	}
};

class PowerString_Ani : public PowerstringBaseType{
protected:
	CC_SYNTHESIZE_READONLY(std::string, mImageName, ImageName);
public:
	PowerString_Ani(const char* imageName):PowerstringBaseType(PS_ANI),mImageName(imageName){

	}
};

class PowerString : public Ref{

public:
	CC_SYNTHESIZE_READONLY(__Array*, mTypes, Types);
	CC_SYNTHESIZE(std::string, mText, sText);
public:

	virtual ~PowerString(){
		CC_SAFE_RELEASE(mTypes);
	}

	bool init(const char*);

};



class StringLabel : public NodeRGBA{
 
public:
    ~StringLabel();
    
    int getRealyWidth();
    int getRealyHight();

    void setTextColor(ccColor3B);
    void setTextSize(int size);

    virtual bool init();
    static StringLabel* create();
    
    void setPowerString(PowerString* powerStirng, bool isAutoLabelLine = false);
    
	CC_SYNTHESIZE(bool, m_bSingleLine, SingleLine);
	CC_SYNTHESIZE(int, mLineNum, LineNum);
	CC_SYNTHESIZE(std::string, mFontName, FontName);

private:
	Node* mCurrentLine;
	__Array* mLabelLines;
	int mCurrentLineX;
	int mLine;
	cocos2d::Color3B mCurrentColor;
	int mFontSize;
	int mMaxCurrentLineX;
protected:

	CC_SYNTHESIZE_READONLY(PowerString*, mPowerString, PowerString)

	void preCalculateSize();

	void addNormalText(PowerString_NormalText* type);
	void addAutoNormalText(PowerString_NormalText* type);
	void addImage(PowerString_Image* type);
	int addColor(PowerString_Color* type);
	void addAni(PowerString_Ani* type);

	void cleanAllLine();
	bool nextLine();
	void resetSizeAndShow();
};

#endif /* defined(__StringLabel__) */
