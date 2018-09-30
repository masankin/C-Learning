package org.hawk.game.protocol;

public interface ProtocolId {
	//错误通知
	public static final int ERR_NOTIFY = 100;
	//消息通知
	public static final int MSG_NOTIFY = 101;
	//时间同步
	public static final int TIME_SYNC = 102;
	//登入游戏
	public static final int LOGIN_GAME = 200;
	//踢出游戏
	public static final int KICKOUT_GAME = 201;
	//玩家初始化
	public static final int PLAYER_INIT = 202;
	//同步数据完成
	public static final int PLAYER_ASSEMBLY = 203;
};
