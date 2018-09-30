#ifndef JROOMMANAGER_H
#define JROOMMANAGER_H

#include <QObject>
#include <QMap>

#include "jsnakeglobal.h"

class JSnakeGameOnServer;

const JID MAX_ROOM=65535;

class JRoomManager : public QObject
{
	Q_OBJECT
private:
	explicit JRoomManager(QObject* parent=0);
	static JRoomManager s_object;
public:
	static JRoomManager& getInstance();
	JCode updateRoom(const Snake::JRoom&);
	// if roomId is less than 0 , function addRoom will
	// assign an Id for the room .
	JCode addRoom(Snake::JRoom&);
	JCode removeRoom(JID roomId);
	JCode enterRoom(JID roomId,JID userId);
	JCode escapeRoom(JID roomId,JID userId);
	JCode enterHall(JID userId);
	Snake::JRoom getRoomInfo(JID roomId)const;
	JSnakeGameOnServer* getGame(JID roomId)const;
	QList<JID> getIdList()const;
	QList<Snake::JRoom> getRoomList()const;
signals:
	void roomUpdated(JID roomId);
	void roomAdded(JID roomId);
	void roomRemoved(JID roomId);
	void roomEnter(JID roomId,JID userId);
	void roomEscape(JID roomId,JID userId);
private:
	QMap<JID,Snake::JRoom> m_rooms;
	QMap<JID,JSnakeGameOnServer*>m_games;
};

#endif // JROOMMANAGER_H
