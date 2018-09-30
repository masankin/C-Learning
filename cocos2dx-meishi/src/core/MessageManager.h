#pragma once
#include <map>
#include <set>
#include <list>
#include "Singleton.h"


#include "Message.h"

#include "cocos2d.h"
USING_NS_CC;


class MessageManager
{
public:

	MessageManager();

	void loadFactorys();

	void update();
	void sendMessage(const GameMessage* message);
	void sendMessage(eMessageType eType, std::string params);

	void regisiterMessageHandler(unsigned int id, MessageHandler* handler);
	void removeMessageHandler(unsigned int id, MessageHandler* messagehandler);
	void removeMessageHandler(MessageHandler* messagehandler);
	void removeMessageHandler(unsigned int id);

	bool registerFactory(int id, GameMessageFactory* factory);	//id-eMessageType
	const GameMessage*	createMessage(int id, std::string params);

private:
	// callback list
	typedef std::map<unsigned int,std::set<MessageHandler*> > MESSAGE_HANDLER_MAP;
	MESSAGE_HANDLER_MAP mHandlers;
	// message list
	typedef std::list<GameMessage*> MESSAGE_LIST;
	MESSAGE_LIST mMessages;

	// message factory list
	typedef std::map<int, GameMessageFactory*> MessageFactoryMap;
	MessageFactoryMap mMessageFactoryMap;
};

#define sgMessageManager() Singleton<MessageManager>::Get()
