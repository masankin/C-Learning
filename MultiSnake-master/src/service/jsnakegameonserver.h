#ifndef JSNAKEGAMEONSERVER_H
#define JSNAKEGAMEONSERVER_H

#include <QObject>

#include <Global/Global>

#include "jsnakegame.h"

class JSnakeGame;
class QTimer;

class JSnakeGameOnServer : public QObject
{
    Q_OBJECT
public:
	explicit JSnakeGameOnServer(JID roomId,QObject *parent = 0);
	virtual ~JSnakeGameOnServer();
	void enter(int num);
	void escape(int num);
	void ready(bool ready,int num);
	void start(int msec);
	void setTurn(JSnake::EDire dire,int num);
	void stop();
	bool isReady(int num)const;
private:
	const JID m_roomId;
	JSnakeGame* m_game;
	QTimer* m_timer;
	int m_interval_msec;
	int m_countDown;
	JSnake::EDire m_dires[NUM_SNAKE];
	bool m_sit[NUM_SNAKE];
	bool m_ready[NUM_SNAKE];
	bool m_hasStarted;
private:
	void getReady(bool ready,int num);
	void countDown(int sec);
	void getCommand();
	void turn(JSnake::EDire dire,int num);
	void collision(int num);
	void createBean(const QPoint& pt);
	void increase(int num);
	void moveOn(int num);
	void getStop();
private slots:
	void on_timer_timeout();
protected:
	bool canStart();
	bool canStop();
	void reset();
};

#endif // JSNAKEGAMEONSERVER_H
