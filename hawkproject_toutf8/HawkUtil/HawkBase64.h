#ifndef HAWK_BASE64_H 
#define HAWK_BASE64_H 

#include "HawkRefCounter.h"

namespace Hawk
{
	/************************************************************************/
	/* Base64编码                                                           */
	/************************************************************************/
	class UTIL_API HawkBase64
	{
	public:
		//编码
		static void	Encode(const void* pData, Int32 iSize, AString& sOut);
			
		//解码
		static void	Decode(const AString& sData, void* pOut, Int32& iOut);
	};
}
#endif
