#ifndef JUSERLISTMANAGER_H
#define JUSERLISTMANAGER_H

#include <Global/Global>

#include <QList>
#include <QMap>

class JUserlistManager
{
public:
	JUserlistManager();
	JID getRoomByUser(JID userId)const;
	QList<JID> getUserlistInRoom(JID roomId)const;
	JCode addUser(JID userId,JID roomId);
	JCode moveUser(JID userId,JID roomId);
	JCode removeUser(JID userId);
private:
	static QMap<JID,JID> s_user2room;
};

#endif // JUSERLISTMANAGER_H
