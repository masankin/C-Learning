#ifndef _TABLE_H_
#define _TABLE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ev++.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <errno.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include <json/json.h>

#include <map>
#include <set>

#include "bull_game_logic.h"
#include "global_define.h"

#include "jpacket.h"


///������׶�ʱ��
const int READY_TIME     = 5;    // ׼��ʱ��
const int BANKER_TIME    = 2;    // ѡׯʱ��
const int GROUP_TIME     = 15;   // ����ʱ��
const int COMPARE_TIME   = 5;    // ����ʱ��    


const int GROUP_TIME_EXTRA = 3;  // �û�û����ʱ�ĸ���ʱ��


class Player;
class Client;

//��λ
typedef struct
{
	int seatid;                //��λID
	int uid;                   //�û�id

	enChairStatus status;      //��λ״̬ (��λ,�Թ�,��Ϸ��)	
	enRole role;               //��ɫ 0:�м�, 1:ׯ��	
	Player *player;            //��� 


	unsigned long long  goup_time; //��������ʱ��
	int group_time_diff;           //��ׯ�ҵ�����ʱ���ֵ

	_uint8	m_cbUserHandCard[MAX_HAND_CARD_COUNT]; //���������

	CT_BULL_TYPE m_cardType;                          //���� 
	_uint8	     m_cbChoiceCard[MAX_HAND_CARD_COUNT]; //ѡ�����
	enEndKind    end_kind;                            //�������ͣ���Ӯ��
	bool         bPlayerChoiced;                            //�Ƿ�ѡ��

	///�����λ�ϵ����
	void clearSeat(void)
	{
		uid = 0;              // �û�id
		player = NULL;
		status = STATUS_NULL; // ��ʼΪ��λ 
		role   = ROLE_NULL;   // ��ɫ���Ƿ�ׯ�ң�

		resetSeat();
	}

	///��λ����ұ�����
	void resetSeat(void)
	{
		goup_time = GROUP_TIME + GROUP_TIME_EXTRA; //�������ʱ��
		group_time_diff = 0;

		memset(m_cbUserHandCard, 0, sizeof(m_cbUserHandCard));
		memset(m_cbChoiceCard, 0, sizeof(m_cbChoiceCard));

		m_cardType = CT_NOBULL;
		end_kind   = KIND_NULL;
		bPlayerChoiced = false;   ///�����ûѡ��
	}

} Seat;


///��Ϸ״̬����Ϸ������
typedef enum
{
	READY = 0,     ///׼��
	BANKER,        ///ѡׯ����Ϸ��ʼ��
	SEND_CARD,     ///����
	COMPARE,       ///���ƣ���Ϸ������
	WAIT,          ///�ȴ�
} EnGameState;


///������������Ϊ��λgame��
class Table
{
public:
	int					   tid;          ///����ID
	int             	   vid;          ///����ID
	int             	   min_money;    ///���Я��
	int					   max_money;    ///���Я��
	int					   base_money;   ///��ע���мҽ���=��ǰ����׷֡�Ӯ�����ͱ�����ʱ��ӳ�   ׯ�ҽ���=-���мҽ��㣩
	int                    fee;          ///��ˮ��ϵͳ��ÿ����ҿ�˰��
	int                    lose_exp;     ///��ľ���ֵ
	int                    win_exp;      ///Ӯ�ľ���ֵ
	EnGameState			   game_state;   // game state(����Ϸ����)

	int					   dealer;         //ׯ����λID
	std::map<int, Player*> playersMap;     //���map(uid->Player)
	int             	   cur_players;    //��ǰ��Ҹ���
	int					   ready_players;

	Seat                   seats[MAX_CHAIR_COUNT]; // ��λ����(һ������5��λ��)

	unsigned long long     start_group_time;  //���ƿ�ʼʱ��	

	unsigned long long     start_ready_time;   //׼����ʼʱ��	
	unsigned long long     start_banker_time;  //ѡׯ��ʼʱ��	
	unsigned long long     start_compare_time; //���ƿ�ʼʱ��	



	CBullGameLogic		   *m_GameLogic;	  //��Ϸ�߼�

	///1��׼������ʱ��
	ev_timer               ready_timer;          
	ev_tstamp              ready_timer_stamp;

	///2�����ׯ�ҡ���ʱ��
	ev_timer               banker_timer;         
	ev_tstamp              banker_timer_stamp;   

	///3�����ơ���ʱ��
	ev_timer               send_card_timer;      
	ev_tstamp              send_card_timer_stamp;   

	///4�����ơ���ʱ��
	ev_timer               compare_timer;         
	ev_tstamp              compare_timer_stamp;

	///�����˶�ʱ��
	ev_timer                robot_timer;         
	ev_tstamp               robot_timer_stamp;

public:
	Table();
	virtual ~Table();

	///��ʼ��
	int init(int my_tid, 
		int my_vid, 
		int my_min_money,
		int my_max_money, 
		int my_base_money, 
		int my_fee, 
		int my_lose_exp, 
		int my_win_exp);

	///�㲥
	int broadcast(Player *player, const std::string &packet);
	///����
	int unicast(Player *player, const std::string &packet);
	///���������
	int random(int start, int end);

//private:
	///�û���¼
	int handler_login(Player *player);
	///���е�¼�ɹ�Ӧ��
	int send_login_succ_uc(Player *player);
	///���е�¼�ɹ��㲥�����ҡ�����Ϣ�㲥��ȥ��
	int send_login_succ_bc(Player *player);
	///���з�������
	int send_table_info_uc(Player *player);

	///�û�����
	int sit_down(Player *player);
	///�û��뿪
	void stand_up(Player *player);

	///ɾ���û�
	int del_player(Player *player);


	// 1�㲥��Ϸ׼��
	int send_game_ready_bc();
	// 2-1���ѡׯ
	int game_start();
	///ȷ��ׯ��
	int make_banker();
	// 2-2��˰����ˮ��
	int pay_tax();
	// 2-3�㲥���ѡׯ
	int send_make_banker_bc();
	// 3����
	void send_card();
	// 4����
	int compare_card();
	// 5Ԥ������
	int pre_ready();

	/// ���й㲥�������������Ϣ
	int send_all_player_info_bc();

	// 1��׼����ʱ�����
	static void ready_timer_cb(struct ev_loop *loop, struct ev_timer *w, int revents);
	// 2�����ѡׯ��ʱ�����
	static void banker_timer_cb(struct ev_loop *loop, struct ev_timer *w, int revents);
	// 3�����ơ�ʱ�����
	static void send_card_timer_cb(struct ev_loop *loop, struct ev_timer *w, int revents);
	// 4�����ơ�ʱ�����
	static void compare_timer_cb(struct ev_loop *loop, struct ev_timer *w, int revents);
	// 6���ȴ���ʱ�����
	static void wait_timer_cb(struct ev_loop *loop, struct ev_timer *w, int revents);



	///4-1�û���������
	int handler_group_card(Player *player);

	void compare_timeout();


	///��Ϸ����    
	int game_end();

	int handler_logout(Player *player);

	///������
	int handler_chat(Player *player);
	///������
	int handler_face(Player *player);


	static void robot_timer_cb(struct ev_loop *loop, struct ev_timer *w, int revents);
	void lose_exp_update(Player *player);
	void win_exp_update(Player *player);

	///�ѵ��������Ϣ�㲥��ȥ
	int handler_info(Player *player);

	int handler_interaction_emotion(Player *player); // cfc add 20140310

	///�������״̬
	int setPlayStatus(int bankSeatid);

	//������ׯ�ҵ�����ʱ���ֵ
	int CalGroupTimeDiff();

	//����
	void group_cards(Seat &seat);

	///��һ��ׯ��
	int next_banker();

	//�Ƿ�����������ƶ�������
	bool isAllPlayerChoiced();
};

#endif