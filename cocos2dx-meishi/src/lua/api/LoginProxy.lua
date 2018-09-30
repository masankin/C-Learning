
--------------------------------
-- @module LoginProxy
-- @extend Ref,BaseProxy
-- @parent_module 

--------------------------------
-- 
-- @function [parent=#LoginProxy] currSignalNum 
-- @param self
-- @return LoginProxy#LoginProxy self (return value: LoginProxy)
        
--------------------------------
-- 
-- @function [parent=#LoginProxy] setReloginShowing 
-- @param self
-- @param #bool var
-- @return LoginProxy#LoginProxy self (return value: LoginProxy)
        
--------------------------------
-- 
-- @function [parent=#LoginProxy] setGameServerName 
-- @param self
-- @param #string var
-- @return LoginProxy#LoginProxy self (return value: LoginProxy)
        
--------------------------------
-- 
-- @function [parent=#LoginProxy] setLoginSuccess 
-- @param self
-- @param #bool var
-- @return LoginProxy#LoginProxy self (return value: LoginProxy)
        
--------------------------------
-- 
-- @function [parent=#LoginProxy] setRetry 
-- @param self
-- @param #int var
-- @return LoginProxy#LoginProxy self (return value: LoginProxy)
        
--------------------------------
-- 
-- @function [parent=#LoginProxy] updateRetryEnterServer 
-- @param self
-- @param #float dt
-- @return LoginProxy#LoginProxy self (return value: LoginProxy)
        
--------------------------------
-- 
-- @function [parent=#LoginProxy] getIsAccepted 
-- @param self
-- @return bool#bool ret (return value: bool)
        
--------------------------------
-- 
-- @function [parent=#LoginProxy] getLoginSuccess 
-- @param self
-- @return bool#bool ret (return value: bool)
        
--------------------------------
-- 
-- @function [parent=#LoginProxy] setSendStart 
-- @param self
-- @param #unsigned long long var
-- @return LoginProxy#LoginProxy self (return value: LoginProxy)
        
--------------------------------
-- 
-- @function [parent=#LoginProxy] setConnTimes 
-- @param self
-- @param #int var
-- @return LoginProxy#LoginProxy self (return value: LoginProxy)
        
--------------------------------
-- 
-- @function [parent=#LoginProxy] setGameServer 
-- @param self
-- @param #string var
-- @return LoginProxy#LoginProxy self (return value: LoginProxy)
        
--------------------------------
-- 
-- @function [parent=#LoginProxy] getCurrSignalMs 
-- @param self
-- @return int#int ret (return value: int)
        
--------------------------------
-- 
-- @function [parent=#LoginProxy] sendRegisterRqst 
-- @param self
-- @return LoginProxy#LoginProxy self (return value: LoginProxy)
        
--------------------------------
-- 
-- @function [parent=#LoginProxy] getReloginShowing 
-- @param self
-- @return bool#bool ret (return value: bool)
        
--------------------------------
-- 
-- @function [parent=#LoginProxy] setCurrSignalMs 
-- @param self
-- @param #int var
-- @return LoginProxy#LoginProxy self (return value: LoginProxy)
        
--------------------------------
-- 
-- @function [parent=#LoginProxy] sendEnterServerRqst 
-- @param self
-- @return LoginProxy#LoginProxy self (return value: LoginProxy)
        
--------------------------------
-- 
-- @function [parent=#LoginProxy] sendHeartBeat 
-- @param self
-- @return LoginProxy#LoginProxy self (return value: LoginProxy)
        
--------------------------------
-- 
-- @function [parent=#LoginProxy] onRetryEnterServer 
-- @param self
-- @return LoginProxy#LoginProxy self (return value: LoginProxy)
        
--------------------------------
-- 
-- @function [parent=#LoginProxy] sendPKeyRest 
-- @param self
-- @return LoginProxy#LoginProxy self (return value: LoginProxy)
        
--------------------------------
-- 
-- @function [parent=#LoginProxy] getCurrSignal 
-- @param self
-- @return int#int ret (return value: int)
        
--------------------------------
-- 
-- @function [parent=#LoginProxy] isLogin 
-- @param self
-- @return LoginProxy#LoginProxy self (return value: LoginProxy)
        
--------------------------------
-- 
-- @function [parent=#LoginProxy] loginSuccess 
-- @param self
-- @return LoginProxy#LoginProxy self (return value: LoginProxy)
        
--------------------------------
-- 
-- @function [parent=#LoginProxy] getGameServer 
-- @param self
-- @return string#string ret (return value: string)
        
--------------------------------
-- 
-- @function [parent=#LoginProxy] setRetryLoginServer 
-- @param self
-- @param #int var
-- @return LoginProxy#LoginProxy self (return value: LoginProxy)
        
--------------------------------
-- 
-- @function [parent=#LoginProxy] getIsUpdateResource 
-- @param self
-- @return bool#bool ret (return value: bool)
        
--------------------------------
-- 
-- @function [parent=#LoginProxy] getGameServerName 
-- @param self
-- @return string#string ret (return value: string)
        
--------------------------------
-- 
-- @function [parent=#LoginProxy] requestLoginKey 
-- @param self
-- @return LoginProxy#LoginProxy self (return value: LoginProxy)
        
--------------------------------
-- 
-- @function [parent=#LoginProxy] onCallBack 
-- @param self
-- @param #cc.Ref ref
-- @return LoginProxy#LoginProxy self (return value: LoginProxy)
        
--------------------------------
-- 
-- @function [parent=#LoginProxy] getRetry 
-- @param self
-- @return int#int ret (return value: int)
        
--------------------------------
-- 
-- @function [parent=#LoginProxy] setServerTime 
-- @param self
-- @param #unsigned long long var
-- @return LoginProxy#LoginProxy self (return value: LoginProxy)
        
--------------------------------
-- 
-- @function [parent=#LoginProxy] doResLoadOver 
-- @param self
-- @return LoginProxy#LoginProxy self (return value: LoginProxy)
        
--------------------------------
-- 
-- @function [parent=#LoginProxy] sendLogOut 
-- @param self
-- @return LoginProxy#LoginProxy self (return value: LoginProxy)
        
--------------------------------
-- 
-- @function [parent=#LoginProxy] sendOtherRqst 
-- @param self
-- @return LoginProxy#LoginProxy self (return value: LoginProxy)
        
--------------------------------
-- 
-- @function [parent=#LoginProxy] sendClientKey 
-- @param self
-- @return LoginProxy#LoginProxy self (return value: LoginProxy)
        
--------------------------------
-- 
-- @function [parent=#LoginProxy] getSendStart 
-- @param self
-- @return unsigned long long#unsigned long long ret (return value: unsigned long long)
        
--------------------------------
-- 
-- @function [parent=#LoginProxy] setIsAccepted 
-- @param self
-- @param #bool var
-- @return LoginProxy#LoginProxy self (return value: LoginProxy)
        
--------------------------------
-- 
-- @function [parent=#LoginProxy] getServerTime 
-- @param self
-- @return unsigned long long#unsigned long long ret (return value: unsigned long long)
        
--------------------------------
-- 
-- @function [parent=#LoginProxy] getConnTimes 
-- @param self
-- @return int#int ret (return value: int)
        
--------------------------------
-- 
-- @function [parent=#LoginProxy] setIsUpdateResource 
-- @param self
-- @param #bool var
-- @return LoginProxy#LoginProxy self (return value: LoginProxy)
        
--------------------------------
-- 
-- @function [parent=#LoginProxy] sendOther2Rqst 
-- @param self
-- @return LoginProxy#LoginProxy self (return value: LoginProxy)
        
--------------------------------
-- 
-- @function [parent=#LoginProxy] checkLogin 
-- @param self
-- @return bool#bool ret (return value: bool)
        
--------------------------------
-- 
-- @function [parent=#LoginProxy] sendGetServerRqst 
-- @param self
-- @return LoginProxy#LoginProxy self (return value: LoginProxy)
        
--------------------------------
-- 
-- @function [parent=#LoginProxy] sendDirectEnterRqst 
-- @param self
-- @return LoginProxy#LoginProxy self (return value: LoginProxy)
        
--------------------------------
-- 
-- @function [parent=#LoginProxy] InitNetConnection 
-- @param self
-- @param #string addrs
-- @return LoginProxy#LoginProxy self (return value: LoginProxy)
        
--------------------------------
-- 
-- @function [parent=#LoginProxy] getRetryLoginServer 
-- @param self
-- @return int#int ret (return value: int)
        
--------------------------------
-- 
-- @function [parent=#LoginProxy] analysisLoginDNS 
-- @param self
-- @return LoginProxy#LoginProxy self (return value: LoginProxy)
        
--------------------------------
-- 
-- @function [parent=#LoginProxy] updateHearBeat 
-- @param self
-- @param #float dt
-- @return LoginProxy#LoginProxy self (return value: LoginProxy)
        
--------------------------------
-- 
-- @function [parent=#LoginProxy] LoginProxy 
-- @param self
-- @return LoginProxy#LoginProxy self (return value: LoginProxy)
        
return nil
