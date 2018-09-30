#pragma once
#include "cocos2d.h"
#include "Constant.h"

#include "BaseProxy.h"
#include "ui/dialogs/ActivationDialog.h"

class ActivationProxy : public BaseProxy
{
public:

	ActivationProxy();
	~ActivationProxy();

	virtual void DoRsp(NetPack& stRsp);
	virtual bool fault(const fgame::CSErrorMsg &err);

	void sendGiftCodeRqst(std::string code);

};

class ActivationControl:public Node
{
public:
	ActivationControl(void);
	~ActivationControl(void);
	
	CC_SYNTHESIZE(ActivationDialog*, mActivationDialog, ActivationDialog);

	void sendGiftCodeRqst(std::string code);
protected:
	ActivationProxy mActivationProxy;
};

#define sgActivationControl() Singleton<ActivationControl>::Get()
