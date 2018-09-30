#ifndef JSNAKESERVERPROCESSOR_H
#define JSNAKESERVERPROCESSOR_H

#include <Processor/JProcessor>

#include "jsnake.h"

class JRoomManager;
class QPoint;

class JSnakeServerProcessor : public JProcessor
{
	Q_OBJECT
public:
	static JSnakeServerProcessor* instance();
	void process(JSocket* socket , const QByteArray& data);
	JType getProcessorType()const;
	
	void sendUserlist(JSocket* socket);
	void sendGameAct_getReady(JSocket* socket , bool ready,int num);
	void sendGameAct_countDown(JSocket* socket , int sec);
	void sendGameAct_getCommand(JSocket* socket);
	void sendGameAct_turn(JSocket* socket , JSnake::EDire dire,int num);
	void sendGameAct_collision(JSocket* socket , int num);
	void sendGameAct_createBean(JSocket* socket , const QPoint& pt);
	void sendGameAct_increase(JSocket* socket , int num);
	void sendGameAct_moveOn(JSocket* socket , int num);
	void sendGameAct_Stop(JSocket* socket);
	
	void processEnterRoom(JSocket* socket , JID roomId);
	void processEscapeRoom(JSocket* socket);
	
// broadcast functions
public slots:
	void sendRoominfoUpdate(JID roomId);
	void sendRoominfoAdd(JID roomId);
	void sendRoominfoDelete(JID roomId);
	void sendRoomEnter(JID roomId,JID userId);
	void sendRoomEscape(JID roomId,JID userId);
private:
	JRoomManager* m_roomMng;
	explicit JSnakeServerProcessor(QObject* parent=0);
};

#endif // JSNAKESERVERPROCESSOR_H
