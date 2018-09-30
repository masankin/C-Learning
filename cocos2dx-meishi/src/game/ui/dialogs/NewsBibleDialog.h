#pragma once

#include "game/ui/system/ActionWindow.h"
#include "data/TableManager.h"

#define NEWCAMP_TAG (101)

namespace CustomDropDownListBox
{
#define DROPDOWNLIST_NORMAL_COLOR3       Color3B(95,39,0)
#define DROPDOWNLIST_SELECTED_COLOR3     Color3B::WHITE

	class DropDownList : public Node
	{
	public:
		DropDownList() : mTabList(NULL), mCurTabId(0), mCurTabItemId(0), mTabCount(0), mTabItemSpace(0), mTabSpace(0){};
		~DropDownList()
		{
			mTabList.clear();
			/*MapTabItemList::iterator it = mTabItemList.begin();
			for (; it != mTabItemList.end(); ++it)
			{
				TabItemList& _list = it->second;
				TabItemList::iterator it1 = _list.begin();
				for (; it1 != _list.end(); ++it1)
				{
					CC_SAFE_RELEASE(*it1);
				}
			}*/
			mTabItemList.clear();
		};

		static DropDownList* create()
		{
			DropDownList* list = new DropDownList();
			list->autorelease();
			return list;
		};

		Button* addTab(std::string str, int starTag, int tabId)
		{
			std::string str1 = "ui/NewsBible/bigNormal.png";
			//if (tabId == NEWCAMP_TAG - 1)
				//str1 = "ui/NewsBible/newNormal.png";

			Button* tabBtn = Button::create(str1, str1, "", TextureResType::PLIST);
			mTabSize = tabBtn->getContentSize();
			tabBtn->setScale9Enabled(true);
			mTabList.push_back(tabBtn);

			tabBtn->setAnchorPoint(Point(0.5, 0.0));
			tabBtn->setPosition(Point(0.0, (mTabSize.height*mTabList.size() + (mTabList.size() - 1)*mTabSpace)*(-1)));
			addChild(tabBtn, 2);

			auto imageView = ImageView::create("ui/NewsBible/"+ str + "Normal.png", TextureResType::PLIST);
			imageView->setPosition(mTabSize / 2);
			tabBtn->addChild(imageView,1,1);

			tabBtn->setTouchEnabled(true);
			tabBtn->setCallbackType("Click");
			tabBtn->setCallbackName(Value(tabId).asString());
			tabBtn->setSwallowTouches(false);

			if (starTag && starTag != 4)//星星标签
			{
				auto imageView1 = ImageView::create(StringUtils::format("ui/NewsBible/star%d.png", starTag), TextureResType::PLIST);
				imageView1->setPosition(Point(12, tabBtn->getContentSize().height / 2));
				tabBtn->addChild(imageView1, 0, 12);
			}

			mTabCount++;

			return tabBtn;
		};

		Button* addTabItem(std::string str, int starTag, Button* mainTab, int tabItemId)
		{
			auto imageView = ImageView::create("ui/NewsBible/littleSelectedBg.png", TextureResType::PLIST);
			mTabItemSize = imageView->getContentSize();
			imageView->setPosition(mTabItemSize / 2);
			imageView->setVisible(false);

			Button* tabItemBtn = Button::create();
			tabItemBtn->addChild(imageView, -1, 1);

			int key = Value(mainTab->getCallbackName()).asInt();
			MapTabItemList::iterator it = mTabItemList.find(key);
			int size ;

			if (it == mTabItemList.end())
			{
				TabItemList _list;
				_list.push_back(tabItemBtn);
				mTabItemList.insert(std::make_pair(key, _list));
				size = 1;
			}
			else
			{
				TabItemList& _list = it->second;
				size = _list.size() + 1;
				_list.push_back(tabItemBtn);
			}

			tabItemBtn->ignoreContentAdaptWithSize(false);
			tabItemBtn->setContentSize(mTabItemSize);
			tabItemBtn->setTitleColor(DROPDOWNLIST_NORMAL_COLOR3);
			tabItemBtn->setTitleText(str);
			tabItemBtn->setTitleFontSize(26);
			tabItemBtn->setTitleFontName("font.TTF");
			tabItemBtn->setAnchorPoint(Point(0.5, 0.0));
			tabItemBtn->setPosition(Point(0.0, mainTab->getPositionY() + mTabItemSize.height*size*(-1) - mTabItemSpace));
			addChild(tabItemBtn, 1);

			tabItemBtn->setVisible(false);
			tabItemBtn->setCallbackName(Value(tabItemId).asString());
			tabItemBtn->setTouchEnabled(true);
			tabItemBtn->setCallbackType("Click");
			tabItemBtn->setSwallowTouches(false);

			if (starTag && starTag != 4)//星星标签
			{
				auto imageView1 = ImageView::create(StringUtils::format("ui/NewsBible/star%d.png",starTag), TextureResType::PLIST);
				imageView1->setPosition(Point(12, tabItemBtn->getContentSize().height/2));
				tabItemBtn->addChild(imageView1, 0, 12);
			}

			return tabItemBtn;
		};

		typedef std::list<Button*>	TabItemList;

		std::list<Button*> getTabItemListByTab(Button* mainTab)
		{
			int tagId = Value(mainTab->getCallbackName()).asInt();
			std::list<Button*> _list = std::list<Button*>();
			MapTabItemList::iterator it = mTabItemList.find(tagId);
			if (it != mTabItemList.end())
			{
				TabItemList::iterator lt = it->second.begin();
				for (; lt != it->second.end(); ++lt)
				{
					_list.push_back(*lt);
				}
			}

			return _list;
		};


		//列表展示
		void showTabItem(bool isShow, Button* tabBtn)
		{
			std::list<Button*> tabItemList = getTabItemListByTab(tabBtn);

			if (tabItemList.size() == 0)
				return;

			TabItemList::iterator it = tabItemList.begin();
			for (; it != tabItemList.end(); ++it)
			{
				(*it)->setVisible(isShow);
			}
		};

		//tab键移动
		void moveTab(bool isDown, Button* tabBtn)
		{
			std::list<Button*> tabItemList = getTabItemListByTab(tabBtn);
			int tabId = Value(tabBtn->getCallbackName()).asInt();
			bool tag = false;
			int j = 1;
			for (int i = 0; i < mTabList.size(); i++)
			{
				auto tab = mTabList.at(i);

				if (tag)
				{
					if (isDown)
						tab->setPositionY(tabBtn->getPositionY() + (mTabSize.height + mTabSpace)*j*(-1) + mTabItemSize.height*tabItemList.size()*(-1));
					else
						tab->setPositionY(tabBtn->getPositionY() + (mTabSize.height + mTabSpace)*j*(-1));

					auto m = tab->getPositionY();
					int n = 0;
					j++;
					continue;
				}

				if (tabId == Value(tab->getCallbackName()).asInt())
					tag = true;
			}
		};

		Button* getCurTab()
		{
			for (int i = 0; i < mTabList.size(); i++)
			{
				auto tab = mTabList.at(i);
				int tabId = Value(tab->getCallbackName()).asInt();
				if (tabId == mCurTabId)
					return tab;
			}
			return NULL;
		};

		int getTabCount(){ return mTabCount; };

		int getTabItemCountByTabId(int tabId)
		{
			MapTabItemList::iterator it = mTabItemList.find(tabId);
			if (it != mTabItemList.end())
				return it->second.size();
			return 0;
		};

		void onTabIsSelected(bool isSelected,Button* tabBtn)
		{
			int tabId = Value(tabBtn->getCallbackName()).asInt();
			const Bible* bible = sgTableManager()->getBible(tabId);
			ImageView* img = (ImageView*)(tabBtn->getChildByTag(1));

			std::string str = "ui/NewsBible/bigNormal.png";
			std::string str1 = "ui/NewsBible/bigSelected.png";
			//if (tabId == NEWCAMP_TAG - 1){
			//	str = "ui/NewsBible/newNormal.png";
			//	str1 = "ui/NewsBible/newSelected.png";
			//}
				
			if (isSelected)
			{
				tabBtn->loadTextures(str1, str1, "", TextureResType::PLIST);
				if (img)
					img->loadTexture("ui/NewsBible/" + bible->title + "Selected.png", TextureResType::PLIST);
			}
			else
			{
				tabBtn->loadTextures(str, str, "", TextureResType::PLIST);
				if (img)
					img->loadTexture("ui/NewsBible/" + bible->title + "Normal.png", TextureResType::PLIST);
			}
		};

		Button* getSelectedTabItemByTabId(int tabId)
		{
			Button* btn = NULL;
			Button* pbtn = NULL;
			MapTabItemList::iterator it = mTabItemList.find(tabId);
			if (it != mTabItemList.end())
			{
				TabItemList::iterator lt = it->second.begin();
				for (int i = 0; lt != it->second.end(); ++lt,++i)
				{
					if (i == 0)
						pbtn = (*lt);

					if (Value((*lt)->getCallbackName()).asInt() == mCurTabItemId)
						btn = (*lt);
				}
				if ((btn == NULL) && (it->second.size() != 0))
					btn = pbtn;
			}

			return btn;
		};

		void onTabItemIsSelected(bool isSelected, Button* tabBtn)
		{
			int tabId = Value(tabBtn->getCallbackName()).asInt();
			Button* tabItemBtn = getSelectedTabItemByTabId(tabId);
			if (tabItemBtn == NULL) return;

			ImageView* img = (ImageView*)(tabItemBtn->getChildByTag(1));

			if (isSelected)
			{
				tabItemBtn->setTitleColor(DROPDOWNLIST_SELECTED_COLOR3);

				if (img)
				img->setVisible(true);
			}
			else
			{
				tabItemBtn->setTitleColor(DROPDOWNLIST_NORMAL_COLOR3);
				if (img)
				img->setVisible(false);
			}
		};

	private:

		std::vector<Button*> mTabList;
		CC_SYNTHESIZE(int, mCurTabId, CurTabId);
		CC_SYNTHESIZE(float, mTabSpace, TabSpace);
		CC_SYNTHESIZE(Size, mTabSize, TabSize);
		int mTabCount;

		typedef std::map<int, TabItemList>	MapTabItemList;
		MapTabItemList mTabItemList;
		CC_SYNTHESIZE(int, mCurTabItemId, CurTabItemId);
		CC_SYNTHESIZE(float, mTabItemSpace, TabItemSpace);
		CC_SYNTHESIZE(Size, mTabItemSize, TabItemSize);
	};
}

class NewsBibleDialog : public ActionWindow
{
public:
	CREATE_FUNC(NewsBibleDialog);

protected:

	NewsBibleDialog();
	virtual ~NewsBibleDialog();

	virtual bool init();
	virtual void onMenuItemAction(std::string, cocos2d::Ref*);
	cocostudio::Armature* getArmatureById(int id);

	void onClickTab(cocos2d::Ref* sender);
	void onClickTabItem(cocos2d::Ref* sender);
	void onScroll(Ref* ref, Widget::TouchEventType type);

	void onEnter();
	void onExit();

	void aniCallback(cocostudio::Armature * pAr, cocostudio::MovementEventType type, const std::string& aniName);

	void newCampChangeBone();
private:

	void checkHasPassNewLevel();
	void updateBibleData(int id);

	CustomDropDownListBox::DropDownList* mListBox;
	cocostudio::Armature* mArmature;
	ImageView* mImage;
	bool mIsScroll;
	ui::ScrollView* mScrollView;
	int mNewPassLevel;
};
