#include "jroommanager.h"

#include "jsnakegameonserver.h"

JRoomManager JRoomManager::s_object;

JRoomManager::JRoomManager(QObject* parent)
	:QObject(parent)
{
}

JRoomManager& JRoomManager::getInstance()
{
	return s_object;
}

JCode JRoomManager::updateRoom(const Snake::JRoom& room)
{
	m_rooms[room.getRoomId()]=room;
	emit roomUpdated(room.getRoomId());
	return 0;
}

JCode JRoomManager::addRoom(Snake::JRoom& room)
{
	if(m_rooms.contains(room.getRoomId()))
	{
		return 1;
	}
	if(room.getRoomId()<0)
	{
		JID i;
		for(i=1;i<MAX_ROOM;++i)
		{
			if(!m_rooms.contains(i))
			{
				room.setRoomId(i);
				m_rooms.insert(room.getRoomId(),room);
				m_games.insert(room.getRoomId(),new JSnakeGameOnServer(room.getRoomId(),this));
				emit roomAdded(room.getRoomId());
				return 0;
			}
		}
		return 2;
	}
	m_rooms.insert(room.getRoomId(),room);
	m_games.insert(room.getRoomId(),new JSnakeGameOnServer(room.getRoomId(),this));
	emit roomAdded(room.getRoomId());
	return 0;
}

JCode JRoomManager::removeRoom(JID roomId)
{
	if(!m_rooms.contains(roomId))
	{
		return 1;
	}
	m_rooms.remove(roomId);
	JSnakeGameOnServer *game=m_games.value(roomId);
	if(game!=NULL)
	{
		game->deleteLater();
	}
	emit roomRemoved(roomId);
	return 0;
}

JCode JRoomManager::enterRoom(JID roomId,JID userId)
{
	if(!m_rooms.contains(roomId))
	{
		return 1;
	}
	if(m_rooms[roomId].isFull())
	{
		return 2;
	}
	Snake::JRoom &room=m_rooms[roomId];
	room.enterRoom(userId);
	JSnakeGameOnServer *game=m_games.value(roomId);
	Q_ASSERT(game!=NULL);
	game->enter(room.getPositionById(userId));
	emit roomEnter(roomId,userId);
	emit roomUpdated(roomId);
	return 0;
}

JCode JRoomManager::escapeRoom(JID roomId,JID userId)
{
	if(0==roomId)
	{
		emit roomEscape(roomId,userId);
		return 0;
	}
	if(!m_rooms.contains(roomId))
	{
		return 1;
	}
	Snake::JRoom &room=m_rooms[roomId];
	room.leaveRoom(userId);
	JSnakeGameOnServer *game=m_games.value(roomId);
	Q_ASSERT(game!=NULL);
	game->escape(room.getPositionById(userId));
	emit roomEscape(roomId,userId);
	if(room.isEmpty())
	{
		m_rooms.remove(roomId);
		game->deleteLater();
		m_games.remove(roomId);
		emit roomRemoved(roomId);
	}else{
		emit roomUpdated(roomId);
	}
	return 0;
}

JCode JRoomManager::enterHall(JID userId)
{
	emit roomEnter(0,userId);
	return 0;
}

Snake::JRoom JRoomManager::getRoomInfo(JID roomId)const
{
	return m_rooms.value(roomId);
}

JSnakeGameOnServer* JRoomManager::getGame(JID roomId)const
{
	return m_games.value(roomId,NULL);
}

QList<JID> JRoomManager::getIdList()const
{
	return m_rooms.keys();
}

QList<Snake::JRoom> JRoomManager::getRoomList()const
{
	return m_rooms.values();
}
