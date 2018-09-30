#include "ExRichText.h"
//
//#include "game/chat/AnimSprite.h"
//#include "defines.h"
//#include "ToolUtils.h"
#include "game/utils/Comm.h"

bool ExRichElement::init(int tag, const ccColor3B &color, GLubyte opacity)
{
    _tag = tag;
    _color = color;
    _opacity = opacity;
    return true;
}

ExRichElementNewLine* ExRichElementNewLine::create(int tag)
{
	ExRichElementNewLine* element = new ExRichElementNewLine();
	if (element && element->init(tag))
	{
		element->autorelease();
		return element;
	}
	CC_SAFE_DELETE(element);
	return NULL;
}

bool ExRichElementNewLine::init(int tag)
{
	if (ExRichElement::init(tag, ccc3(0xff, 0xff, 0xff), 0xff))
	{
		
		return true;
	}
	return false;
}

ExRichElementText* ExRichElementText::create(int tag, const ccColor3B& color, GLubyte opacity, Text *pText) {
	ExRichElementText *pRet = new ExRichElementText();

	if(pRet != NULL && pRet->init(tag, color, opacity, pText)) {
		pRet->autorelease();
	} else {
		CC_SAFE_DELETE(pRet);
	}

	return pRet;
}

bool ExRichElementText::init(int tag, const ccColor3B& color, GLubyte opacity, Text *pText) {
	if (ExRichElement::init(tag, color, opacity))
	{
		_pText = pText;	
		_pText->retain();
		return true;
	}
	return false;
}

void ExRichElementText::setText(const std::string &text) {
	_text = text;
}

ExRichElementNode* ExRichElementNode::create(int tag, const ccColor3B &color, GLubyte opacity, cocos2d::CCNode *node)
{
    ExRichElementNode* element = new ExRichElementNode();
    if (element && element->init(tag, color, opacity, node))
    {
        element->autorelease();
        return element;
    }
    CC_SAFE_DELETE(element);
    return NULL;
}
    
bool ExRichElementNode::init(int tag, const ccColor3B &color, GLubyte opacity, cocos2d::CCNode *node)
{
    if (ExRichElement::init(tag, color, opacity))
    {
        _node = node;
        _node->retain();
        return true;
    }
    return false;
}

ExRichElementWidget* ExRichElementWidget::create(int tag, const ccColor3B &color, GLubyte opacity, Widget *widget)
{
	ExRichElementWidget* element = new ExRichElementWidget();
	if (element && element->init(tag, color, opacity, widget))
	{
		element->autorelease();
		return element;
	}
	CC_SAFE_DELETE(element);
	return NULL;
}

bool ExRichElementWidget::init(int tag, const ccColor3B &color, GLubyte opacity, Widget *widget)
{
	if (ExRichElement::init(tag, color, opacity))
	{
		_widget = widget;
		_widget->retain();
		return true;
	}
	return false;
}
    
ExRichText::ExRichText():
_formatTextDirty(true),
_richElements(NULL),
_limitWidth(0.0f),
_leftSpaceWidth(0.0f),
_verticalSpace(0.0f),
_elementRenderersContainer(NULL),
_text(""),
_pCurElmt(NULL),
_pListener(NULL),
_pSelector(NULL),
_lineNum(0)
{
    
}
    
ExRichText::~ExRichText()
{
    _richElements->release();
}
    
ExRichText* ExRichText::create()
{
    ExRichText *pRet = new ExRichText();

    if (pRet && pRet->init())
    {
        pRet->autorelease();
    } else {
		CC_SAFE_DELETE(pRet);
	}
    
    return pRet;
}
    
bool ExRichText::init()
{
    if (Widget::init())
    {
        _richElements = CCArray::create();
        _richElements->retain();
        return true;
    }
    return false;
}

//void ExRichText::visit()
//{
//	if(_enabled) {
//		formatText();
//		Widget::visit();
//	}
//}

void ExRichText::initRenderer()
{
    _elementRenderersContainer = Widget::create();
    _elementRenderersContainer->setAnchorPoint(ccp(0.0f, 0.0f));
	_elementRenderersContainer->ignoreContentAdaptWithSize(false);
    addChild(_elementRenderersContainer);

}

void ExRichText::insertElement(ExRichElement *element, int index)
{
    _richElements->insertObject(element, index);
}
    
void ExRichText::pushBackElement(ExRichElement *element)
{
    _richElements->addObject(element);
}
  
void ExRichText::formatText()
{
    if (_formatTextDirty)
    {
		for (unsigned int i=0; i<_richElements->count(); i++) {
			ExRichElement *pRichElmt = static_cast<ExRichElement*>(_richElements->objectAtIndex(i));

			switch(pRichElmt->_type) {
				case EX_RICH_NEWLINE:
					{
						addNewLine();
						break;
					}
				case EX_RICH_TEXT:
					{
						ExRichElementText *pElmtText = dynamic_cast<ExRichElementText*>(pRichElmt);
						std::string s = pElmtText->_text;
						Text *pText = pElmtText->_pText;
						pText->setString(s);
						
						std::vector<std::string> texts;
						parseUTFStr(s, texts);
						std::vector<std::string> stVals;
						float calcWidth = _leftSpaceWidth;

						if (calcWidth - pText->getContentSize().width < 0) {
							std::string tmp = "";

							for(unsigned int i = 0; i < texts.size(); i++) {
								pText->setString(tmp + texts[i]);

								if (calcWidth - pText->getContentSize().width < 0) { // 超出宽度另起一行
									stVals.push_back(tmp);
									tmp = texts[i];
									calcWidth = _limitWidth;
								} else {
									tmp += texts[i];
								}

							}

							if(tmp != "") {
								stVals.push_back(tmp);
							}
						} else {
							stVals.push_back(s);
						}

						for(unsigned int i = 0; i < stVals.size(); i++) { // stVals的size应该>=1
							if(i == 0) {
								pText->setString(stVals[i]);
								_leftSpaceWidth -= pText->getContentSize().width;
								pushToContainer(pText);
							} else {
								addNewLine();
								
								//Label *pNewLabel = dynamic_cast<Label*>(pText->clone());
								Text *pNewLabel = Text::create();
								pNewLabel->setColor(pText->getColor());
								pNewLabel->setFontName(pText->getFontName());
								pNewLabel->setFontSize(pText->getFontSize());
								pNewLabel->setString(stVals[i]);
								_leftSpaceWidth -= pNewLabel->getContentSize().width;
								pushToContainer(pNewLabel);
							}
						}
						break;
					}
				case EX_RICH_NODE:
					{
						ExRichElementNode *pElmtNode = dynamic_cast<ExRichElementNode*>(pRichElmt);
						CCNode *pRenderer = pElmtNode->_node;
						CCSize nodeSize = pRenderer->getContentSize();
						_leftSpaceWidth -= nodeSize.width;

						if(_leftSpaceWidth < 0.0f) {
							addNewLine();
							_leftSpaceWidth -= nodeSize.width;
							pushToContainer(pRenderer);
						} else {
							pushToContainer(pRenderer);
						}
						break;
					}
				case EX_RICH_WIDGET:
					{
						ExRichElementWidget *pElmtWidget = dynamic_cast<ExRichElementWidget*>(pRichElmt);
						Widget *pRenderer = pElmtWidget->_widget;
						CCSize widgetSize = pRenderer->getSize();
						_leftSpaceWidth -= widgetSize.width;

						if(_leftSpaceWidth < 0.0f) {
							addNewLine();
							_leftSpaceWidth -= widgetSize.width;
							pushToContainer(pRenderer);
						} else {
							pushToContainer(pRenderer);
						}
						break;
					}
				default:
					break;
			}
		}

        formarRenderers();
        _formatTextDirty = false;
    }
}

void ExRichText::handleNodeRenderer(CCNode *renderer)
{
    CCSize nodeSize = renderer->getContentSize();
    _leftSpaceWidth -= nodeSize.width;

    if (_leftSpaceWidth < 0.0f)
    {
        addNewLine();
        _leftSpaceWidth -= nodeSize.width;
        pushToContainer(renderer);
    } else {
        pushToContainer(renderer);
    }
}

void ExRichText::handleWidgetRenderer(Widget *renderer)
{
	CCSize widgetSize = renderer->getSize();

	_leftSpaceWidth -= widgetSize.width;
	if (_leftSpaceWidth < 0.0f)
	{
		addNewLine();
		_leftSpaceWidth -= widgetSize.width;
		pushToContainer(renderer);
	} else {
		pushToContainer(renderer);
	}
}
    
void ExRichText::addNewLine()
{
    _leftSpaceWidth = _limitWidth;
    _elementRenderers.push_back(CCArray::create());
	_lineNum++;
}
    
void ExRichText::formarRenderers()
{
    if (_ignoreSize)
    {
        float newContentSizeWidth = 0.0f;
        float newContentSizeHeight = 0.0f;
        
        CCArray* row = (CCArray*)(_elementRenderers[0]);
        float nextPosX = 0.0f;
        for (unsigned int j=0; j<row->count(); j++)
        {
            CCNode* l = dynamic_cast<CCNode*>(row->objectAtIndex(j));
            l->setAnchorPoint(Point::ZERO);
            l->setPosition(ccp(nextPosX, 0.0f));

			CCSize iSize;

			if(dynamic_cast<Widget*>(l) != NULL) {
				iSize = dynamic_cast<Widget*>(l)->getSize();
				_elementRenderersContainer->addChild(l, 1, j);
				/*	} else {
						iSize = l->getContentSize();
						_elementRenderersContainer->addNode(l, 1, j);*/
			}
            
            newContentSizeWidth += iSize.width;
            newContentSizeHeight = MAX(newContentSizeHeight, iSize.height);
            nextPosX += iSize.width;
        }

        _elementRenderersContainer->setSize(CCSizeMake(newContentSizeWidth, newContentSizeHeight));
    }
    else
    {
		float newContentSizeWidth = 0.0f;
        float newContentSizeHeight = 0.0f;
        float *maxHeights = new float[_elementRenderers.size()];
        
        for (unsigned int i=0; i<_elementRenderers.size(); i++)
        {
            CCArray* row = (CCArray*)(_elementRenderers[i]);
            float maxHeight = 0.0f;
            for (unsigned int j=0; j<row->count(); j++)
            {
                CCNode* l = (CCNode*)(row->objectAtIndex(j));
                maxHeight = MAX(l->getContentSize().height, maxHeight);
            }
            maxHeights[i] = maxHeight;
            newContentSizeHeight += maxHeights[i];
        }
        
        
        float nextPosY = 0;
        for (unsigned int i=0; i<_elementRenderers.size(); i++)
        {
            CCArray* row = (CCArray*)(_elementRenderers[i]);
            float nextPosX = 0.0f;
            nextPosY -= (maxHeights[i] + _verticalSpace);
            
            for (unsigned int j=0; j<row->count(); j++)
            {
                CCNode* l = dynamic_cast<CCNode*>(row->objectAtIndex(j));
                l->setAnchorPoint(Point::ZERO);
                l->setPosition(ccp(nextPosX, nextPosY));

				if(dynamic_cast<Widget*>(l) != NULL) {
					nextPosX += dynamic_cast<Widget*>(l)->getSize().width;
					_elementRenderersContainer->addChild(l, 1, i*10 + j);
					/*	} else {
							nextPosX += l->getContentSize().width;
							_elementRenderersContainer->addNode(l, 1, i*10 + j);*/
				}

				newContentSizeWidth = newContentSizeWidth > nextPosX ? newContentSizeWidth : nextPosX;
            }
        }
        _elementRenderersContainer->setSize(CCSizeMake(newContentSizeWidth, newContentSizeHeight));
        delete [] maxHeights;
    }
    _elementRenderers.clear();
    //if (_ignoreSize)
    //{
    //    CCSize s = getContentSize();
    //    _size = s;
    //}
    //else
    //{
    //    _size = _customSize;
    //}
}
    
void ExRichText::pushToContainer(cocos2d::CCNode *renderer)
{
    if (_elementRenderers.size() <= 0)
    {
        return;
    }
    _elementRenderers[_elementRenderers.size()-1]->addObject(renderer);
}

void ExRichText::setVerticalSpace(float space)
{
    _verticalSpace = space;
}

void ExRichText::setLimitWidth(float width) {
	_limitWidth = width;
	setSize(CCSizeMake(width, getSize().height));
}

void ExRichText::setAnchorPoint(const CCPoint &pt)
{
	Widget::setAnchorPoint(pt);
	_formatTextDirty = true;
}
    
const CCSize& ExRichText::getSize() const
{
	return _elementRenderersContainer->getSize();
}

void ExRichText::setSize(const CCSize &size) {
	Widget::setSize(size);

	_limitWidth = size.width;
}

void ExRichText::ignoreContentAdaptWithSize(bool ignore)
{
    if (_ignoreSize != ignore)
    {
        _formatTextDirty = true;
        Widget::ignoreContentAdaptWithSize(ignore);
    }
}

void ExRichText::setString(const std::string &text)
{
	setText(getRichFormatText(text));
}

void ExRichText::setText(const std::string &text) {
	_text = text;
	_richElements->removeAllObjects();

	std::stringstream ss;
	ss << "<root>" << text << "</root>";

	SAXParser parser;
	
	if(parser.init("UTF-8")) {
		parser.setDelegator(this);
		parser.parse(ss.str().c_str(), ss.str().size());

	} else {
		CCLOG("must be utf8 format");
	}

	_elementRenderersContainer->removeAllChildren();
//	_elementRenderersContainer->removeAllNodes();
	_elementRenderers.clear();

	addNewLine();

	_formatTextDirty = true;
	formatText();
}

void ExRichText::appendText(const std::string &text) {
	_text += text;
	
	std::stringstream ss;
	ss << "<root>" << text << "</root>";

	SAXParser parser;

	if(parser.init("UTF-8")) {
		parser.setDelegator(this);
		parser.parse(ss.str().c_str(), ss.str().size());

	} else {
		CCLOG("must be utf8 format");
	}

	_formatTextDirty = true;
	formatText();
}

void ExRichText::addElmtTouchEventListener(CCObject *pTarget, ui::SEL_TouchEvent selector) {
	_pListener = pTarget;
	_pSelector = selector;
}

void ExRichText::startElement(void *ctx, const char *name, const char **atts) {
	attrs_t attrs;

	if(atts != NULL) {
		for(size_t i = 0; atts[i] != NULL && atts[i+1] != NULL; i += 2) {
			attrs.insert(std::make_pair<std::string, std::string>(std::string(atts[i]), std::string(atts[i+1])));
		}
	}

	ExRichElement *pElmt = NULL;

	if(std::strcmp(name, "br") == 0) {
		pElmt = ExRichElementNewLine::create(0);
	} else if(std::strcmp(name, "txt") == 0) { // 文本
		std::string name = "";
		std::string face = "";
		unsigned int size = 24;
		ccColor3B color = ccc3(0xff, 0xff, 0xff);
		bool touchable = false;

		if(!attrs.empty()) {
			if(hasAttribute(attrs, "name")) {
				name = attrs["name"];
			}

			if(hasAttribute(attrs, "face")) {
				face = attrs["face"];
			}
			
			if(hasAttribute(attrs, "size")) {
				size = std::atoi(attrs["size"].c_str());
			}
			
			if(hasAttribute(attrs, "color")) {
				unsigned int r = str_to_hex(attrs["color"].substr(1, 2));
				unsigned int g = str_to_hex(attrs["color"].substr(3, 2));
				unsigned int b = str_to_hex(attrs["color"].substr(5, 2));
				color = ccc3(r, g, b);
			}

			if(hasAttribute(attrs, "touchable")) {
				touchable = std::string("true") == attrs["touchable"];
			}
			
		}
		
		Text *pText = Text::create();
		
		//Label * pText = Label::create();
		pText->setName(name);
		pText->setFontName(face);
		pText->setColor(color);
		pText->setFontSize(size);
			
		if (touchable) {
			pText->setTouchEnabled(touchable);
			pText->addTouchEventListener(this, toucheventselector(ExRichText::touchEventHandler));
		}

		pElmt = ExRichElementText::create(0, color, 0xff, pText);
	} else if(std::strcmp(name, "img") == 0) { // 图片
		const char *src = "";
		float width = 0.0f;
		float height = 0.0f;

		if(!attrs.empty()) {
			if(hasAttribute(attrs, "src")) {
				src = attrs["src"].c_str();
			}
			
			if(hasAttribute(attrs, "width")) {
				width = std::atof(attrs["width"].c_str());
			}
			
			if(hasAttribute(attrs, "height")) {
				height = std::atof(attrs["height"].c_str());
			}
			
		}

		CCSprite *pSprite = CCSprite::create(src);
		pSprite->setContentSize(CCSizeMake(width, height));
		pElmt = ExRichElementNode::create(1, ccc3(0xff, 0xff, 0xff), 0xff, pSprite);
	} else if(std::strcmp(name, "anim") == 0) { // 动画
		const char *src = "";
		float width = 0.0f;
		float height = 0.0f;
		unsigned int frames = 1;
		float delay = 0.1f;

		if(!attrs.empty()) {
			if(hasAttribute(attrs, "src")) {
				src = attrs["src"].c_str();
			}
			
			if(hasAttribute(attrs, "width")) {
				width = std::atof(attrs["width"].c_str());
			}
			
			if(hasAttribute(attrs, "height")) {
				height = std::atof(attrs["height"].c_str());
			}
			
			if(hasAttribute(attrs, "frames")) {
				frames = std::atoi(attrs["frames"].c_str());
			}
			
			if(hasAttribute(attrs, "delay")) {
				delay = std::atof(attrs["delay"].c_str());
			}
			
		}

		/*	AnimSprite *pAnim = AnimSprite::create(src, CCSizeMake(width, height), frames);
			pAnim->setContentSize(CCSizeMake(width, height));
			pElmt = ExRichElementNode::create(2, ccc3(0xff, 0xff, 0xff), 0xff, pAnim);*/
	}

	if(pElmt != NULL) {
		_pCurElmt = pElmt;
		pushBackElement(pElmt);
	}
	
}

void ExRichText::endElement(void *ctx, const char *name) {
	if(std::strcmp(name, "br") == 0) { // 换行
		
	}
}

void ExRichText::textHandler(void *ctx, const char *s, int len) {
	if(_pCurElmt->_type == EX_RICH_TEXT) {
		ExRichElementText *pRichElmt = dynamic_cast<ExRichElementText*>(_pCurElmt);
		pRichElmt->setText(s);
	}
}

void ExRichText::touchEventHandler(CCObject *pSender, ui::TouchEventType type) {
	if(_pListener != NULL && _pSelector != NULL) {
		(_pListener->*_pSelector)(pSender, type);
	}
}

int ExRichText::getLineNum()
{
	return _lineNum;
}
