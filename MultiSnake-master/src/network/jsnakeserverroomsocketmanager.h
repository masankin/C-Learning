#ifndef JSNAKESERVERROOMSOCKETMANAGER_H
#define JSNAKESERVERROOMSOCKETMANAGER_H

#include <QMap>
#include <QSet>

#include <Global/Global>

class JSocket;

class JSnakeServerRoomSocketManager
{
public:
	static JSnakeServerRoomSocketManager* instance();
	void enterRoom(JID roomId,JSocket* socket);
	void escapeRoom(JID roomId,JSocket* socket);
	QSet<JSocket*> socketListInRoom(JID roomId)const;
private:
	JSnakeServerRoomSocketManager();
	QMap<JID,QSet<JSocket*> > m_socketList;
};

#endif /* JSNAKESERVERROOMSOCKETMANAGER_H */

