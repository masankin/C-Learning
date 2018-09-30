#include "jsnakeserverprocessor.h"

#include "jsnakeglobal.h"
#include "service/juserlistmanager.h"
#include "service/jroommanager.h"
#include "service/jsnakegameonserver.h"
#include "jsnakeserver.h"
#include "jsnakeserverroomsocketmanager.h"

#include <Socket/JSocket>
#include <Session/JSession>
#include <Socket/JBroadcastCenter>

JSnakeServerProcessor* JSnakeServerProcessor::instance()
{
	static JSnakeServerProcessor instance;
	return &instance;
}

JSnakeServerProcessor::JSnakeServerProcessor(QObject* parent) :
	JProcessor(parent)
{
	m_roomMng=&JRoomManager::getInstance();
	connect(m_roomMng,SIGNAL(roomAdded(JID)),SLOT(sendRoominfoAdd(JID)));
	connect(m_roomMng,SIGNAL(roomRemoved(JID)),SLOT(sendRoominfoDelete(JID)));
	connect(m_roomMng,SIGNAL(roomUpdated(JID)),SLOT(sendRoominfoUpdate(JID)));
	connect(m_roomMng,SIGNAL(roomEnter(JID,JID)),SLOT(sendRoomEnter(JID,JID)));
	connect(m_roomMng,SIGNAL(roomEscape(JID,JID)),SLOT(sendRoomEscape(JID,JID)));
}

void JSnakeServerProcessor::process(JSocket* socket , const QByteArray& data)
{
	using namespace SnakeProtocol;
	QDataStream stream(data);
	JProtocol protocol;
	stream>>protocol;
	qDebug()<<"JSnakeServerProcessor::process"<<" protocol:"<<protocol;
	switch(protocol)
	{
	case SP_Hello :
		{
			JID userId;
			stream>>userId;
			socket->session()->setUserId(userId);
			JCode code;
			JUserlistManager ulm;
			code=ulm.addUser(userId,0);
			QByteArray outdata;
			QDataStream outstream(&outdata,QIODevice::WriteOnly);
			outstream<<SP_Hello;
			outstream<<code;
			sendData(socket,outdata);
			m_roomMng->enterHall(userId);
		}
		break;
	case SP_Roomlist :
		{
			QList<Snake::JRoom> roomlist;
			roomlist=m_roomMng->getRoomList();
			QByteArray outdata;
			QDataStream outstream(&outdata,QIODevice::WriteOnly);
			outstream<<SP_Roomlist;
			outstream<<roomlist;
			sendData(socket,outdata);
		}
		break;
	case SP_RoominfoUpdate :
		{
			JID roomId;
			stream>>roomId;
			qDebug()<<"unfinish protocol : SP_RoominfoUpdate : roomid="<<roomId;
		}
		break;
	case SP_RoominfoAdd :
		{
			Snake::JRoom roominfo;
			stream>>roominfo;
			JCode code=m_roomMng->addRoom(roominfo);
			if(0==code)
			{
				processEscapeRoom(socket);
				processEnterRoom(socket,roominfo.getRoomId());
			}
		}
		break;
	case SP_RoominfoDelete :
		qDebug()<<"SP_RoominfoDelete : clinet will never send this .";
		break;
	case SP_RoomAct :
		qDebug()<<"SP_RoomAct : clinet will never send this .";
		break;
	case SP_RoomEnter :
		{
			JID roomId;
			stream>>roomId;
			processEscapeRoom(socket);
			processEnterRoom(socket,roomId);
		}
		break;
	case SP_RoomEscape :
		{
			processEscapeRoom(socket);
			m_roomMng->enterHall(socket->session()->userId());
		}
		break;
	case SP_Userlist :
		sendUserlist(socket);
		break;
	case SP_GameAct:
		qDebug()<<"SP_GameAct : clinet will never send this .";
		break;
	case SP_GA_Ready:
		{
			qDebug()<<__FUNCTION__<<__LINE__<<"SP_GA_Ready";
			bool ready;
			stream>>ready;
			JID userId;
			JID roomId;
			userId = socket->session()->userId();
			qDebug()<<__FUNCTION__<<__LINE__<<"userId:"<<userId;
			
			JUserlistManager ulm;
			roomId=ulm.getRoomByUser(userId);
			qDebug()<<__FUNCTION__<<__LINE__<<"roomId:"<<roomId;
			
			JSnakeGameOnServer *game=m_roomMng->getGame(roomId);
			Q_ASSERT(game!=NULL);
			Snake::JRoom room=m_roomMng->getRoomInfo(roomId);
			int position = room.getPositionById(socket->session()->userId());
			game->ready(ready,position);
			
			JSnakeServerRoomSocketManager *ssrsm=JSnakeServerRoomSocketManager::instance();
			foreach(JSocket *socketInRoom,ssrsm->socketListInRoom( roomId ) ){
				qDebug()<<__FUNCTION__<<__LINE__<<"socketInRoom";
				sendGameAct_getReady(
					socketInRoom,
					ready,
					position
				);
			}
		}
		break;
	case SP_GA_CountDown:
		qDebug()<<"SP_GA_CountDown : clinet will never send this .";
		break;
	case SP_GA_GetCommand:
		qDebug()<<"SP_GA_GetCommand : clinet will never send this .";
		break;
	case SP_GA_Turn:
		{
			qint16 dire;
			stream>>dire;
			if(dire==JSnake::ED_NONE) break;
			JUserlistManager ulm;
			JID roomId;
			roomId=ulm.getRoomByUser(socket->session()->userId());
			JSnakeGameOnServer *game=m_roomMng->getGame(roomId);
			Q_ASSERT(game!=NULL);
			Snake::JRoom room=m_roomMng->getRoomInfo(roomId);
			game->setTurn((JSnake::EDire)dire,room.getPositionById(socket->session()->userId()));
		}
		break;
	case SP_GA_Collision:
		qDebug()<<"SP_GA_Collision : clinet will never send this .";
		break;
	case SP_GA_CreateBean:
		qDebug()<<"SP_GA_CreateBean : clinet will never send this .";
		break;
	case SP_GA_Increase:
		qDebug()<<"SP_GA_Increase : clinet will never send this .";
		break;
	case SP_GA_MoveOn:
		qDebug()<<"SP_GA_MoveOn : clinet will never send this .";
		break;
	}
}

JType JSnakeServerProcessor::getProcessorType()const
{
	return Snake::SnakeProcessorType;
}

void JSnakeServerProcessor::sendUserlist(JSocket* socket)
{
	using namespace SnakeProtocol;
	JUserlistManager ulm;
	JID roomId;
	roomId=ulm.getRoomByUser(socket->session()->userId());
	QList<JID> userlist=ulm.getUserlistInRoom(roomId);
	QByteArray outdata;
	QDataStream outstream(&outdata,QIODevice::WriteOnly);
	outstream<<SP_Userlist;
	outstream<<roomId;
	outstream<<userlist;
	sendData(socket,outdata);
}

void JSnakeServerProcessor::sendRoominfoUpdate(JID roomId)
{
	using namespace SnakeProtocol;
	QByteArray outdata;
	QDataStream outstream(&outdata,QIODevice::WriteOnly);
	outstream<<SP_RoominfoUpdate;
	outstream<<m_roomMng->getRoomInfo(roomId);
	sendData(JSnakeServer::broadcastCenter(),outdata);
}

void JSnakeServerProcessor::sendRoominfoAdd(JID roomId)
{
	using namespace SnakeProtocol;
	QByteArray outdata;
	QDataStream outstream(&outdata,QIODevice::WriteOnly);
	outstream<<SP_RoominfoAdd;
	outstream<<m_roomMng->getRoomInfo(roomId);
	sendData(JSnakeServer::broadcastCenter(),outdata);
}

void JSnakeServerProcessor::sendRoominfoDelete(JID roomId)
{
	using namespace SnakeProtocol;
	QByteArray outdata;
	QDataStream outstream(&outdata,QIODevice::WriteOnly);
	outstream<<SP_RoominfoDelete;
	outstream<<roomId;
	sendData(JSnakeServer::broadcastCenter(),outdata);
}

void JSnakeServerProcessor::sendRoomEnter(JID roomId,JID userId)
{
	using namespace SnakeProtocol;
	QByteArray outdata;
	QDataStream outstream(&outdata,QIODevice::WriteOnly);
	outstream<<SP_RoomEnter;
	outstream<<roomId;
	outstream<<userId;
	sendData(JSnakeServer::broadcastCenter(),outdata);
}

void JSnakeServerProcessor::sendRoomEscape(JID roomId,JID userId)
{
	using namespace SnakeProtocol;
	QByteArray outdata;
	QDataStream outstream(&outdata,QIODevice::WriteOnly);
	outstream<<SP_RoomEscape;
	outstream<<roomId;
	outstream<<userId;
	sendData(JSnakeServer::broadcastCenter(),outdata);
}

void JSnakeServerProcessor::sendGameAct_getReady(JSocket* socket , bool ready,int num)
{
	using namespace SnakeProtocol;
	QByteArray outdata;
	QDataStream outstream(&outdata,QIODevice::WriteOnly);
	outstream<<SP_GA_Ready;
	outstream<<ready;
	outstream<<num;
	sendData(socket,outdata);
}

void JSnakeServerProcessor::sendGameAct_countDown(JSocket* socket , int sec)
{
	using namespace SnakeProtocol;
	QByteArray outdata;
	QDataStream outstream(&outdata,QIODevice::WriteOnly);
	outstream<<SP_GA_CountDown;
	outstream<<sec;
	sendData(socket,outdata);
}

void JSnakeServerProcessor::sendGameAct_getCommand(JSocket* socket)
{
	using namespace SnakeProtocol;
	QByteArray outdata;
	QDataStream outstream(&outdata,QIODevice::WriteOnly);
	outstream<<SP_GA_GetCommand;
	sendData(socket,outdata);
}

void JSnakeServerProcessor::sendGameAct_turn(JSocket* socket , JSnake::EDire dire,int num)
{
	using namespace SnakeProtocol;
	QByteArray outdata;
	QDataStream outstream(&outdata,QIODevice::WriteOnly);
	outstream<<SP_GA_Turn;
	outstream<<(qint16)dire;
	outstream<<num;
	sendData(socket,outdata);
}

void JSnakeServerProcessor::sendGameAct_collision(JSocket* socket , int num)
{
	using namespace SnakeProtocol;
	QByteArray outdata;
	QDataStream outstream(&outdata,QIODevice::WriteOnly);
	outstream<<SP_GA_Collision;
	outstream<<num;
	sendData(socket,outdata);
}

void JSnakeServerProcessor::sendGameAct_createBean(JSocket* socket , const QPoint& pt)
{
	using namespace SnakeProtocol;
	QByteArray outdata;
	QDataStream outstream(&outdata,QIODevice::WriteOnly);
	outstream<<SP_GA_CreateBean;
	outstream<<pt;
	sendData(socket,outdata);
}

void JSnakeServerProcessor::sendGameAct_increase(JSocket* socket , int num)
{
	using namespace SnakeProtocol;
	QByteArray outdata;
	QDataStream outstream(&outdata,QIODevice::WriteOnly);
	outstream<<SP_GA_Increase;
	outstream<<num;
	sendData(socket,outdata);
}

void JSnakeServerProcessor::sendGameAct_moveOn(JSocket* socket , int num)
{
	using namespace SnakeProtocol;
	QByteArray outdata;
	QDataStream outstream(&outdata,QIODevice::WriteOnly);
	outstream<<SP_GA_MoveOn;
	outstream<<num;
	sendData(socket,outdata);
}

void JSnakeServerProcessor::sendGameAct_Stop(JSocket* socket)
{
	using namespace SnakeProtocol;
	QByteArray outdata;
	QDataStream outstream(&outdata,QIODevice::WriteOnly);
	outstream<<SP_GA_Stop;
	sendData(socket,outdata);
}

void JSnakeServerProcessor::processEnterRoom(JSocket* socket , JID roomId)
{
	JID userId=socket->session()->userId();
	qDebug()<<__FUNCTION__<<__LINE__<<"userId:"<<userId<<"roomId:"<<roomId;
	JUserlistManager ulm;
	JID formerRoomId=ulm.getRoomByUser(userId);
	if(formerRoomId!=0) return;
	if(0==m_roomMng->enterRoom(roomId,userId))
	{
		JCode mur = ulm.moveUser(userId,roomId);
		qDebug()<<__FUNCTION__<<__LINE__<<"move user result:"<<mur;
		Q_ASSERT(0==mur);
		
		JSnakeServerRoomSocketManager *ssrsm=JSnakeServerRoomSocketManager::instance();
		ssrsm->enterRoom(roomId,socket);
		
		JSnakeGameOnServer *game=m_roomMng->getGame(roomId);
		sendUserlist(socket);
		for(int i=0;i<NUM_SNAKE;++i)
		{
			if(game->isReady(i))
			{
				this->sendGameAct_getReady(socket,game->isReady(i),i);
			}
		}
	}
}

void JSnakeServerProcessor::processEscapeRoom(JSocket* socket)
{
	JID userId=socket->session()->userId();
	JUserlistManager ulm;
	JID formerRoomId=ulm.getRoomByUser(userId);
	if(formerRoomId<0) return;
	
	JCode rmu = ulm.moveUser(userId,0);
	qDebug()<<__FUNCTION__<<__LINE__<<"move user result:"<<rmu;
	Q_ASSERT(0==rmn);
	
	JCode rer = m_roomMng->escapeRoom(formerRoomId,userId);
	qDebug()<<__FUNCTION__<<__LINE__<<"escape room result:"<<rer;
	Q_ASSERT(0==rer);
	
	JSnakeServerRoomSocketManager *ssrsm=JSnakeServerRoomSocketManager::instance();
	ssrsm->escapeRoom(formerRoomId,socket);
}
