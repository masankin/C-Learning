package org.hawk.game.protocol;

import org.hawk.protocol.HawkProtocolManager;

public class ProtocolReg {

	public static void init() {
		HawkProtocolManager.getInstance().register(ProtocolId.LOGIN_GAME,new GsProtocol.LoginGame());
		HawkProtocolManager.getInstance().register(ProtocolId.KICKOUT_GAME,new GsProtocol.KickoutGame());
		HawkProtocolManager.getInstance().register(ProtocolId.PLAYER_INIT,new GsProtocol.PlayerInit());
		HawkProtocolManager.getInstance().register(ProtocolId.PLAYER_ASSEMBLY,new GsProtocol.PlayerAssembly());
		HawkProtocolManager.getInstance().register(ProtocolId.ERR_NOTIFY,new StdProtocol.ErrNotify());
		HawkProtocolManager.getInstance().register(ProtocolId.MSG_NOTIFY,new StdProtocol.MsgNotify());
		HawkProtocolManager.getInstance().register(ProtocolId.TIME_SYNC,new StdProtocol.TimeSync());
	}
}
