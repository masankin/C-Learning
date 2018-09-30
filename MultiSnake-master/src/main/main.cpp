#include <QtCore/QCoreApplication>

#include "jargumentanalyse.h"

#include "network/jsnakeserver.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    JArgumentAnalyse aa(argc,argv);
	JSnakeServer server;
	server.run(60373);
	quint16 port=server.serverPort();
	qDebug()<<"snake server :"<<port;
    return a.exec();
}
