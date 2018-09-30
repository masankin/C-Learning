
--------------------------------
-- @module ActionWindow
-- @extend Window
-- @parent_module 

--------------------------------
-- 
-- @function [parent=#ActionWindow] setIsPlayBorn 
-- @param self
-- @param #bool isPlay
-- @return ActionWindow#ActionWindow self (return value: ActionWindow)
        
--------------------------------
-- 
-- @function [parent=#ActionWindow] setIsPlayEndOnClose 
-- @param self
-- @param #bool isPlay
-- @return ActionWindow#ActionWindow self (return value: ActionWindow)
        
--------------------------------
-- 
-- @function [parent=#ActionWindow] _initWindow 
-- @param self
-- @param #int priority
-- @param #ccui.Widget showFrom
-- @return bool#bool ret (return value: bool)
        
--------------------------------
-- 
-- @function [parent=#ActionWindow] getArmatureBg 
-- @param self
-- @return Armature#Armature ret (return value: ccs.Armature)
        
--------------------------------
-- 
-- @function [parent=#ActionWindow] getBonePosition 
-- @param self
-- @param #string boneName
-- @param #vec2_table offPos
-- @param #ccs.Armature ani
-- @return vec2_table#vec2_table ret (return value: vec2_table)
        
--------------------------------
-- 
-- @function [parent=#ActionWindow] changeBoneByDefaultArmature 
-- @param self
-- @param #string stBone
-- @param #cc.Node display
-- @param #int index
-- @return ActionWindow#ActionWindow self (return value: ActionWindow)
        
--------------------------------
-- 
-- @function [parent=#ActionWindow] setAnimation 
-- @param self
-- @param #string name
-- @return bool#bool ret (return value: bool)
        
--------------------------------
-- 
-- @function [parent=#ActionWindow] changeBone 
-- @param self
-- @param #ccs.Armature pAr
-- @param #string stBone
-- @param #cc.Node display
-- @param #int index
-- @return ActionWindow#ActionWindow self (return value: ActionWindow)
        
--------------------------------
-- 
-- @function [parent=#ActionWindow] playAnimationByName 
-- @param self
-- @param #string name
-- @return ActionWindow#ActionWindow self (return value: ActionWindow)
        
--------------------------------
-- 
-- @function [parent=#ActionWindow] setStandName 
-- @param self
-- @param #string name
-- @return ActionWindow#ActionWindow self (return value: ActionWindow)
        
--------------------------------
-- 
-- @function [parent=#ActionWindow] setPopType 
-- @param self
-- @param #int type
-- @return ActionWindow#ActionWindow self (return value: ActionWindow)
        
--------------------------------
-- 
-- @function [parent=#ActionWindow] create 
-- @param self
-- @return ActionWindow#ActionWindow ret (return value: ActionWindow)
        
--------------------------------
-- 
-- @function [parent=#ActionWindow] setShow 
-- @param self
-- @param #bool b
-- @return ActionWindow#ActionWindow self (return value: ActionWindow)
        
--------------------------------
-- 
-- @function [parent=#ActionWindow] ActionWindow 
-- @param self
-- @return ActionWindow#ActionWindow self (return value: ActionWindow)
        
return nil
