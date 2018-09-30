#ifndef _NETBASE_H_
#define _NETBASE_H_

//======================================================
/// ��Ϣͷ
//======================================================
typedef struct tagPACKET_HEADER
{
	unsigned short	size;		/// ��Ϣ����
} PACKET_HEADER;

//======================================================
/// ��Ϣ����
//======================================================
struct tagMsg
{
	unsigned short size;
	unsigned int hdCode;
};

// ��Ϣ�࿪ͷ
#define  CMD_START(MSGID) \
struct tag##MSGID : public tagMsg \
{\
	tag##MSGID(){hdCode=MSGID;size = sizeof(tag##MSGID);}

// ��Ϣ�����
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