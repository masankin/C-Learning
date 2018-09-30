#pragma once

#include "BaseProxy.h"
#include "data/Tables.h"

class ShopProxy: public BaseProxy
{
public:
	ShopProxy(void);
	~ShopProxy(void);

	void DoRsp(NetPack& stRsp);
	bool fault(const fgame::CSErrorMsg &err);

	void sendShopBuy(int id);
	void sendShopList();

	
private:
	std::map<int, ShopInfo> mShopList;
};
#define sgShopProxy() Singleton<ShopProxy>::Get()
