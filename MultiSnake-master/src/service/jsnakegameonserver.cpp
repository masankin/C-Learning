#include "jsnakegameonserver.h"

#include <QTimer>
#include <QCoreApplication>
#include <QTime>

#include "network/jsnakeserverroomsocketmanager.h"
#include "network/jsnakeserverprocessor.h"

JSnakeGameOnServer::JSnakeGameOnServer(JID roomId,QObject *parent) :
    QObject(parent),
    m_roomId(roomId)
{
	m_timer=new QTimer(this);
	m_game=new JSnakeGame;
	connect(m_timer,
			SIGNAL(timeout()),
			SLOT(on_timer_timeout()));
	qsrand(QTime::currentTime().msecsTo(QTime()));
	for(int i=0;i<NUM_SNAKE;++i)
	{
		m_sit[i]=false;
	}
	reset();
}

JSnakeGameOnServer::~JSnakeGameOnServer()
{
	delete m_game;
	m_game=NULL;
}

void JSnakeGameOnServer::enter(int num)
{
	if(num>=0 && num<NUM_SNAKE)
	{
		m_sit[num]=true;

	}
}

void JSnakeGameOnServer::escape(int num)
{
	if(num>=0 && num<NUM_SNAKE)
	{
		m_sit[num]=false;
		m_game->clearSnake(num);
	}
	stop();
}

void JSnakeGameOnServer::ready(bool ready,int num)
{
	if(num>=0 && num<NUM_SNAKE)
	{
		m_ready[num]=ready;
		m_game->resetSnake(num);
		m_game->resetLifeScore(num);
		//emit getReady(ready,num);
		
		if(canStart())
		{
			start(Elapse);
		}
	}
}

void JSnakeGameOnServer::start(int msec)
{
	m_hasStarted=true;
	m_interval_msec=msec;
	m_countDown=4;
	m_timer->start(1000);
}

void JSnakeGameOnServer::setTurn(JSnake::EDire dire,int num)
{
	if(num>=0 && num<NUM_SNAKE)
	{
		if(m_sit[num])
		{
			if(dire>=0 && dire<JSnake::ED_NONE)
			{
				m_dires[num]=dire;
			}
		}
	}
}

void JSnakeGameOnServer::stop()
{
	m_timer->stop();
	reset();
	emit getStop();
}

bool JSnakeGameOnServer::isReady(int num)const
{
	if(num>=0 && num<NUM_SNAKE)
	{
		return m_ready[num];
	}
	return false;
}

void JSnakeGameOnServer::getReady(bool ready,int num){
	JSnakeServerRoomSocketManager *ssrsm=JSnakeServerRoomSocketManager::instance();
	JSnakeServerProcessor *processor = JSnakeServerProcessor::instance();
	foreach(JSocket *socketInRoom,ssrsm->socketListInRoom( m_roomId ) ){
		processor->sendGameAct_getReady(
			socketInRoom,
			ready,
			num
		);
	}
}

void JSnakeGameOnServer::countDown(int sec){
	JSnakeServerRoomSocketManager *ssrsm=JSnakeServerRoomSocketManager::instance();
	JSnakeServerProcessor *processor = JSnakeServerProcessor::instance();
	foreach(JSocket *socketInRoom,ssrsm->socketListInRoom( m_roomId ) ){
		processor->sendGameAct_countDown(
			socketInRoom,
			sec
		);
	}
}

void JSnakeGameOnServer::getCommand(){
	JSnakeServerRoomSocketManager *ssrsm=JSnakeServerRoomSocketManager::instance();
	JSnakeServerProcessor *processor = JSnakeServerProcessor::instance();
	foreach(JSocket *socketInRoom,ssrsm->socketListInRoom( m_roomId ) ){
		processor->sendGameAct_getCommand(
			socketInRoom
		);
	}
}

void JSnakeGameOnServer::turn(JSnake::EDire dire,int num){
	JSnakeServerRoomSocketManager *ssrsm=JSnakeServerRoomSocketManager::instance();
	JSnakeServerProcessor *processor = JSnakeServerProcessor::instance();
	foreach(JSocket *socketInRoom,ssrsm->socketListInRoom( m_roomId ) ){
		processor->sendGameAct_turn(
			socketInRoom,
			dire,
			num
		);
	}
}

void JSnakeGameOnServer::collision(int num){
	JSnakeServerRoomSocketManager *ssrsm=JSnakeServerRoomSocketManager::instance();
	JSnakeServerProcessor *processor = JSnakeServerProcessor::instance();
	foreach(JSocket *socketInRoom,ssrsm->socketListInRoom( m_roomId ) ){
		processor->sendGameAct_collision(
			socketInRoom,
			num
		);
	}
}

void JSnakeGameOnServer::createBean(const QPoint& pt){
	JSnakeServerRoomSocketManager *ssrsm=JSnakeServerRoomSocketManager::instance();
	JSnakeServerProcessor *processor = JSnakeServerProcessor::instance();
	foreach(JSocket *socketInRoom,ssrsm->socketListInRoom( m_roomId ) ){
		processor->sendGameAct_createBean(
			socketInRoom,
			pt
		);
	}
}

void JSnakeGameOnServer::increase(int num){
	JSnakeServerRoomSocketManager *ssrsm=JSnakeServerRoomSocketManager::instance();
	JSnakeServerProcessor *processor = JSnakeServerProcessor::instance();
	foreach(JSocket *socketInRoom,ssrsm->socketListInRoom( m_roomId ) ){
		processor->sendGameAct_increase(
			socketInRoom,
			num
		);
	}
}

void JSnakeGameOnServer::moveOn(int num){
	JSnakeServerRoomSocketManager *ssrsm=JSnakeServerRoomSocketManager::instance();
	JSnakeServerProcessor *processor = JSnakeServerProcessor::instance();
	foreach(JSocket *socketInRoom,ssrsm->socketListInRoom( m_roomId ) ){
		processor->sendGameAct_moveOn(
			socketInRoom,
			num
		);
	}
}

void JSnakeGameOnServer::getStop(){
	JSnakeServerRoomSocketManager *ssrsm=JSnakeServerRoomSocketManager::instance();
	JSnakeServerProcessor *processor = JSnakeServerProcessor::instance();
	foreach(JSocket *socketInRoom,ssrsm->socketListInRoom( m_roomId ) ){
		processor->sendGameAct_Stop(
			socketInRoom
		);
	}
}

void JSnakeGameOnServer::on_timer_timeout()
{
	if(m_countDown>0)
	{
		--m_countDown;
		emit countDown(m_countDown);
		if(m_countDown<=0)
		{
			m_timer->stop();
			m_timer->start(m_interval_msec);
			QPoint pt;
			pt.setX(qrand()%MAXX);
			pt.setY(qrand()%MAXY);
			emit createBean(pt);
			m_game->setBean(pt);
		}
	}else{
		int i;
		for(i=0;i<NUM_SNAKE;++i)
		{
			m_dires[i]=JSnake::ED_NONE;
		}
		emit getCommand();
		
		/*
		JElapsedTimer timer;
		timer.start();
		while(timer.elapsed()< m_interval_msec*0.2)
		{
			QCoreApplication::processEvents();
		}
		*/
		QEventLoop el;
		QTimer::singleShot(
			m_interval_msec*0.2 ,
			&el,
			SLOT(quit())
		);
		el.exec();
		
		for(i=0;i<NUM_SNAKE;++i)
		{
			if(m_sit[i] && m_dires[i]!=JSnake::ED_NONE)
			{
				emit turn(m_dires[i],i);
				m_game->turn(m_dires[i],i);
			}
		}
		qint16 bit=m_game->getMoveOnCollision();
		for(i=0;i<NUM_SNAKE;++i)
		{
			if(m_sit[i])
			{
				if(m_game->isSnakeCollision(i,bit))
				{
					m_game->resetSnake(i);
					m_game->decreaseLife(i);
					emit collision(i);
					if(canStop())
					{
						stop();
						return;
					}
				}else{
					m_game->moveOn(i);
					emit moveOn(i);
				}
			}
		}
		if(m_game->isBeanCollision(bit))
		{
			QPoint pt;
			pt.setX(qrand()%MAXX);
			pt.setY(qrand()%MAXY);
			emit createBean(pt);
			m_game->setBean(pt);
			int num=m_game->getBeanCollitionSnakeNumber(bit);
			m_game->increaseScore(num);
			m_game->growLonger(num);
			emit increase(num);
		}
	}
}

bool JSnakeGameOnServer::canStart()
{
	int nPlayers=0;
	for(int i=0;i<NUM_SNAKE;++i)
	{
		if(m_sit[i])
		{
			++nPlayers;
			if(!m_ready[i])
			{
				return false;
			}
		}
	}
	if(nPlayers>1) return true;
	else return false;
}

bool JSnakeGameOnServer::canStop()
{
	for(int i=0;i<NUM_SNAKE;++i)
	{
		if(m_sit[i])
		{
			if(m_game->getLife(i)>0) return false;
		}
	}
	return true;
}

void JSnakeGameOnServer::reset()
{
	m_game->resetSnake();
	m_game->clearLifeScore();
	for(int i=0;i<NUM_SNAKE;++i)
	{
		m_ready[i]=false;
		emit getReady(m_ready[i],i);
	}
	m_hasStarted=false;
}
