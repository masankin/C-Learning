#include "base/ccConfig.h"
#ifndef __lua_mgame_manual_h__
#define __lua_mgame_manual_h__


#ifdef __cplusplus
extern "C" {
#endif
#include "tolua++.h"
#ifdef __cplusplus
}
#endif

int register_all_mgame_manual(lua_State* tolua_S);

#endif
