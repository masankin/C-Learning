
--------------------------------
-- @module GuideManager
-- @extend MessageHandler
-- @parent_module 

--------------------------------
-- 
-- @function [parent=#GuideManager] setExtraCondition 
-- @param self
-- @param #int var
-- @return GuideManager#GuideManager self (return value: GuideManager)
        
--------------------------------
-- 
-- @function [parent=#GuideManager] setCloseGuide 
-- @param self
-- @param #bool var
-- @return GuideManager#GuideManager self (return value: GuideManager)
        
--------------------------------
-- 
-- @function [parent=#GuideManager] resetGuideDragFlag 
-- @param self
-- @param #int gateID
-- @return GuideManager#GuideManager self (return value: GuideManager)
        
--------------------------------
-- 
-- @function [parent=#GuideManager] setCurGuideGroup 
-- @param self
-- @param #int var
-- @return GuideManager#GuideManager self (return value: GuideManager)
        
--------------------------------
-- 
-- @function [parent=#GuideManager] getDragCondition 
-- @param self
-- @return int#int ret (return value: int)
        
--------------------------------
-- 
-- @function [parent=#GuideManager] checkOpenSoldierControl 
-- @param self
-- @param #int gateID
-- @return bool#bool ret (return value: bool)
        
--------------------------------
-- 
-- @function [parent=#GuideManager] rollbackRookieGuide 
-- @param self
-- @return GuideManager#GuideManager self (return value: GuideManager)
        
--------------------------------
-- 
-- @function [parent=#GuideManager] getCurGuideData 
-- @param self
-- @return GuideData#GuideData ret (return value: GuideData)
        
--------------------------------
-- 
-- @function [parent=#GuideManager] getExtraCondition2 
-- @param self
-- @return int#int ret (return value: int)
        
--------------------------------
-- 
-- @function [parent=#GuideManager] unRegistGuideMessage 
-- @param self
-- @return bool#bool ret (return value: bool)
        
--------------------------------
-- 
-- @function [parent=#GuideManager] getCloseGuide 
-- @param self
-- @return bool#bool ret (return value: bool)
        
--------------------------------
-- 
-- @function [parent=#GuideManager] getGuideLock 
-- @param self
-- @return bool#bool ret (return value: bool)
        
--------------------------------
-- 
-- @function [parent=#GuideManager] getOpenRookieCamp 
-- @param self
-- @return bool#bool ret (return value: bool)
        
--------------------------------
-- 
-- @function [parent=#GuideManager] checkHaveGateGuide 
-- @param self
-- @param #int gateID
-- @return bool#bool ret (return value: bool)
        
--------------------------------
-- 
-- @function [parent=#GuideManager] checkGuideFinishByGroup 
-- @param self
-- @param #int group
-- @return bool#bool ret (return value: bool)
        
--------------------------------
-- 
-- @function [parent=#GuideManager] checkOpenPropControl 
-- @param self
-- @param #int gateID
-- @return bool#bool ret (return value: bool)
        
--------------------------------
-- 
-- @function [parent=#GuideManager] initGuideData 
-- @param self
-- @return GuideManager#GuideManager self (return value: GuideManager)
        
--------------------------------
-- 
-- @function [parent=#GuideManager] setDragCondition 
-- @param self
-- @param #int var
-- @return GuideManager#GuideManager self (return value: GuideManager)
        
--------------------------------
-- 
-- @function [parent=#GuideManager] init 
-- @param self
-- @return bool#bool ret (return value: bool)
        
--------------------------------
-- 
-- @function [parent=#GuideManager] getExtraCondition 
-- @param self
-- @return int#int ret (return value: int)
        
--------------------------------
-- 
-- @function [parent=#GuideManager] registGuideMessage 
-- @param self
-- @param #int gateID
-- @param #int levelID
-- @param #int guideLocation
-- @param #cc.Node parentNode
-- @param #cc.Node sceneNode
-- @param #bool highestLayer
-- @param #vec2_table guideLayerPos
-- @return bool#bool ret (return value: bool)
        
--------------------------------
-- 
-- @function [parent=#GuideManager] checkRookieFlag 
-- @param self
-- @return bool#bool ret (return value: bool)
        
--------------------------------
-- 
-- @function [parent=#GuideManager] setGuideLock 
-- @param self
-- @param #bool lock
-- @return GuideManager#GuideManager self (return value: GuideManager)
        
--------------------------------
-- 
-- @function [parent=#GuideManager] getIsRunning 
-- @param self
-- @return bool#bool ret (return value: bool)
        
--------------------------------
-- 
-- @function [parent=#GuideManager] checkIsReady 
-- @param self
-- @return bool#bool ret (return value: bool)
        
--------------------------------
-- 
-- @function [parent=#GuideManager] getCurGuideGroup 
-- @param self
-- @return int#int ret (return value: int)
        
--------------------------------
-- 
-- @function [parent=#GuideManager] getCurGuideID 
-- @param self
-- @return int#int ret (return value: int)
        
--------------------------------
-- 
-- @function [parent=#GuideManager] getGuideDataByGroup 
-- @param self
-- @param #int groupID
-- @return GuideData#GuideData ret (return value: GuideData)
        
--------------------------------
-- 
-- @function [parent=#GuideManager] update 
-- @param self
-- @param #float delta
-- @return GuideManager#GuideManager self (return value: GuideManager)
        
--------------------------------
-- 
-- @function [parent=#GuideManager] getGuideLayer 
-- @param self
-- @return Layer#Layer ret (return value: cc.Layer)
        
--------------------------------
-- 
-- @function [parent=#GuideManager] setCurGuideGroupFinish 
-- @param self
-- @return GuideManager#GuideManager self (return value: GuideManager)
        
--------------------------------
-- 
-- @function [parent=#GuideManager] rollbackGuide 
-- @param self
-- @return GuideManager#GuideManager self (return value: GuideManager)
        
--------------------------------
-- 
-- @function [parent=#GuideManager] checkOpeartor 
-- @param self
-- @param #int operatorType
-- @return bool#bool ret (return value: bool)
        
--------------------------------
-- 
-- @function [parent=#GuideManager] saveGuideData 
-- @param self
-- @param #int id
-- @return GuideManager#GuideManager self (return value: GuideManager)
        
--------------------------------
-- 
-- @function [parent=#GuideManager] getIsRookie 
-- @param self
-- @return bool#bool ret (return value: bool)
        
--------------------------------
-- 
-- @function [parent=#GuideManager] checkNeedPower 
-- @param self
-- @param #int gateID
-- @return bool#bool ret (return value: bool)
        
--------------------------------
-- 
-- @function [parent=#GuideManager] checkGuideFinishByID 
-- @param self
-- @param #int id
-- @return bool#bool ret (return value: bool)
        
--------------------------------
-- 
-- @function [parent=#GuideManager] setCurGuideID 
-- @param self
-- @param #int var
-- @return GuideManager#GuideManager self (return value: GuideManager)
        
--------------------------------
-- 
-- @function [parent=#GuideManager] setOpenRookieCamp 
-- @param self
-- @param #bool var
-- @return GuideManager#GuideManager self (return value: GuideManager)
        
--------------------------------
-- 
-- @function [parent=#GuideManager] setGroupFinishByID 
-- @param self
-- @param #int group
-- @param #bool serverSave
-- @return GuideManager#GuideManager self (return value: GuideManager)
        
--------------------------------
-- 
-- @function [parent=#GuideManager] clearGuideData 
-- @param self
-- @return GuideManager#GuideManager self (return value: GuideManager)
        
--------------------------------
-- 
-- @function [parent=#GuideManager] getStopDragFlag 
-- @param self
-- @return bool#bool ret (return value: bool)
        
--------------------------------
-- 
-- @function [parent=#GuideManager] setIsRookie 
-- @param self
-- @param #bool var
-- @return GuideManager#GuideManager self (return value: GuideManager)
        
--------------------------------
-- 
-- @function [parent=#GuideManager] checkDragOperator 
-- @param self
-- @return bool#bool ret (return value: bool)
        
--------------------------------
-- 
-- @function [parent=#GuideManager] setStopDragFlag 
-- @param self
-- @param #bool flag
-- @return GuideManager#GuideManager self (return value: GuideManager)
        
--------------------------------
-- 
-- @function [parent=#GuideManager] checkGuideTypeFinished 
-- @param self
-- @param #int type
-- @return bool#bool ret (return value: bool)
        
--------------------------------
-- 
-- @function [parent=#GuideManager] getGuideDataByGuideID 
-- @param self
-- @param #int guideID
-- @return GuideData#GuideData ret (return value: GuideData)
        
--------------------------------
-- 
-- @function [parent=#GuideManager] setExtraCondition2 
-- @param self
-- @param #int var
-- @return GuideManager#GuideManager self (return value: GuideManager)
        
--------------------------------
-- 
-- @function [parent=#GuideManager] GuideManager 
-- @param self
-- @return GuideManager#GuideManager self (return value: GuideManager)
        
return nil
