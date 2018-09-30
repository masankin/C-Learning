package org.hawk.game.protocol;

import java.util.Map;
import java.util.List;
import java.util.HashMap;
import java.util.ArrayList;
import org.hawk.os.HawkException;
import org.hawk.octets.HawkOctets;
import org.hawk.protocol.HawkMarshal;
import org.hawk.protocol.HawkProtocol;
import org.hawk.octets.HawkOctetsStream;

@SuppressWarnings("unused")

public class GsProtocol {

	public static class PlayerInfo extends HawkMarshal
	{
		public PlayerInfo() {
			this.m_iPlayerId = 0;
			this.m_sNickName = new String();
			this.m_iGender = 0;
		};
	
		public HawkMarshal clone() {
			return new PlayerInfo();
		}
	
		public void marshal(HawkOctetsStream os) {
			os.writeInt(m_iPlayerId);
			os.writeString(m_sNickName);
			os.writeByte(m_iGender);
		}
	
		public void unmarshal(HawkOctetsStream os) throws HawkException {
			m_iPlayerId = os.readInt();
			m_sNickName = os.readString();
			m_iGender = os.readByte();
		}
	
		public boolean clear() {
			this.m_iPlayerId= 0;
			this.m_sNickName = "";
			this.m_iGender= 0;
			return true;
		};
	
		public Integer m_iPlayerId;
		public String m_sNickName;
		public Byte m_iGender;
	};

	//登入游戏
	public static class LoginGame extends HawkProtocol {
		public LoginGame() {
			super(ProtocolId.LOGIN_GAME, DriveType.DRIVE_EVENT, "");
			this.m_sName = new String();
			this.m_sPwd = new String();
			this.m_iFlag = 0;
		};
	
		public HawkMarshal clone() {
			return new LoginGame();
		}
	
		public void marshal(HawkOctetsStream os) {
			os.writeString(m_sName);
			os.writeString(m_sPwd);
			os.writeByte(m_iFlag);
		}
	
		public void unmarshal(HawkOctetsStream os) throws HawkException {
			m_sName = os.readString();
			m_sPwd = os.readString();
			m_iFlag = os.readByte();
		}
	
		public boolean clear() {
			this.m_sName = "";
			this.m_sPwd = "";
			this.m_iFlag= 0;
			return true;
		};
	
		public String m_sName;
		public String m_sPwd;
		public Byte m_iFlag;
	};

	//踢出游戏
	public static class KickoutGame extends HawkProtocol {
		public KickoutGame() {
			super(ProtocolId.KICKOUT_GAME, DriveType.DRIVE_EVENT, "");
			this.m_iPlayerId = 0;
			this.m_sName = new String();
		};
	
		public HawkMarshal clone() {
			return new KickoutGame();
		}
	
		public void marshal(HawkOctetsStream os) {
			os.writeInt(m_iPlayerId);
			os.writeString(m_sName);
		}
	
		public void unmarshal(HawkOctetsStream os) throws HawkException {
			m_iPlayerId = os.readInt();
			m_sName = os.readString();
		}
	
		public boolean clear() {
			this.m_iPlayerId= 0;
			this.m_sName = "";
			return true;
		};
	
		public Integer m_iPlayerId;
		public String m_sName;
	};

	//玩家初始化
	public static class PlayerInit extends HawkProtocol {
		public PlayerInit() {
			super(ProtocolId.PLAYER_INIT, DriveType.DRIVE_EVENT, "");
			this.m_sInfo = new PlayerInfo();
		};
	
		public HawkMarshal clone() {
			return new PlayerInit();
		}
	
		public void marshal(HawkOctetsStream os) {
			m_sInfo.marshal(os);
		}
	
		public void unmarshal(HawkOctetsStream os) throws HawkException {
			m_sInfo.unmarshal(os);
		}
	
		public boolean clear() {
			this.m_sInfo.clear();
			return true;
		};
	
		public PlayerInfo m_sInfo;
	};

	//同步数据完成
	public static class PlayerAssembly extends HawkProtocol {
		public PlayerAssembly() {
			super(ProtocolId.PLAYER_ASSEMBLY, DriveType.DRIVE_EVENT, "");
			this.m_iPlayerId = 0;
			this.m_iSvrId = 0;
		};
	
		public HawkMarshal clone() {
			return new PlayerAssembly();
		}
	
		public void marshal(HawkOctetsStream os) {
			os.writeInt(m_iPlayerId);
			os.writeInt(m_iSvrId);
		}
	
		public void unmarshal(HawkOctetsStream os) throws HawkException {
			m_iPlayerId = os.readInt();
			m_iSvrId = os.readInt();
		}
	
		public boolean clear() {
			this.m_iPlayerId= 0;
			this.m_iSvrId= 0;
			return true;
		};
	
		public Integer m_iPlayerId;
		public Integer m_iSvrId;
	};

}
