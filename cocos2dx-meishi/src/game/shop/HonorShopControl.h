#pragma once

#include "BaseProxy.h"
#include "data/Tables.h"

class HonorShopItemData: public cocos2d::Ref
{
public:
	HonorShopItemData();
	virtual ~HonorShopItemData();
	bool init();
	CREATE_FUNC(HonorShopItemData);

	CC_SYNTHESIZE(int, mItemId, ItemId);	
	CC_SYNTHESIZE(int, mAllLeftCount, AllLeftCount)
	CC_SYNTHESIZE(int, mExchangeLeftCount, ExchangeLeftCount);  // 兑换剩余次数
	CC_SYNTHESIZE(int, mRefreshTime, RefreshTime);				// 剩余刷新时间
};

class HonorShopProxy : public BaseProxy
{
public:
	HonorShopProxy(void);
	~HonorShopProxy(void);

	void DoRsp(NetPack& stRsp);
	bool fault(const fgame::CSErrorMsg &err);

	void	sendGetExchangeInfoReq();
	void	sendExchangeReq(int itemId);
private:
};
#define sgHonorShopProxy() Singleton<HonorShopProxy>::Get()

class HonorShopControl
{
public:
	HonorShopControl();
	virtual ~HonorShopControl();

	void		sendGetExchangeInfoReq();
	void		sendExchangeItemReq(int itemId);

	void		doSyncExchangeInfo(const fgame::CSUpdateExchangeListResp &listResp);
	void		doUpdateExchangeItem(const fgame::CSExchangeItemResp &itemResp);

	void		addItem(HonorShopItemData *data);
	void		removeItemById(int itemId);
	void		clearShopData() { mShopDataList.clear();  }
	int			getShopDataCount() { return mShopDataList.size(); }
	bool		checkHonorCountEnough(int itemId);

	void		try2SyncExchangeInfo();

	HonorShopItemData* getShopDataById(int itemId);
	int			getItemIdByIndex(int index);
private:
	cocos2d::Vector<HonorShopItemData*> mShopDataList;
};

#define sgHonorShopControl() Singleton<HonorShopControl>::Get()
