#include "jsnakeserver.h"

#include "jsnakeserverprocessor.h"
#include "jsnakeserverdisconnectedprocessor.h"

#include <Socket/JServerSocket>
#include <Socket/JBroadcastCenter>

JSnakeServer::JSnakeServer(QObject *parent) :
    JServerBase(parent)
{
}

JBroadcastCenter* JSnakeServer::broadcastCenter()
{
	static JBroadcastCenter bcc;
	return &bcc;
}

JSocket* JSnakeServer::getConnection(QTcpSocket* socket,QObject* parent)const
{
	JServerSocket* connection = new JServerSocket(socket,parent);
	
	connect(
		broadcastCenter(),
		SIGNAL(broadcast(JType,QByteArray)),
		connection,
		SLOT(on_broadcast(JType,QByteArray))
	);
	
	connection->registerProcessor( JSnakeServerProcessor::instance() );
	connection->registerProcessor( JSnakeServerDisconnectedProcessor::instance() );
	return connection;
}
