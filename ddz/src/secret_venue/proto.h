#ifndef __PROTO_H__
#define __PROTO_H__
///---Э��ָ���
enum system_command
{
    SYS_ECHO						= 0001,       /* echo */
    SYS_ONLINE						= 0002,       /* get online */
};
///---�ͻ�������ָ��
enum client_command
{
	CLIENT_LOGIN_REQ			= 1001,      /* join table */
	CLIENT_LOGOUT_REQ			= 1002,
	CLIENT_READY_REQ			= 1003,	     /* game ready */
	CLIENT_BET_REQ		 		= 1004,
	CLIENT_CHAT_REQ				= 1005,
	CLIENT_FACE_REQ				= 1006,
	CLIENT_CHANGE_REQ           = 1007,      /* change table */
	CLIENT_ROBOT_REQ            = 1008,
	CLIENT_INFO_REQ             = 1009,      /* update player info */
	CLIENT_TABLE_INFO_REQ       = 1010,      /* table info */
	CLIENT_EMOTION_REQ          = 1011,      /* interaction emotion */
	CLIENT_PROP_REQ             = 1012,      /* game prop */

	CLIENT_GROUP_CARD_REQ       = 1020,      /*�û�����������Ϣ*/

	///+++
	CLIENT_LOGIN_SECRET_VENU_REQ= 1100,      /* ��¼˽�ܳ� */
	CLIENT_JOIN_GAME_REQ        = 1101,      /* ������Ϸ */
	CLIENT_KICK_PLAYER_REQ      = 1102,      /*	��������*/
	CLIENT_DISMISS_ROOM_REQ     = 1103,      /*	��ɢ��������*/
};
///---���bet��action
enum client_action
{
	PLAYER_CALL 					= 2001,       /* call */
    PLAYER_RAISE	                = 2002,       /* raise */
	PLAYER_COMPARE		            = 2003,       /* compare */
	PLAYER_SEE						= 2004,		  /* see */
    PLAYER_FOLD	                    = 2005,       /* fold */
    PLAYER_ALLIN              = 2006,     /* all in */
    PLAYER_ALLIN_COMPARE      = 2007,     /* all in compare */
};
///---����ָ��
enum prop_item
{
	CHANGE_CARD = 3001,  /* change card */
	FORBIDDEN_CARD = 3002, /* forbidden compare card */
	DOUBLE_CARD_FOUR = 3003,    /* compare four multiple bet card */
	DOUBLE_CARD_SIX = 3004,    /* compare six multiple bet card */
	DOUBLE_CARD_EIGHT = 3005,    /* compare eight multiple bet card */
};
///---���������ָ��
enum server_command
{
	SERVER_LOGIN_SUCC_UC       		 = 4000,
    SERVER_LOGIN_SUCC_BC       		 = 4001,
    SERVER_LOGIN_ERR_UC         	 = 4002,
	SERVER_REBIND_UC				 = 4003,
	SERVER_LOGOUT_SUCC_BC			 = 4004,
	SERVER_LOGOUT_ERR_UC			 = 4005,
	SERVER_TABLE_INFO_UC			 = 4006,
	SERVER_READY_SUCC_BC			 = 4007,
	SERVER_READY_ERR_UC				 = 4008,
	SERVER_GAME_START_BC			 = 4009,
	SERVER_NEXT_BET_BC				 = 4010,
	SERVER_BET_SUCC_BC				 = 4011,
	SERVER_BET_SUCC_UC				 = 4012,
	SERVER_BET_ERR_UC			     = 4013,
	SERVER_GAME_END_BC				 = 4014,
	SERVER_GAME_PREREADY_BC			 = 4015,
	SERVER_CHAT_BC					 = 4016,
	SERVER_FACE_BC					 = 4017,
	SERVER_ROBOT_SER_UC          = 4018,
	SERVER_ROBOT_NEED_UC         = 4019,
	SERVER_UPDATE_INFO_BC        = 4020,
	SERVER_EMOTION_BC            = 4021,
	SERVER_PROP_SUCC_UC          = 4022,
	SERVER_PROP_SUCC_BC          = 4023,
	SERVER_PROP_ERR_UC           = 4024,

	SERVER_GROUP_CARD_BC         = 4026,  // �㲥����
	SERVER_GROUP_CARD_UC         = 4027,  // ����Ӧ��
	SERVER_GAME_WAIT_BC          = 4028,  // �㲥��Ϸ�ȴ�  
	SERVER_GAME_READY_BC         = 4029,  // �㲥��Ϸ׼��
	SERVER_MAKE_BANKER_BC        = 4030,  // �㲥���ׯ��
	
	SERVER_SEND_CARD_UC          = 4032,  // ����
	SERVER_COMPARE_CARD_BC       = 4033,  // �㲥����

	///+++
	SERVER_JOIN_GAME_UC          = 4100,  // ������ϷӦ��
	SERVER_JOIN_GAME_BC          = 4101,  // ������Ϸ�㲥
    SERVER_KICK_PLAYER_UC        = 4102,  // ����Ӧ��
    SERVER_KICK_PLAYER_BC        = 4103,  // ���˹㲥
	SERVER_DISMISS_ROOM_UC       = 4104,  // ��ɢ����Ӧ��
	SERVER_DISMISS_ROOM_BC       = 4105,  // ��ɢ����㲥
};

#endif