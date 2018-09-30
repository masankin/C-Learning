#pragma once

#include "game/ui/system/ActionWindow.h"
#include "cocos2d.h"
#include "cocos-ext.h"
#include "game/Message.h"

USING_NS_CC_EXT;
USING_NS_CC;

class HonorShopItem;

class HonorShopDialog : public ActionWindow, public MessageHandler, public TableViewDataSource, public TableViewDelegate
{
public:
	HonorShopDialog();
	virtual ~HonorShopDialog();

	bool			init();
	void			onShow();
	void			updateItem(int itemId);
	void			refreshShop();
	virtual void	onMenuItemAction(std::string, cocos2d::Ref*);
	virtual void	onReceiveMassage(GameMessage *message);
	void			goGetHonorCallback(cocos2d::Ref* sender);

	void			sellOutCallback(cocos2d::Ref* sender);
	CREATE_FUNC(HonorShopDialog);

	virtual bool tableViewMove(bool isRigth);

	virtual Size tableCellSizeForIndex(TableView *table, ssize_t idx);
	virtual ssize_t numberOfCellsInTableView(TableView *table);
	virtual void tableCellTouched(TableView* table, TableViewCell* cell){};
	virtual TableViewCell* tableCellAtIndex(TableView *table, ssize_t idx);
private:
	int				getMaxPageCount();
	int				getCurPage(float pos);
private:
	TableView*		mHonorTableView;
	int				mItemIndex;  //·­Ò³ÆðÊ¼µã
	ui::Text*		mHonorCountLabel;
};
