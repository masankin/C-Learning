
--------------------------------
-- @module BuffManager
-- @extend MessageHandler
-- @parent_module 

--------------------------------
-- 
-- @function [parent=#BuffManager] setCampPeopleToFull 
-- @param self
-- @param #int camp
-- @param #int buffID
-- @return BuffManager#BuffManager self (return value: BuffManager)
        
--------------------------------
-- 
-- @function [parent=#BuffManager] getExistBuff 
-- @param self
-- @param #int camp
-- @param #int buffID
-- @return bool#bool ret (return value: bool)
        
--------------------------------
-- 
-- @function [parent=#BuffManager] getBuffProduceSpeed 
-- @param self
-- @param #int camp
-- @param #float baseValue
-- @return float#float ret (return value: float)
        
--------------------------------
-- 
-- @function [parent=#BuffManager] clearBuff 
-- @param self
-- @return BuffManager#BuffManager self (return value: BuffManager)
        
--------------------------------
-- 
-- @function [parent=#BuffManager] update 
-- @param self
-- @param #float deleta
-- @return BuffManager#BuffManager self (return value: BuffManager)
        
--------------------------------
-- 
-- @function [parent=#BuffManager] getBuffDefence 
-- @param self
-- @param #int camp
-- @param #float baseValue
-- @return float#float ret (return value: float)
        
--------------------------------
-- 
-- @function [parent=#BuffManager] getBuffAttack 
-- @param self
-- @param #int camp
-- @param #float baseValue
-- @return float#float ret (return value: float)
        
--------------------------------
-- 
-- @function [parent=#BuffManager] getBuffAttackSpeed 
-- @param self
-- @param #int camp
-- @param #float baseValue
-- @return float#float ret (return value: float)
        
--------------------------------
-- 
-- @function [parent=#BuffManager] getBuffListByCamp 
-- @param self
-- @param #int camp
-- @return map_table#map_table ret (return value: map_table)
        
--------------------------------
-- 
-- @function [parent=#BuffManager] getBuffMoveSpeed 
-- @param self
-- @param #int camp
-- @param #float baseValue
-- @return float#float ret (return value: float)
        
--------------------------------
-- 
-- @function [parent=#BuffManager] BuffManager 
-- @param self
-- @return BuffManager#BuffManager self (return value: BuffManager)
        
return nil
