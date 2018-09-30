#pragma once
#include "cocos2d.h"
#include "Constant.h"
#include "net/proto/ClientSvrMsg.pb.h"
#include "PropProxy.h"
#include "Message.h"
#include "PropItem.h"
USING_NS_CC;

class BaseBuild;
class PropsControl: public MessageHandler
{
public:
	PropsControl();
	~PropsControl();
	
	static const std::string PropTipsTimeKey;
	static const float	PropTipsTime;

public:
	virtual void onReceiveMassage(GameMessage * message);

	void	doExchange(int buildId, eCampType useCamp);
	void	doFullPeopol(int buildId,eCampType eCamp);
	void	doCookingSolider(const Vec2& pos,eCampType useCamp);
	void	doAddTime(eCampType useCamp);
	void	doFreeze(eCampType useCamp);
	void	doInvisibility(int buildId, eCampType useCamp);
	void	doOil(const Vec2& pos, eCampType useCamp);
	void	update();
	void	chechCdTimeOver();
	bool	checkIsCdTime(int propId);
	void	setCdTime(int propId);
	void    cleanCdTime(int propId);
	int		getUsePropNum();

	bool	isPropIng(){ return mPropIng; };
	bool	doProp(const Vec2& pos, eCampType useCamp , bool isNet = false);
	bool	setProp(int propId, bool isNet = false);
	ePropType	getCurPropType();
	int			getCurPropId();


	PropProxy* getPropProxy();
	void sendBuyPropRqst(int itemid, int price, int count);
	void sendUsePropRqst(int itemid, int count);

	void sendOpenChestRqst(int itemid, int count);

	void doBuyPropRsp(const fgame::CSBuyItemResp&);
	void doUsePropRsp(const fgame::CSUseItemResp&);

	void doOpenChestResp(const fgame::CSOpenChestResp&);

	void	clean();
	void	cleanCdTime();



	void	pushPropItem(ePropPos pos, PropItem*);
	PropItem*	getPropItem(ePropPos pos);
	PropItem*	getPropItem(int id);
	int		getPosPropId(ePropPos pos);
	void	updatePropItem(int propId);
	void	addPropItem(int propId);
	void	removePropItem(int propId);
	void	updatePropItemShow();
	void	updatePropTips();
	int		getSelectPropNum(); //已经选择的 道具有的多少个

	void	prePropEvet();
	void	propEvetCancel();
	void	showOrHideAimEffect(bool v);
	void	stopPropTipsAni();

	void showNoticeText(std::string str);

	void textCallBack();
	void hideOtherItem(int id);

	bool checkPropInLevel(ePropPos pos);
	bool hasPropInLevel();
	bool checkPropValid(BaseBuild* pBuild,eCampType useCamp);

	void init();
	void initPropCd();
	void initSaveProp();
	void initPvpProp();
	void initNeedTipProp();
	void initGuideProp(Node* pRootNode, int index);
	void addGuideProp(int propID);
	void clearGuideProp();
	void doUseGuideProp(Point pos);
	GuidePropItem* getGuideProp(int index);

	void setPropItemShow(bool show);

protected:

	typedef std::map<int, bool> CdList;
	typedef std::map<ePropPos, PropItem*> PropItemList;
	typedef std::vector<GuidePropItem* > GuidePropItemList;
	CdList mCdList;
	CC_SYNTHESIZE(ui::Text*, mInformTextHead, InfomTextHead);
	CC_SYNTHESIZE(ui::Text*, mInformTextBottom, InfomTextBottom);
	CC_SYNTHESIZE(ui::Button*, mCancel, CancelBtn);
	CC_SYNTHESIZE(Vec2, mTouchPos, TouchPos);
	CC_SYNTHESIZE(bool, isExchange, Exchange);//使用综合转换卷 标记
	CC_SYNTHESIZE(int, mSelectGuideProp, SelectGuideProp);

	ePropType mCurProp;//当前道具
	bool mPropIng;//道具使用中
	bool mNeedTipsProp;//是否提示使用道具
	int m_Id;
	int mUsePropNum;

	PropProxy m_PropProxy;
	PropItemList mPropItemList;
	GuidePropItemList mGuidePropItemList;

	std::vector<int> mAimList;

	
};

#define sgPropsControl() Singleton<PropsControl>::Get()
