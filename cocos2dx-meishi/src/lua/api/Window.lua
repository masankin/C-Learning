
--------------------------------
-- @module Window
-- @extend PageBase
-- @parent_module 

--------------------------------
-- 
-- @function [parent=#Window] staticWindow 
-- @param self
-- @return Window#Window self (return value: Window)
        
--------------------------------
-- 
-- @function [parent=#Window] setShow 
-- @param self
-- @param #bool b
-- @return Window#Window self (return value: Window)
        
--------------------------------
-- 
-- @function [parent=#Window] onShow 
-- @param self
-- @return Window#Window self (return value: Window)
        
--------------------------------
-- 
-- @function [parent=#Window] getPriority 
-- @param self
-- @return int#int ret (return value: int)
        
--------------------------------
-- 
-- @function [parent=#Window] afterClose 
-- @param self
-- @return Window#Window self (return value: Window)
        
--------------------------------
-- 
-- @function [parent=#Window] onClose 
-- @param self
-- @return Window#Window self (return value: Window)
        
--------------------------------
-- 
-- @function [parent=#Window] isWindowLayoutEnable 
-- @param self
-- @return bool#bool ret (return value: bool)
        
--------------------------------
-- 
-- @function [parent=#Window] isShow 
-- @param self
-- @return bool#bool ret (return value: bool)
        
--------------------------------
-- 
-- @function [parent=#Window] setWindowLayoutEnable 
-- @param self
-- @param #bool enable
-- @return Window#Window self (return value: Window)
        
--------------------------------
-- 
-- @function [parent=#Window] getBgPanel 
-- @param self
-- @return Layout#Layout ret (return value: ccui.Layout)
        
--------------------------------
-- 
-- @function [parent=#Window] getLuaType 
-- @param self
-- @return int#int ret (return value: int)
        
--------------------------------
-- 
-- @function [parent=#Window] setLuaType 
-- @param self
-- @param #int var
-- @return Window#Window self (return value: Window)
        
--------------------------------
-- 
-- @function [parent=#Window] getShowFromLayer 
-- @param self
-- @return Widget#Widget ret (return value: ccui.Widget)
        
--------------------------------
-- 
-- @function [parent=#Window] setBgOpacity 
-- @param self
-- @param #int opacity
-- @return Window#Window self (return value: Window)
        
--------------------------------
-- 
-- @function [parent=#Window] normalClose 
-- @param self
-- @return Window#Window self (return value: Window)
        
--------------------------------
-- 
-- @function [parent=#Window] addChildCenter 
-- @param self
-- @param #cc.Node node
-- @return Window#Window self (return value: Window)
        
--------------------------------
-- 
-- @function [parent=#Window] setPageType 
-- @param self
-- @param #int pageType
-- @return Window#Window self (return value: Window)
        
--------------------------------
-- 
-- @function [parent=#Window] setPageName 
-- @param self
-- @param #string var
-- @return Window#Window self (return value: Window)
        
--------------------------------
-- 
-- @function [parent=#Window] getPageName 
-- @param self
-- @return string#string ret (return value: string)
        
--------------------------------
-- 
-- @function [parent=#Window] getShowingWindowSize 
-- @param self
-- @return int#int ret (return value: int)
        
--------------------------------
-- 
-- @function [parent=#Window] createWithName 
-- @param self
-- @param #string pageName
-- @param #int pageType
-- @param #int luaType
-- @return Window#Window ret (return value: Window)
        
--------------------------------
-- 
-- @function [parent=#Window] getWindowSize 
-- @param self
-- @return int#int ret (return value: int)
        
--------------------------------
-- 
-- @function [parent=#Window] setAllWindowHide 
-- @param self
-- @return Window#Window self (return value: Window)
        
--------------------------------
-- 
-- @function [parent=#Window] create 
-- @param self
-- @return Window#Window ret (return value: Window)
        
--------------------------------
-- 
-- @function [parent=#Window] showTextTip 
-- @param self
-- @param #string 
-- @param #vec2_table pPoint
-- @param #size_table pSize
-- @return Window#Window self (return value: Window)
        
--------------------------------
-- 
-- @function [parent=#Window] getWindow 
-- @param self
-- @param #string pageName
-- @return Window#Window ret (return value: Window)
        
--------------------------------
-- 
-- @function [parent=#Window] Create 
-- @param self
-- @param #string pageName
-- @param #int pageType
-- @param #int luaType
-- @return Window#Window ret (return value: Window)
        
--------------------------------
-- @overload self, int         
-- @overload self         
-- @overload self, ccui.Widget         
-- @overload self, int, ccui.Widget         
-- @function [parent=#Window] Window
-- @param self
-- @param #int priority
-- @param #ccui.Widget showFrom
-- @return Window#Window self (return value: Window)

return nil
