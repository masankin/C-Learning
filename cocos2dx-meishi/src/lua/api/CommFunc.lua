
--------------------------------
-- @module CommFunc
-- @parent_module 

--------------------------------
-- 
-- @function [parent=#CommFunc] commParseUTFStr 
-- @param self
-- @param #string sin
-- @param #array_table ret
-- @return bool#bool ret (return value: bool)
        
--------------------------------
-- 
-- @function [parent=#CommFunc] commGetRichFormatText 
-- @param self
-- @param #string str
-- @return string#string ret (return value: string)
        
--------------------------------
-- 
-- @function [parent=#CommFunc] getEnvSet 
-- @param self
-- @param #char val
-- @return int#int ret (return value: int)
        
--------------------------------
-- 
-- @function [parent=#CommFunc] commSplitStrToInt 
-- @param self
-- @param #string str
-- @param #string pattern
-- @return array_table#array_table ret (return value: array_table)
        
--------------------------------
-- @overload self, string, char, char         
-- @overload self, string, char         
-- @function [parent=#CommFunc] getStrValue
-- @param self
-- @param #string source
-- @param #char begin
-- @param #char end
-- @return string#string ret (return value: string)

--------------------------------
-- 
-- @function [parent=#CommFunc] commSplitStr 
-- @param self
-- @param #string str
-- @param #string pattern
-- @return array_table#array_table ret (return value: array_table)
        
--------------------------------
-- 
-- @function [parent=#CommFunc] getRandName 
-- @param self
-- @return string#string ret (return value: string)
        
--------------------------------
-- 
-- @function [parent=#CommFunc] randomStrFromPlist 
-- @param self
-- @param #char pszFilename
-- @return __String#__String ret (return value: cc.__String)
        
--------------------------------
-- 
-- @function [parent=#CommFunc] commSetGray 
-- @param self
-- @param #cc.Node target
-- @return CommFunc#CommFunc self (return value: CommFunc)
        
--------------------------------
-- 
-- @function [parent=#CommFunc] commCheckCharacter 
-- @param self
-- @param #string str
-- @param #string cStr
-- @return int#int ret (return value: int)
        
return nil
