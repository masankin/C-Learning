#ifndef ExRichText_h__
#define ExRichText_h__

#include "cocos-ext.h"
#include "cocos2d.h"
#include "platform/CCSAXParser.h"
#include "ui/UIText.h"
USING_NS_CC;
USING_NS_CC_EXT;
using namespace ui;

typedef enum {
	EX_RICH_NEWLINE,
	EX_RICH_TEXT,
    EX_RICH_NODE,
	EX_RICH_WIDGET,
} ExRichElementType;
    
class ExRichElement : public CCObject
{
public:
    ExRichElement(){};
    virtual ~ExRichElement(){};
    virtual bool init(int tag, const ccColor3B& color, GLubyte opacity);
protected:
    ExRichElementType _type;
    int _tag;
    ccColor3B _color;
    GLubyte _opacity;
    friend class ExRichText;
};

class ExRichElementNewLine : public ExRichElement
{
public:
	ExRichElementNewLine(){_type = EX_RICH_NEWLINE;};
	virtual ~ExRichElementNewLine(){};
	virtual bool init(int tag);
	static ExRichElementNewLine* create(int tag);
protected:
	friend class ExRichText;
};

class ExRichElementText : public ExRichElement
{
public:
	ExRichElementText(){_type = EX_RICH_TEXT;};
	virtual ~ExRichElementText(){CC_SAFE_RELEASE(_pText);};
	virtual bool init(int tag, const ccColor3B& color, GLubyte opacity, Text *pText);
	static ExRichElementText* create(int tag, const ccColor3B& color, GLubyte opacity, Text *pText);
	void setText(const std::string &text);
protected:
	std::string _text;
	Text *_pText;
	friend class ExRichText;
};

class ExRichElementNode : public ExRichElement
{
public:
    ExRichElementNode(){_type = EX_RICH_NODE;};
    virtual ~ExRichElementNode(){CC_SAFE_RELEASE(_node);};
    virtual bool init(int tag, const ccColor3B& color, GLubyte opacity, CCNode* node);
    static ExRichElementNode* create(int tag, const ccColor3B& color, GLubyte opacity, CCNode* node);
protected:
    CCNode* _node;
    friend class ExRichText;
};

class ExRichElementWidget : public ExRichElement
{
public:
	ExRichElementWidget(){_type = EX_RICH_WIDGET;};
	virtual ~ExRichElementWidget(){CC_SAFE_RELEASE(_widget);};
	virtual bool init(int tag, const ccColor3B& color, GLubyte opacity, Widget* widget);
	static ExRichElementWidget* create(int tag, const ccColor3B& color, GLubyte opacity, Widget* widget);
protected:
	Widget* _widget;
	friend class ExRichText;
};

typedef std::map<std::string, std::string> attrs_t;

class ExRichText : public Widget, SAXDelegator
{
public:
    ExRichText();
    virtual ~ExRichText();
    static ExRichText* create();
	virtual bool init();
	//virtual void visit();
    virtual void setAnchorPoint(const CCPoint &pt);
    virtual void ignoreContentAdaptWithSize(bool ignore);
	virtual const CCSize& getSize() const;
	virtual void setSize(const CCSize &size);
	void setVerticalSpace(float space); // 设置行间距
	void setLimitWidth(float width); // 设置最大行宽
	void setString(const std::string &text);
	void setText(const std::string &text);
	void appendText(const std::string &text);
	void addElmtTouchEventListener(CCObject *pTarget, ui::SEL_TouchEvent selector);
	int getLineNum();		//获取行数
protected:
    virtual void initRenderer();
	virtual void startElement(void *ctx, const char *name, const char **atts);
	virtual void endElement(void *ctx, const char *name);
	virtual void textHandler(void *ctx, const char *s, int len);
	void insertElement(ExRichElement* element, int index);
	void pushBackElement(ExRichElement* element);
	void formatText();
    void pushToContainer(CCNode* renderer);
    void handleNodeRenderer(CCNode* renderer);
	void handleWidgetRenderer(Widget* renderer);
    void formarRenderers();
    void addNewLine();
private:
	void touchEventHandler(CCObject *pSender, ui::TouchEventType type);
	inline bool hasAttribute(const attrs_t &attrs, const char* attr)
	{
		attrs_t::const_iterator it = attrs.find(attr);

		return it != attrs.end() && !it->second.empty();
	}
	inline int chr_transfer_hex_value(char c)
	{
		int iRet = -1;

		if ( c >= '0' && c <= '9' ) {
			iRet = c - '0';
		} else if ( c >= 'A' && c <= 'F' ) {
			iRet = c - 'A' + 10;
		} else if ( c >= 'a' && c <= 'f' ) {
			iRet = c - 'a' + 10;
		}

		return iRet;
	}
	inline unsigned long str_to_hex(const std::string &hex_str) {
		int factor = 1;
		unsigned long num = 0;
		unsigned int n = 0;

		if(hex_str.substr(0, 1) == "#") {
			n = 1;
		} else if(hex_str.substr(0, 2) == "0x") {
			n = 2;
		}

		for(unsigned int i = n; i < hex_str.size(); i++) {
			unsigned int rIdx = hex_str.size() - 1 - (i - n);
			num += factor * chr_transfer_hex_value(hex_str[rIdx]);
			factor *= 16;
		}

		return num;
	}
	
protected:
    bool _formatTextDirty;
    CCArray* _richElements;
    std::vector<CCArray*> _elementRenderers;
	float _limitWidth; // 限制行宽
    float _leftSpaceWidth; // 当前距离行尾的宽度（行宽参照_limitWidth）
    float _verticalSpace; // 行间距
	int _lineNum;	//行数
    Widget* _elementRenderersContainer;
	std::string _text;
	CCObject *_pListener;
	ui::SEL_TouchEvent _pSelector;
private:
	ExRichElement *_pCurElmt;
	
};

#endif // ExRichText_h__
