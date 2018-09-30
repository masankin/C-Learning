
--------------------------------
-- @module PropsControl
-- @extend MessageHandler
-- @parent_module 

--------------------------------
-- 
-- @function [parent=#PropsControl] doCookingSolider 
-- @param self
-- @param #vec2_table pos
-- @param #int useCamp
-- @return PropsControl#PropsControl self (return value: PropsControl)
        
--------------------------------
-- 
-- @function [parent=#PropsControl] doExchange 
-- @param self
-- @param #int buildId
-- @param #int useCamp
-- @return PropsControl#PropsControl self (return value: PropsControl)
        
--------------------------------
-- 
-- @function [parent=#PropsControl] chechCdTimeOver 
-- @param self
-- @return PropsControl#PropsControl self (return value: PropsControl)
        
--------------------------------
-- 
-- @function [parent=#PropsControl] getSelectPropNum 
-- @param self
-- @return int#int ret (return value: int)
        
--------------------------------
-- 
-- @function [parent=#PropsControl] doAddTime 
-- @param self
-- @param #int useCamp
-- @return PropsControl#PropsControl self (return value: PropsControl)
        
--------------------------------
-- 
-- @function [parent=#PropsControl] setProp 
-- @param self
-- @param #int propId
-- @param #bool isNet
-- @return bool#bool ret (return value: bool)
        
--------------------------------
-- 
-- @function [parent=#PropsControl] checkIsCdTime 
-- @param self
-- @param #int propId
-- @return bool#bool ret (return value: bool)
        
--------------------------------
-- 
-- @function [parent=#PropsControl] updatePropItemShow 
-- @param self
-- @return PropsControl#PropsControl self (return value: PropsControl)
        
--------------------------------
-- 
-- @function [parent=#PropsControl] doInvisibility 
-- @param self
-- @param #int buildId
-- @param #int useCamp
-- @return PropsControl#PropsControl self (return value: PropsControl)
        
--------------------------------
-- 
-- @function [parent=#PropsControl] stopPropTipsAni 
-- @param self
-- @return PropsControl#PropsControl self (return value: PropsControl)
        
--------------------------------
-- 
-- @function [parent=#PropsControl] clearGuideProp 
-- @param self
-- @return PropsControl#PropsControl self (return value: PropsControl)
        
--------------------------------
-- 
-- @function [parent=#PropsControl] initGuideProp 
-- @param self
-- @param #cc.Node pRootNode
-- @param #int index
-- @return PropsControl#PropsControl self (return value: PropsControl)
        
--------------------------------
-- @overload self         
-- @overload self, int         
-- @function [parent=#PropsControl] cleanCdTime
-- @param self
-- @param #int propId
-- @return PropsControl#PropsControl self (return value: PropsControl)

--------------------------------
-- 
-- @function [parent=#PropsControl] doOil 
-- @param self
-- @param #vec2_table pos
-- @param #int useCamp
-- @return PropsControl#PropsControl self (return value: PropsControl)
        
--------------------------------
-- 
-- @function [parent=#PropsControl] propEvetCancel 
-- @param self
-- @return PropsControl#PropsControl self (return value: PropsControl)
        
--------------------------------
-- 
-- @function [parent=#PropsControl] getCurPropType 
-- @param self
-- @return int#int ret (return value: int)
        
--------------------------------
-- 
-- @function [parent=#PropsControl] isPropIng 
-- @param self
-- @return bool#bool ret (return value: bool)
        
--------------------------------
-- 
-- @function [parent=#PropsControl] sendUsePropRqst 
-- @param self
-- @param #int itemid
-- @param #int count
-- @return PropsControl#PropsControl self (return value: PropsControl)
        
--------------------------------
-- 
-- @function [parent=#PropsControl] init 
-- @param self
-- @return PropsControl#PropsControl self (return value: PropsControl)
        
--------------------------------
-- 
-- @function [parent=#PropsControl] doUseGuideProp 
-- @param self
-- @param #vec2_table pos
-- @return PropsControl#PropsControl self (return value: PropsControl)
        
--------------------------------
-- 
-- @function [parent=#PropsControl] updatePropTips 
-- @param self
-- @return PropsControl#PropsControl self (return value: PropsControl)
        
--------------------------------
-- 
-- @function [parent=#PropsControl] setTouchPos 
-- @param self
-- @param #vec2_table var
-- @return PropsControl#PropsControl self (return value: PropsControl)
        
--------------------------------
-- 
-- @function [parent=#PropsControl] checkPropInLevel 
-- @param self
-- @param #int pos
-- @return bool#bool ret (return value: bool)
        
--------------------------------
-- 
-- @function [parent=#PropsControl] doFreeze 
-- @param self
-- @param #int useCamp
-- @return PropsControl#PropsControl self (return value: PropsControl)
        
--------------------------------
-- 
-- @function [parent=#PropsControl] textCallBack 
-- @param self
-- @return PropsControl#PropsControl self (return value: PropsControl)
        
--------------------------------
-- 
-- @function [parent=#PropsControl] getCurPropId 
-- @param self
-- @return int#int ret (return value: int)
        
--------------------------------
-- 
-- @function [parent=#PropsControl] addGuideProp 
-- @param self
-- @param #int propID
-- @return PropsControl#PropsControl self (return value: PropsControl)
        
--------------------------------
-- 
-- @function [parent=#PropsControl] showNoticeText 
-- @param self
-- @param #string str
-- @return PropsControl#PropsControl self (return value: PropsControl)
        
--------------------------------
-- 
-- @function [parent=#PropsControl] initNeedTipProp 
-- @param self
-- @return PropsControl#PropsControl self (return value: PropsControl)
        
--------------------------------
-- 
-- @function [parent=#PropsControl] hasPropInLevel 
-- @param self
-- @return bool#bool ret (return value: bool)
        
--------------------------------
-- 
-- @function [parent=#PropsControl] sendBuyPropRqst 
-- @param self
-- @param #int itemid
-- @param #int price
-- @param #int count
-- @return PropsControl#PropsControl self (return value: PropsControl)
        
--------------------------------
-- 
-- @function [parent=#PropsControl] update 
-- @param self
-- @return PropsControl#PropsControl self (return value: PropsControl)
        
--------------------------------
-- 
-- @function [parent=#PropsControl] hideOtherItem 
-- @param self
-- @param #int id
-- @return PropsControl#PropsControl self (return value: PropsControl)
        
--------------------------------
-- 
-- @function [parent=#PropsControl] getSelectGuideProp 
-- @param self
-- @return int#int ret (return value: int)
        
--------------------------------
-- 
-- @function [parent=#PropsControl] showOrHideAimEffect 
-- @param self
-- @param #bool v
-- @return PropsControl#PropsControl self (return value: PropsControl)
        
--------------------------------
-- 
-- @function [parent=#PropsControl] getExchange 
-- @param self
-- @return bool#bool ret (return value: bool)
        
--------------------------------
-- 
-- @function [parent=#PropsControl] addPropItem 
-- @param self
-- @param #int propId
-- @return PropsControl#PropsControl self (return value: PropsControl)
        
--------------------------------
-- 
-- @function [parent=#PropsControl] initSaveProp 
-- @param self
-- @return PropsControl#PropsControl self (return value: PropsControl)
        
--------------------------------
-- 
-- @function [parent=#PropsControl] updatePropItem 
-- @param self
-- @param #int propId
-- @return PropsControl#PropsControl self (return value: PropsControl)
        
--------------------------------
-- 
-- @function [parent=#PropsControl] getTouchPos 
-- @param self
-- @return vec2_table#vec2_table ret (return value: vec2_table)
        
--------------------------------
-- 
-- @function [parent=#PropsControl] initPropCd 
-- @param self
-- @return PropsControl#PropsControl self (return value: PropsControl)
        
--------------------------------
-- 
-- @function [parent=#PropsControl] setSelectGuideProp 
-- @param self
-- @param #int var
-- @return PropsControl#PropsControl self (return value: PropsControl)
        
--------------------------------
-- 
-- @function [parent=#PropsControl] prePropEvet 
-- @param self
-- @return PropsControl#PropsControl self (return value: PropsControl)
        
--------------------------------
-- 
-- @function [parent=#PropsControl] getPosPropId 
-- @param self
-- @param #int pos
-- @return int#int ret (return value: int)
        
--------------------------------
-- 
-- @function [parent=#PropsControl] sendOpenChestRqst 
-- @param self
-- @param #int itemid
-- @param #int count
-- @return PropsControl#PropsControl self (return value: PropsControl)
        
--------------------------------
-- 
-- @function [parent=#PropsControl] removePropItem 
-- @param self
-- @param #int propId
-- @return PropsControl#PropsControl self (return value: PropsControl)
        
--------------------------------
-- 
-- @function [parent=#PropsControl] setExchange 
-- @param self
-- @param #bool var
-- @return PropsControl#PropsControl self (return value: PropsControl)
        
--------------------------------
-- 
-- @function [parent=#PropsControl] setPropItemShow 
-- @param self
-- @param #bool show
-- @return PropsControl#PropsControl self (return value: PropsControl)
        
--------------------------------
-- 
-- @function [parent=#PropsControl] getUsePropNum 
-- @param self
-- @return int#int ret (return value: int)
        
--------------------------------
-- 
-- @function [parent=#PropsControl] doFullPeopol 
-- @param self
-- @param #int buildId
-- @param #int eCamp
-- @return PropsControl#PropsControl self (return value: PropsControl)
        
--------------------------------
-- 
-- @function [parent=#PropsControl] initPvpProp 
-- @param self
-- @return PropsControl#PropsControl self (return value: PropsControl)
        
--------------------------------
-- 
-- @function [parent=#PropsControl] clean 
-- @param self
-- @return PropsControl#PropsControl self (return value: PropsControl)
        
--------------------------------
-- 
-- @function [parent=#PropsControl] doProp 
-- @param self
-- @param #vec2_table pos
-- @param #int useCamp
-- @param #bool isNet
-- @return bool#bool ret (return value: bool)
        
--------------------------------
-- 
-- @function [parent=#PropsControl] setCdTime 
-- @param self
-- @param #int propId
-- @return PropsControl#PropsControl self (return value: PropsControl)
        
--------------------------------
-- 
-- @function [parent=#PropsControl] PropsControl 
-- @param self
-- @return PropsControl#PropsControl self (return value: PropsControl)
        
return nil
