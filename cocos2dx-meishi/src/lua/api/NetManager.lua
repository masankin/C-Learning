
--------------------------------
-- @module NetManager
-- @extend Ref
-- @parent_module 

--------------------------------
-- 
-- @function [parent=#NetManager] DisConnect 
-- @param self
-- @return NetManager#NetManager self (return value: NetManager)
        
--------------------------------
-- 
-- @function [parent=#NetManager] clearCachePack 
-- @param self
-- @return NetManager#NetManager self (return value: NetManager)
        
--------------------------------
-- 
-- @function [parent=#NetManager] SendCachePack 
-- @param self
-- @return NetManager#NetManager self (return value: NetManager)
        
--------------------------------
-- 
-- @function [parent=#NetManager] delCmdListener 
-- @param self
-- @param #int iCmd
-- @return NetManager#NetManager self (return value: NetManager)
        
--------------------------------
-- 
-- @function [parent=#NetManager] InitNetConnection 
-- @param self
-- @param #string strIP
-- @param #unsigned short uPort
-- @return int#int ret (return value: int)
        
--------------------------------
-- 
-- @function [parent=#NetManager] IsConnect 
-- @param self
-- @return bool#bool ret (return value: bool)
        
--------------------------------
-- 
-- @function [parent=#NetManager] getEncryMsg 
-- @param self
-- @return bool#bool ret (return value: bool)
        
--------------------------------
-- 
-- @function [parent=#NetManager] ReConnect 
-- @param self
-- @return NetManager#NetManager self (return value: NetManager)
        
--------------------------------
-- 
-- @function [parent=#NetManager] RegCmdListener 
-- @param self
-- @param #int iCmd
-- @param #BaseProxy pstProxy
-- @param #bool isUpdateFunc
-- @return int#int ret (return value: int)
        
--------------------------------
-- 
-- @function [parent=#NetManager] setEncryMsg 
-- @param self
-- @param #bool var
-- @return NetManager#NetManager self (return value: NetManager)
        
--------------------------------
-- 
-- @function [parent=#NetManager] NetManager 
-- @param self
-- @return NetManager#NetManager self (return value: NetManager)
        
return nil
