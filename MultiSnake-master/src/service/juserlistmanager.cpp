#include "juserlistmanager.h"

QMap<JID,JID> JUserlistManager::s_user2room;

JUserlistManager::JUserlistManager()
{
}

JID JUserlistManager::getRoomByUser(JID userId)const
{
	if( s_user2room.contains(userId) )
	{
		return s_user2room.value(userId);
	}else{
		return -1;
	}
}

QList<JID> JUserlistManager::getUserlistInRoom(JID roomId)const
{
	return s_user2room.keys(roomId);
}

JCode JUserlistManager::addUser(JID userId,JID roomId)
{
	s_user2room[userId]=roomId;
	return 0;
}

JCode JUserlistManager::moveUser(JID userId,JID roomId)
{
	s_user2room[userId]=roomId;
	return 0;
}

JCode JUserlistManager::removeUser(JID userId)
{
	if(!s_user2room.contains(userId))
	{
		return 1;
	}
	s_user2room.remove(userId);
	return 0;
}
