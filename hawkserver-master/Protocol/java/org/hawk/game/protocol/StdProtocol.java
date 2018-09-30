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

public class StdProtocol {
	//错误通知
	public static class ErrNotify extends HawkProtocol {
		public ErrNotify() {
			super(ProtocolId.ERR_NOTIFY, DriveType.DRIVE_EVENT, "");
			this.m_iErrId = 0;
			this.m_iMode = 0;
		};
	
		public HawkMarshal clone() {
			return new ErrNotify();
		}
	
		public void marshal(HawkOctetsStream os) {
			os.writeShort(m_iErrId);
			os.writeByte(m_iMode);
		}
	
		public void unmarshal(HawkOctetsStream os) throws HawkException {
			m_iErrId = os.readShort();
			m_iMode = os.readByte();
		}
	
		public boolean clear() {
			this.m_iErrId= 0;
			this.m_iMode= 0;
			return true;
		};
	
		public Short m_iErrId;
		public Byte m_iMode;
	};

	//消息通知
	public static class MsgNotify extends HawkProtocol {
		public MsgNotify() {
			super(ProtocolId.MSG_NOTIFY, DriveType.DRIVE_EVENT, "");
			this.m_sMsg = new String();
			this.m_iMode = 0;
		};
	
		public HawkMarshal clone() {
			return new MsgNotify();
		}
	
		public void marshal(HawkOctetsStream os) {
			os.writeString(m_sMsg);
			os.writeByte(m_iMode);
		}
	
		public void unmarshal(HawkOctetsStream os) throws HawkException {
			m_sMsg = os.readString();
			m_iMode = os.readByte();
		}
	
		public boolean clear() {
			this.m_sMsg = "";
			this.m_iMode= 0;
			return true;
		};
	
		public String m_sMsg;
		public Byte m_iMode;
	};

	//时间同步
	public static class TimeSync extends HawkProtocol {
		public TimeSync() {
			super(ProtocolId.TIME_SYNC, DriveType.DRIVE_EVENT, "");
			this.m_iTime = 0;
		};
	
		public HawkMarshal clone() {
			return new TimeSync();
		}
	
		public void marshal(HawkOctetsStream os) {
			os.writeInt(m_iTime);
		}
	
		public void unmarshal(HawkOctetsStream os) throws HawkException {
			m_iTime = os.readInt();
		}
	
		public boolean clear() {
			this.m_iTime= 0;
			return true;
		};
	
		public Integer m_iTime;
	};

}
