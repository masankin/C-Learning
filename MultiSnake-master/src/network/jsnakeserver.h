#ifndef JSNAKESERVER_H
#define JSNAKESERVER_H

#include <Server/JServerBase>

class JBroadcastCenter;

class JSnakeServer : public JServerBase
{
    Q_OBJECT
public:
    explicit JSnakeServer(QObject *parent = 0);
	static JBroadcastCenter* broadcastCenter();
protected:
	JSocket* getConnection(QTcpSocket* socket,QObject* parent)const;
};

#endif // JSNAKESERVER_H
