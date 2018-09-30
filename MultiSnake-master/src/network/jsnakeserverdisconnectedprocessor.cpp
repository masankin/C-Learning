#include "jsnakeserverdisconnectedprocessor.h"

#include "jsnakeserverprocessor.h"
#include "service/juserlistmanager.h"
#include "jsnakeserver.h"

#include <Socket/JSocket>
#include <Session/JSession>
#include <Processor/ProcessorType>
#include <Socket/JBroadcastCenter>

JSnakeServerDisconnectedProcessor* JSnakeServerDisconnectedProcessor::instance()
{
	static JSnakeServerDisconnectedProcessor instance;
	return &instance;
}

void JSnakeServerDisconnectedProcessor::process(JSocket* socket , const QByteArray&)
{
	JSnakeServer::broadcastCenter()->disconnect( socket );
	JSnakeServerProcessor::instance()->processEscapeRoom(socket);
	JUserlistManager ulm;
	ulm.removeUser(socket->session()->userId());
	
	socket->deleteLater() ;
}

JType JSnakeServerDisconnectedProcessor::getProcessorType()const
{
	return EPI_Disconnected;
}

JSnakeServerDisconnectedProcessor::JSnakeServerDisconnectedProcessor(QObject* parent)
	:JProcessor(parent)
{
}
