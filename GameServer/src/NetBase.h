#ifndef _NETBASE_H_
#define _NETBASE_H_

//======================================================
/// 消息头
//======================================================
typedef struct tagPACKET_HEADER
{
	unsigned short	size;		/// 消息长度
} PACKET_HEADER;

//======================================================
/// 消息部分
//======================================================
struct tagMsg
{
	unsigned short size;
	unsigned int hdCode;
};

// 消息类开头
#define  CMD_START(MSGID) \
struct tag##MSGID : public tagMsg \
{\
	tag##MSGID(){hdCode=MSGID;size = sizeof(tag##MSGID);}

// 消息类结束
#define  CMD_END \
};

enum hdnccodes
{
	NC_PlayerAuthSession = 0,
	NC_Type_Nums
};

#pragma pack(push, 1) 

CMD_START(NC_PlayerAuthSession)
	unsigned int playerID;
CMD_END

#pragma pack(pop)
#endif