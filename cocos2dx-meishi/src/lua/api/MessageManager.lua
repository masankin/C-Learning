
--------------------------------
-- @module MessageManager
-- @parent_module 

--------------------------------
-- @overload self, MessageHandler         
-- @overload self, unsigned int, MessageHandler         
-- @overload self, unsigned int         
-- @function [parent=#MessageManager] removeMessageHandler
-- @param self
-- @param #unsigned int id
-- @param #MessageHandler messagehandler
-- @return MessageManager#MessageManager self (return value: MessageManager)

--------------------------------
-- 
-- @function [parent=#MessageManager] createMessage 
-- @param self
-- @param #int id
-- @param #string params
-- @return GameMessage#GameMessage ret (return value: GameMessage)
        
--------------------------------
-- 
-- @function [parent=#MessageManager] regisiterMessageHandler 
-- @param self
-- @param #unsigned int id
-- @param #MessageHandler handler
-- @return MessageManager#MessageManager self (return value: MessageManager)
        
--------------------------------
-- 
-- @function [parent=#MessageManager] update 
-- @param self
-- @return MessageManager#MessageManager self (return value: MessageManager)
        
--------------------------------
-- @overload self, int, string         
-- @overload self, GameMessage         
-- @function [parent=#MessageManager] sendMessage
-- @param self
-- @param #int eType
-- @param #string params
-- @return MessageManager#MessageManager self (return value: MessageManager)

--------------------------------
-- 
-- @function [parent=#MessageManager] loadFactorys 
-- @param self
-- @return MessageManager#MessageManager self (return value: MessageManager)
        
--------------------------------
-- 
-- @function [parent=#MessageManager] registerFactory 
-- @param self
-- @param #int id
-- @param #GameMessageFactory factory
-- @return bool#bool ret (return value: bool)
        
--------------------------------
-- 
-- @function [parent=#MessageManager] MessageManager 
-- @param self
-- @return MessageManager#MessageManager self (return value: MessageManager)
        
return nil
