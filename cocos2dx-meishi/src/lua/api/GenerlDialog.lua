
--------------------------------
-- @module GenerlDialog
-- @extend ActionWindow
-- @parent_module 

--------------------------------
-- 
-- @function [parent=#GenerlDialog] init 
-- @param self
-- @return bool#bool ret (return value: bool)
        
--------------------------------
-- 
-- @function [parent=#GenerlDialog] hide 
-- @param self
-- @return GenerlDialog#GenerlDialog self (return value: GenerlDialog)
        
--------------------------------
-- @overload self, string, string, function, bool, bool, int         
-- @overload self, string, string, bool         
-- @overload self, string, string, function, function, bool, bool         
-- @function [parent=#GenerlDialog] show
-- @param self
-- @param #string stHead
-- @param #string stContent
-- @param #function ConfirmCallBack
-- @param #function CloseCallBack
-- @param #bool isAll
-- @param #bool bEnableClose
-- @return GenerlDialog#GenerlDialog self (return value: GenerlDialog)

--------------------------------
-- 
-- @function [parent=#GenerlDialog] setHeadText 
-- @param self
-- @param #string stHead
-- @return GenerlDialog#GenerlDialog self (return value: GenerlDialog)
        
--------------------------------
-- 
-- @function [parent=#GenerlDialog] setContentShow 
-- @param self
-- @return GenerlDialog#GenerlDialog self (return value: GenerlDialog)
        
--------------------------------
-- 
-- @function [parent=#GenerlDialog] setContentText 
-- @param self
-- @param #string stHead
-- @return GenerlDialog#GenerlDialog self (return value: GenerlDialog)
        
--------------------------------
-- 
-- @function [parent=#GenerlDialog] showLuaDialog 
-- @param self
-- @param #string stHead
-- @param #string stContent
-- @param #string ConfirmCallBackStr
-- @param #string CancelCallBackStr
-- @param #string CloseCallBackStr
-- @param #bool bEnableCancel
-- @param #bool bEnableClose
-- @return GenerlDialog#GenerlDialog self (return value: GenerlDialog)
        
--------------------------------
-- 
-- @function [parent=#GenerlDialog] setCancel 
-- @param self
-- @param #function 
-- @return GenerlDialog#GenerlDialog self (return value: GenerlDialog)
        
--------------------------------
-- 
-- @function [parent=#GenerlDialog] create 
-- @param self
-- @return GenerlDialog#GenerlDialog ret (return value: GenerlDialog)
        
--------------------------------
-- 
-- @function [parent=#GenerlDialog] GenerlDialog 
-- @param self
-- @return GenerlDialog#GenerlDialog self (return value: GenerlDialog)
        
return nil
