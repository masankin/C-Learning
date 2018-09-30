//
//  StringLabel.cpp
//
//  Created by Airel on 15-9-8.
//
//

#include "StringLabel.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "Gif/GifAnimation.h"

#define CEHCK_STRING_EOF(I, L) if(I >= L) break;

#define DEFAULT_FONT_SIZE (20)

#define TEXT_CELL_HIGHT 7     //每行高度增量


int parseColorString(std::string str) {

	int color = 0;

	if(str.length() != 6){
		return 0x00ffffff;
	}

	for(int i = 0 ; i < str.length() ; i++){

		char c = str[i];
		int v = 0;
		if(c == '0') v = 0;
		if(c == '1') v = 1;
		if(c == '2') v = 2;
		if(c == '3') v = 3;
		if(c == '4') v = 4;
		if(c == '5') v = 5;
		if(c == '6') v = 6;
		if(c == '7') v = 7;
		if(c == '8') v = 8;
		if(c == '9') v = 9;
		if(c == 'A' || c == 'a') v = 10;
		if(c == 'B' || c == 'b') v = 11;
		if(c == 'C' || c == 'c') v = 12;
		if(c == 'D' || c == 'd') v = 13;
		if(c == 'E' || c == 'e') v = 14;
		if(c == 'F' || c == 'f') v = 15;

		color =  color | (v << ((str.length() - (i + 1)) * 4));

	}

	return color;
}

bool PowerString::init(const char* t_text){
	this->mText = t_text;
	mTypes = __Array::create();
	mTypes->retain();

	int slen = strlen(t_text);

	for(int i = 0 ; i < slen ; i++){

		char c = t_text[i];

		if(c == '\\')
		{
			if(t_text[i + 1] == 'n')
			{
				i++;
				PowerString_NextLine* type = new PowerString_NextLine();
				type->autorelease();
				mTypes->addObject(type);
				continue;
			}
			else if(t_text[i + 1] == 'c')
			{
				i+=2;
				CEHCK_STRING_EOF(i, slen);
				if(t_text[i] == '[')
				{
					i++;
					CEHCK_STRING_EOF(i, slen);
					std::string color = "";
					while(t_text[i] != ']')
					{
						color += t_text[i];
						i++;
						CEHCK_STRING_EOF(i, slen);
					}
					int intcolor = parseColorString(color);
					PowerString_Color* type = new PowerString_Color(intcolor);
					type->autorelease();
					mTypes->addObject(type);
					continue;
				}
			}
			else if(t_text[i + 1] == 'a')
			{
				i+=2;
				CEHCK_STRING_EOF(i, slen);
				if(t_text[i] == '[')
				{
					i++;
					CEHCK_STRING_EOF(i, slen);
					std::string pict = "";
					while(t_text[i] != ']')
					{
						pict += t_text[i];
						i++;
						CEHCK_STRING_EOF(i, slen);
					}
					PowerString_Ani* type = new PowerString_Ani(pict.c_str());
					type->autorelease();
					mTypes->addObject(type);
					continue;
				}
			}
			else if(t_text[i + 1] == 'p')
			{
				i+=2;
				CEHCK_STRING_EOF(i, slen);
				if(t_text[i] == '[')
				{
					i++;
					CEHCK_STRING_EOF(i, slen);
					std::string pict = "";
					while(t_text[i] != ']')
					{
						pict += t_text[i];
						i++;
						CEHCK_STRING_EOF(i, slen);
					}
					PowerString_Image* type = new PowerString_Image(pict.c_str());
					type->autorelease();
					mTypes->addObject(type);
					continue;
				}
			}
		}else
		{
			std::string str;
			while(t_text[i] != '\\')
			{
				if(t_text[i] == '\n')
				{

					if(str.length() > 0)
					{
						PowerString_NormalText* type = new PowerString_NormalText(str.c_str());
						type->autorelease();
						mTypes->addObject(type);
						str = "";
					}

					PowerString_NextLine* type = new PowerString_NextLine();
					type->autorelease();
					mTypes->addObject(type);
					i++;
					break;
				}
				str += t_text[i];
				i ++;
				CEHCK_STRING_EOF(i, slen);
			}
			i--;
			PowerString_NormalText* type = new PowerString_NormalText(str.c_str());
			type->autorelease();
			mTypes->addObject(type);
			continue;

		}

	};
	return true;
}


unsigned char utf8_look_for_table[] =
{
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 1, 1
};

#define UTFLEN(x)  utf8_look_for_table[(x)]

int getUtf8Length(const char *str)
{
    int clen = strlen(str);
    int len = 0;
    char* _tmp = (char*)str;
    for(char *ptr = _tmp;
        *ptr!=0&&len<clen;
        len++, ptr+=UTFLEN((unsigned char)*ptr));
    
    return len;
}

//get子串
char* subUtfString(char *str, unsigned int start, unsigned int end)
{
    int len = getUtf8Length(str);
    
    if(start >= len) return NULL;
    if(end > len) end = len;
    
    char *sptr = str;
    for(int i = 0; i < start; ++i,sptr+=UTFLEN((unsigned char)*sptr));
    
    char *eptr = sptr;
    for(int i = start; i < end; ++i,eptr += UTFLEN((unsigned char)*eptr));
    
    int retLen = eptr - sptr;
    char *retStr = (char*)malloc(retLen+1);
    memcpy(retStr, sptr, retLen);
    retStr[retLen] = 0;
    
    return retStr;
}

bool StringLabel::init(){
    
    mCurrentLine = NULL;
    
    mLabelLines = __Array::create();
    mLabelLines->retain();
    
    mCurrentLineX = 0;
    mMaxCurrentLineX = 0;
    
    mCurrentColor = cocos2d::Color3B(135,72,51);
    
    mFontSize = DEFAULT_FONT_SIZE;
    
	mLineNum = 0;

    return true;
}

StringLabel* StringLabel::create() {
    StringLabel* powerLabel = new StringLabel();
    if(powerLabel->init()){
        powerLabel->autorelease();
        return powerLabel;
    }
    return powerLabel;
}

StringLabel::~StringLabel(){
    CC_SAFE_RELEASE(mLabelLines);
}

void StringLabel::addNormalText(PowerString_NormalText* type){

	if(mCurrentLine == NULL){
		nextLine();
	}

	std::string text = type->getsText();

	int textLength = getUtf8Length(text.c_str());
	mMaxCurrentLineX = getContentSize().width;

	for (int i = 0; i < textLength ; i++) {

		char* tmpText = subUtfString((char*)text.c_str(), i, i + 1);
		 ui::Text* label = ui::Text::create();
		label->setFontName(mFontName);
		label->setColor(mCurrentColor);
		if(tmpText && strlen(tmpText) > 0){
			label->setString(tmpText);
			free(tmpText);
		}
		label->setFontSize(mFontSize);
		label->setAnchorPoint(ccp(0, 0));

		// new line
		if(mCurrentLineX + label->getContentSize().width > getContentSize().width && !m_bSingleLine){

			if(!nextLine()) break;
		}

		label->setPosition(ccp(mCurrentLineX, 0));

		mCurrentLine->addChild(label);

		mCurrentLineX += label->getContentSize().width;
		if(mCurrentLineX > mMaxCurrentLineX){
			mMaxCurrentLineX = mCurrentLineX;
		}

		int nWidth = mCurrentLineX;
		int nHeight = mCurrentLine->getContentSize().height;
		if(label->getContentSize().height > nHeight){
			nHeight = label->getContentSize().height;
		}
		// reset size
		mCurrentLine->setContentSize(CCSize(nWidth, nHeight));



		if (mCurrentLineX >= getContentSize().width && !m_bSingleLine) {
			if(!nextLine()) break;
		}

	}
}

void StringLabel::addAutoNormalText(PowerString_NormalText* type)
{
	if(mCurrentLine == NULL){
		nextLine();
	}

	std::string text = type->getsText();
	int textLength = getUtf8Length(text.c_str());

	std::vector<int> ends;
	ends.push_back(0);
	int wordLineWidth = 0;
	bool isOneLine = true; //只有单行
	int currentLineX = mCurrentLineX;
	int contentWidth = getContentSize().width;
	mMaxCurrentLineX = contentWidth;
	for (int i=0; i<textLength; i++) {
		char* tmpText = subUtfString((char*)text.c_str(), i, i + 1);
		int tmpSize = strlen(tmpText);
		free(tmpText);


		int wordWidth = mFontSize;
		if(tmpSize == 1) {
			wordWidth = mFontSize/2 + 3;
		}

		wordLineWidth = wordLineWidth + wordWidth;

		if (currentLineX + wordLineWidth >= contentWidth && !m_bSingleLine ) {
			isOneLine = false;
			currentLineX = 0;
			wordLineWidth = 0;
			ends.push_back(i);
		}
	}
	ends.push_back(textLength);

	if (textLength > 0) {
		for (int i=0; i<ends.size() -1; i++) {
			char* tmpText = subUtfString((char*)text.c_str(), ends[i],  ends[i+1]);
			ui::Text* label = ui::Text::create();
			label->setColor(mCurrentColor);
			label->setString(tmpText);
			label->setFontName(mFontName);
			free(tmpText);

			label->setFontSize(mFontSize);
			label->setAnchorPoint(ccp(0, 0));
			label->setPosition(ccp(mCurrentLineX, 0));
			mCurrentLine->addChild(label);

			int nWidth = mCurrentLineX + label->getContentSize().width;//m_iCurrentLineX + wordLineWidth;
			int nHeight = mCurrentLine->getContentSize().height;
			if(label->getContentSize().height > nHeight) {
				nHeight = label->getContentSize().height;
			}
			mCurrentLine->setContentSize(CCSize(nWidth, nHeight));
			mCurrentLineX = mCurrentLineX +  label->getContentSize().width;
			if(mCurrentLineX > mMaxCurrentLineX){
				mMaxCurrentLineX = mCurrentLineX;
			}

			if (isOneLine == false) {
				nextLine();
			}
		}
	}
}


int  StringLabel::getRealyWidth(){
    return mMaxCurrentLineX;
}

int  StringLabel::getRealyHight(){
	return getContentSize().height;
}


bool StringLabel::nextLine(){

    mLine++;
	if(mLineNum && mLine > mLineNum) return false;
    mCurrentLine = Node::create();
    mCurrentLine->setAnchorPoint(Vec2(0,0));
    mLabelLines->addObject(mCurrentLine);
    mCurrentLineX = 0;
    return true;
}


void StringLabel::resetSizeAndShow(){
    
    int height = 0;

    Ref* pObject = NULL;
    CCARRAY_FOREACH(mLabelLines, pObject){
		Node* pLineNode = dynamic_cast<Node*>(pObject);		
        height += pLineNode->getContentSize().height + TEXT_CELL_HIGHT;
    }
    setContentSize(CCSize(mMaxCurrentLineX, height));
    height = 0;
    CCARRAY_FOREACH_REVERSE(mLabelLines, pObject){
        Node* pLineNode = dynamic_cast<Node*>(pObject);
        pLineNode->setPositionY(height);
        height += pLineNode->getContentSize().height + TEXT_CELL_HIGHT;	
		this->addChild(pLineNode);
    }
        
}

void StringLabel::addImage(PowerString_Image* type){
    if(mCurrentLine == NULL){
        nextLine();
    }
    
    std::string imageName = type->getImageName();
    
    Sprite* sprite = Sprite::create(imageName.c_str());
    
    if(!sprite) return;
    Size img_size = sprite->getContentSize();
    
    // new line
    if(mCurrentLineX + img_size.width > getContentSize().width && !m_bSingleLine){
        nextLine();
    }
    
    sprite->setAnchorPoint(Vec2(0,0.5));
    sprite->setPosition(Vec2(mCurrentLineX, mCurrentLine->getContentSize().height/2));
    mCurrentLineX += img_size.width;
    mCurrentLine->addChild(sprite);
    if(mCurrentLineX > mMaxCurrentLineX){
        mMaxCurrentLineX = mCurrentLineX;
    }
    
    int nWidth = mCurrentLineX;
    int nHeight = mCurrentLine->getContentSize().height;

    mCurrentLine->setContentSize(Size(nWidth, nHeight));
}

void StringLabel::addAni(PowerString_Ani* type){

	if(mCurrentLine == NULL){
		nextLine();
	}

	std::string imageName = type->getImageName();
	imageName = FileUtils::getInstance() -> fullPathForFilename(imageName.c_str());

	// 动画定义
	GifAnimationDef def;
	def.loops = -1;						// 循环次数
	def.filePath = imageName;				// 文件路径
	def.delayPerUnit = 0.08f;			// 每帧间隔
	def.restoreOriginalFrame = true;	// 还原初始状态

	// 创建动画
	auto pAnimation = GifAnimation::Get()->createAnimation(def);

	// 创建精灵播放动画
	auto ani = Sprite::create();
	ani->runAction(Animate::create(pAnimation));

	if(!ani) return;
	ani->setScale(0.6f);
	Size ani_size = Size(mFontSize,mFontSize);//ani->getContentSize();

	// new line
	if(mCurrentLineX + ani_size.width > getContentSize().width && !m_bSingleLine){
		nextLine();
	}
	ani->setAnchorPoint(ccp(0,0.5));
	ani->setPosition(ccp(mCurrentLineX + (ani_size.width / 2), ani_size.height/2));
	mCurrentLineX += ani_size.width;
	mCurrentLine->addChild(ani);

	mMaxCurrentLineX = getContentSize().width;

	int nWidth = mCurrentLineX;
	int nHeight = mCurrentLine->getContentSize().height;
	if(ani_size.height > nHeight){
		nHeight = (int)ani_size.height;
	}

	// reset size
	mCurrentLine->setContentSize(Size(nWidth, nHeight));

}


int StringLabel::addColor(PowerString_Color* type){
    return type->getColor();
}


void StringLabel::cleanAllLine(){
    mCurrentLineX = 0; 
	mCurrentLine = NULL;
    Ref* pObject = NULL;
    CCARRAY_FOREACH(mLabelLines, pObject){
        Node* pLineNode = dynamic_cast<Node*>(pObject);
        this->removeChild(pLineNode);
    }
    mLabelLines->removeAllObjects();
    mLine = 0;
}

void StringLabel::setTextColor(cocos2d::Color3B c){
    mCurrentColor = c;
}

void StringLabel::setTextSize(int size){
    mFontSize = size;
}

void StringLabel::setPowerString(PowerString* powerStirng, bool isAutoLabelLine){

    this->cleanAllLine();
    mMaxCurrentLineX = 0;
    mCurrentLineX = 0;
        
    Ref* pObject;
    CCARRAY_FOREACH(powerStirng->getTypes(), pObject){
        PowerstringBaseType* powerType = dynamic_cast<PowerstringBaseType*>(pObject);
        if(powerType){
            switch (powerType->getiType()) {
                case PS_NORMAL_TEXT:
                {
                    if (isAutoLabelLine) {
                        addAutoNormalText((PowerString_NormalText*)powerType);
                    } else {
                        addNormalText((PowerString_NormalText*)powerType);
                    }
                    break;
                }
                case PS_COLOR:{
                    int resu_color =  addColor((PowerString_Color*)powerType);
                    mCurrentColor = cocos2d::Color3B(resu_color >> 16 & 0xff, resu_color >> 8 & 0xff, resu_color & 0xff);
                    break;
                }
                case PS_IMAGE:
                {
                    addImage((PowerString_Image*)powerType);
                    break;
                }
                case PS_NLINE:
                {
                    nextLine();
                    break;
                }
				case PS_ANI:
				{
					addAni((PowerString_Ani*)powerType);
					break;
				}
                default:
                    break;
            }
        }
    }
    this->resetSizeAndShow();
}
