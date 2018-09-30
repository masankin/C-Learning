package org.hawk.game.protocol;

public class ErrorCode {
	//登陆模块错误码[100-150]
	public static final class LoginErr{
		//用户名非法
		public static final int NAME_INVALID = 100;
		//用户名错误
		public static final int NAME_ERROR = 101;
		//密码错误
		public static final int PWD_ERROR = 102;
	};
}
