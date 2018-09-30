#include "HsGame.h"
#include "HsPlayer.h"

namespace Hawk
{
	HsGame* g_Game = 0;

	HsGame::HsGame() : HawkAppFrame(XID(HSO_MANAGER, HMI_APP))
	{
		HawkAssert(g_Game == 0);
		g_Game = this;

		#include "ProtocolReg.inl"
	}

	HsGame::~HsGame()
	{
		g_Game = 0;
	}

	Bool HsGame::Init(const AString& sCfgFile)
	{
		//��ʼ�������ļ���ȡ
		AppCfg     sAppCfg;
		HawkKVFile cfgFile;
		AString    sValue = "";
		if (cfgFile.Open(sCfgFile, HawkFile::OPEN_READ))
		{			
			//��ʼ����Ϸȫ�ֶ���
			if(!InitGameObj())
			{
				HawkPrint("GameServer Init GameObj Error.");
				return false;
			}
			
			//������id
			if (cfgFile.QueryValue("SvrId", sValue))
			{
				sAppCfg.SvrId = HawkStringUtil::StringToUInt<AString>(sValue);
				HawkFmtPrint("HsGame Init, SvrId: %s", sValue.c_str());
			}
			else
			{
				HawkFmtPrint("HsGame Init Error, SvrId: %s", sValue.c_str());
				return false;
			}

			//�߼��߳���
			if (cfgFile.QueryValue("ThreadNum", sValue))
			{
				sAppCfg.Threads = HawkStringUtil::StringToUInt<AString>(sValue);
				HawkFmtPrint("HsGame Init, ThreadNum: %s", sValue.c_str());
			}
			else
			{
				HawkFmtPrint("HsGame Init Error, ThreadNum: %s", sValue.c_str());
				return false;
			}

			//�Ƿ�������̨��ӡ
			if (cfgFile.QueryValue("Console", sValue))
			{
				sAppCfg.Console = HawkStringUtil::StringToBool<AString>(sValue);
				HawkFmtPrint("HsGame Init, Console: %s", sValue.c_str());
			}

			//���ط�������ַ
			if (cfgFile.QueryValue("GateAddr", sValue))
			{
				sAppCfg.Gateproxy.Address = sValue;
				HawkStringUtil::Replace<AString>(sAppCfg.Gateproxy.Address, "*", "127.0.0.1");
				HawkFmtPrint("HsGame Init, GateAddr: %s", sValue.c_str());
			}
			else
			{
				HawkFmtPrint("HsGame Init Error, GateAddr: %s", sValue.c_str());
				return false;
			}

			//����������
			if (cfgFile.QueryValue("ConnLimit", sValue))
			{
				sAppCfg.Gateproxy.ConnLimit = HawkStringUtil::StringToUInt<AString>(sValue);
				HawkFmtPrint("HsGame Init, ConnLimit: %s", sValue.c_str());
			}			

			
			//��־��������ַ
			if (cfgFile.QueryValue("LogAddr", sValue))
			{
				sAppCfg.LogAddr = sValue;
				HawkFmtPrint("HsGame Init, LogAddr: %s", sValue.c_str());
			}

			//������������ģʽ
			if (cfgFile.QueryValue("InnerGate", sValue) && HawkStringUtil::StringToBool<AString>(sValue) == true)
			{
				cfgFile.QueryValue("GateCfg",	 sAppCfg.Gateway.GwCfg);
				cfgFile.QueryValue("GwSecurity", sAppCfg.Gateway.GwSecurity);

				HawkFmtPrint("HsGame Init, GateCfg: %s",    sAppCfg.Gateway.GwCfg.c_str());
				HawkFmtPrint("HsGame Init, GwSecurity: %s", sAppCfg.Gateway.GwSecurity.c_str());
			}
			
			//��ʼ�����ݿ�����
			if (cfgFile.QueryValue("DbEnable", sValue))
			{
				HawkFmtPrint("HsGame Init, DbEnable: %s", sValue.c_str());
				if (HawkStringUtil::StringToBool(sValue) == true)
				{
					//host
					cfgFile.QueryValue("DbHost", sValue);
					sAppCfg.DBConn.SetHost(sValue);
					HawkFmtPrint("HsGame Init, DbHost: %s", sValue.c_str());

					//type
					cfgFile.QueryValue("DbType", sValue);
					sAppCfg.DBConn.SetType(HawkStringUtil::StringToUInt<AString>(sValue));
					HawkFmtPrint("HsGame Init, DbType: %s", sValue.c_str());

					//port
					cfgFile.QueryValue("DbPort", sValue);
					sAppCfg.DBConn.SetPort(HawkStringUtil::StringToInt(sValue));
					HawkFmtPrint("HsGame Init, DbPort: %s", sValue.c_str());

					//user
					cfgFile.QueryValue("DbUser", sValue);
					sAppCfg.DBConn.SetUser(sValue);
					HawkFmtPrint("HsGame Init, DbUser: %s", sValue.c_str());

					//pwd
					cfgFile.QueryValue("DbPwd", sValue);
					sAppCfg.DBConn.SetPwd(sValue);
					HawkFmtPrint("HsGame Init, DbPwd: %s", sValue.c_str());

					//db
					cfgFile.QueryValue("DbName", sValue);
					sAppCfg.DBConn.SetDBName(sValue);
					HawkFmtPrint("HsGame Init, DbName: %s", sValue.c_str());
				}
			}

			return HawkAppFrame::Init(sAppCfg);
		}
		return false;
	}

	Bool HsGame::Run()
	{
		return HawkAppFrame::Run();
	}

	Bool HsGame::OnTick(UInt32 iPeriod)
	{
		if (!HawkAppFrame::OnTick(iPeriod))
			return false;

		//��ʱ���¶���
		static UInt32 obj_tick_time = 0;
		if (!obj_tick_time || m_iTickTime - obj_tick_time >= HS_OBJ_TICK_PERIOD)
		{
			obj_tick_time = m_iTickTime;

			TickPlayers();
			TickManagers();
		}
		return true;
	}

	Bool HsGame::OnException(Int32 iCode)
	{
		static XIDVector vXID;

		//ϵͳ�쳣, ֪ͨ��ҽ��д洢
		ObjMan* pUserMan = GetObjMan(HSO_PLAYER);
		if (pUserMan)
		{
			vXID.clear();
			if (!pUserMan->CollectObjKey(vXID))
				return false;

			PostException(vXID, iCode);
		}

		//ϵͳ�쳣, ֪ͨ�������������д洢
		ObjMan* pManagerMan = GetObjMan(HSO_MANAGER);
		if (pManagerMan)
		{
			vXID.clear();
			if (!pManagerMan->CollectObjKey(vXID))
				return false;

			PostException(vXID, iCode);
		}

		return true;
	}

	Bool HsGame::TickPlayers()
	{
		ObjMan* pUserMan = GetObjMan(HSO_PLAYER);
		if (pUserMan)
		{
			static XIDVector vXID;
			vXID.clear();
			if (!pUserMan->CollectObjKey(vXID))
				return false;

			return PostTick(vXID);
		}
		return false;
	}

	Bool HsGame::TickManagers()
	{
		ObjMan* pManagerMan = GetObjMan(HSO_MANAGER);
		if (pManagerMan)
		{
			static XIDVector vXID;
			vXID.clear();
			if (!pManagerMan->CollectObjKey(vXID))
				return false;

			return PostTick(vXID);
		}
		return false;
	}

	Bool HsGame::Stop()
	{
		if (!HawkAppFrame::Stop())
			return false;

		return true;
	}

	Bool HsGame::OnMessage(const HawkMsg& sMsg)
	{
		//�����ͷ�
		if (sMsg.Msg == HSM_DELOBJ)
		{
			XID sXid = sMsg.Params.Get<XID>(0);
			DeleteObj(sXid);
			return true;
		}
		return false;
	}

	Bool HsGame::InitGameObj()
	{
		//ע����������
		CreateObjMan(HSO_MANAGER);		
		CreateObjMan(HSO_PLAYER);		
		
		//TODO: ������������
		return true;
	}

	HawkAppObj*	HsGame::AppCreateObj(const XID& sXid)
	{
		HawkAppObj* pObj = 0;

		if (sXid.Type == HSO_MANAGER)
		{
			//TODO: ������������
		}
		else if (sXid.Type == HSO_PLAYER)
		{
			pObj = new HsPlayer(sXid);
		}

		HawkAssert(pObj != 0);
		return pObj;
	}

	Bool HsGame::OnSessionStart(SID iSid, const AString& sAddr)
	{
		return HawkAppFrame::OnSessionStart(iSid, sAddr);
	}

	Bool HsGame::OnSessionProtocol(SID iSid, Protocol* pProto)
	{
		return HawkAppFrame::OnSessionProtocol(iSid, pProto);
	}

	Bool HsGame::PostProtocol(SID iSid, Protocol* pProto)
	{
		XID sXid;
		ProtoType iType = pProto->GetType();
		if (iType == ProtocolId::LOGIN_GAME)
		{
			UString sName = ((LoginGame*)pProto)->m_sName;

			HawkStringUtil::Trim<UString>(sName);
			UInt32 iPlayerId = HawkStringUtil::HashString<UString>(sName);

			SafeObj user_obj(XID(HSO_PLAYER, iPlayerId));
			if(!user_obj.IsObjValid())
				CreateObj(XID(HSO_PLAYER, iPlayerId));

			sXid = XID(HSO_PLAYER, iPlayerId);
		}
		else
		{
			sXid = GetXidBySid(iSid);
		}

		return HawkAppFrame::PostProtocol(sXid, iSid, pProto);
	}

	Bool HsGame::DispatchMsg(const XID& sXid, HawkMsg* pMsg)
	{
		if (sXid == XID(HSO_MANAGER, HMI_APP))
			return OnMessage(*pMsg);

		return HawkAppFrame::DispatchMsg(sXid, pMsg);
	}

	Bool HsGame::OnSessionClose(SID iSid, const AString& sAddr)
	{
		if (m_mSidAddr.Find(iSid))
		{
			//ע: �˴��п�����δ�󶨶���SID֮ǰ���������Ϣ©��
			//��: Э��ͨ���̳߳ش���, �п����ڰ�Э��Ͷ�ݶ�δ����ʱ�����˴�����
			XID sXid = GetXidBySid(iSid);
			if (sXid.IsValid())
			{
				HawkMsg* pMsg = P_MsgManager->GetMsg(HSM_DISCONNECT);
				if (pMsg)
				{
					pMsg->Target = sXid;
					pMsg->Params.Push<SID>(iSid);
					PostMsg(pMsg);
				}
			}
		}

		return HawkAppFrame::OnSessionClose(iSid, sAddr);
	}
}
