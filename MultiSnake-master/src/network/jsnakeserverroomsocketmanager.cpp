#include "jsnakeserverroomsocketmanager.h"

JSnakeServerRoomSocketManager* JSnakeServerRoomSocketManager::instance()
{
	static JSnakeServerRoomSocketManager instance;
	return &instance;
}

void JSnakeServerRoomSocketManager::enterRoom(JID roomId,JSocket* socket)
{
	m_socketList[roomId].insert(socket);
}

void JSnakeServerRoomSocketManager::escapeRoom(JID roomId,JSocket* socket)
{
	m_socketList[roomId].remove(socket);
}

QSet<JSocket*> JSnakeServerRoomSocketManager::socketListInRoom(JID roomId)const
{
	return m_socketList[roomId];
}

JSnakeServerRoomSocketManager::JSnakeServerRoomSocketManager()
{
}
