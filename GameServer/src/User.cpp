#include "User.h"
#include "Logger.h"
CodesHandler<User, tagMsg> user_table[NC_Type_Nums] =
{
	{"nc_playerAuthSession", nc_playerAuthSession, &User::nc_playerAuthSession}
};

void User::OnRecv( BYTE *pMsg, WORD wSize )
{
	tagMsg* m = (tagMsg*)pMsg;
	if(m->hdCode<NC_Type_Nums)
		(this->*(user_table[m->hdCode].handler))(m);
	else
	{
		Logger::get_instance().TraceError("code error%s", GetIP());
	}
}

void User::nc_playerAuthSession(tagMsg* m)
{
	tagnc_playerAuthSession* pMsg = (tagnc_playerAuthSession*)m;
	Logger::get_instance().TraceKeyInfo("playerAuthSession %d", pMsg->playerID);
}