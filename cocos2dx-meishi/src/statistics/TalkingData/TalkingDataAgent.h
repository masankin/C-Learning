#pragma once

#include "cocos2d.h"
#include "Singleton.h"

class TDCCAccount;

class TalkingDataAgent : cocos2d::Ref
{
public:
	enum AccountType {
		kAccountAnonymous = 0,
		kAccountRegistered = 1,
		kAccountSianWeibo = 2,
		kAccountQQ = 3,
		kAccountTencentWeibo = 4,
		kAccountND91 = 5,
		kAccountType1 = 11,
		kAccountType2 = 12,
		kAccountType3 = 13,
		kAccountType4 = 14,
		kAccountType5 = 15,
		kAccountType6 = 16,
		kAccountType7 = 17,
		kAccountType8 = 18,
		kAccountType9 = 19,
		kAccountType10 = 20
	};

	enum Gender {
		kGenderUnknown = 0,
		kGenderMale = 1,
		kGenderFemale = 2
	};

	typedef std::pair< std::string, std::string >   EventParamPair;
	typedef std::map< std::string, std::string >    EventParamMap;

public:
	TalkingDataAgent();
	~TalkingDataAgent();

	bool initilized();


	void onStart(const std::string& appId, const std::string& channelId);

	void onEvent(const std::string& eventId, EventParamMap* map = NULL);

	void setLatitude(double latitude, double longitude);

	std::string getDeviceId();

	void onKill();

	void setVerboseLogDisabled();

	void setVerboseLogEnabled();

	
	void setAccount(const std::string& accountId);

	void accountSetName(const std::string& accountName);

	void accountSetType(AccountType accountType);

	void accountSetLevel(int level);

	void accountSetGender(Gender gender);

	void accountSetAge(int age);

	void accountSetGameServer(const std::string& gameServer);


	void missionBegin(const std::string& missionId);

	void missionCompleted(const std::string& missionId);

	void missionFailed(const std::string& missionId, const std::string& failedCause);


	void itemPurchase(const std::string& item, int number, double price);

	void itemUse(const std::string& item, int number);


	void virtualCurrencyChargeRequest(const std::string& orderId, const std::string& iapId, double currencyAmount, const std::string& currencyType, double virtualCurrencyAmount, const std::string& paymentType);

	void virtualCurrencyChargeSuccess(const std::string& orderId);

	void virtualCurrencyReward(double currencyAmount, const std::string& reason);

private:
	bool mEnable;
	TDCCAccount* mTDAccount;
};

#define sgTalkingDataAgent() Singleton<TalkingDataAgent>::Get()
