// ***********************************************************************
// Filename         : Define.h
// Author           : LIZHENG
// Created          : 2014-07-01
// Description      : 
//
// Last Modified By : LIZHENG
// Last Modified On : 2014-07-01
// 
// Copyright (c) lizhenghn@gmail.com. All rights reserved.
// ***********************************************************************
#ifndef ZL_STDDEFINE_H
#define ZL_STDDEFINE_H

#include <vector>
#include <string>
#include <list>
#include <queue>
#include <map>
#include <set>
#include <hash_map>
#include <hash_map>
#include <algorithm>
#include <functional>
#include <iterator>
#include <numeric>

#include "OsDefine.h"


#define NAMESPACE_ZL_START namespace ZL {
#define NAMESPACE_ZL_END   }


#define Safe_Delete(p)        do { delete p; p = NULL; } while (0);
#define Safe_Delete_Array(p)  do { delete[] p; p = NULL; } while (0);

#define ZL_NDEBUG 1
#ifdef  ZL_NDEBUG
#define ZL_ASSERT(expr) ((void) 0)
#define ZL_ASSERTEX(expr, file, lineno, func) ((void) 0)
#else
#define ZL_ASSERT(expr)                                    \
			((void) ((expr) ? 0 :                          \
		    printf("%s:%d: %s:  Assertion `%s' failed.\n", \
			__FILE__, __LINE__, __FUNCTION__, #expr)))
#define ZL_ASSERTEX(expr, file, lineno, func)              \
	        ((void) ((expr) ? 0 :                          \
		    printf("%s:%d: %s: Assertion `%s' failed. "    \
		    "(called from %s:%d:%s)\n",                    \
		     __FILE__, __LINE__, __FUNCTION__, #expr,      \
            file, lineno, func)))
#endif

#endif /* ZL_STDDEFINE_H */