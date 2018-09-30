// ***********************************************************************
// Filename         : NonCopy.h
// Author           : LIZHENG
// Created          : 2014-05-16
// Description      : ��ֹ�������졢��ֵ���죬����private�̳�
//
// Last Modified By : LIZHENG
// Last Modified On : 2014-05-16
// 
// Copyright (c) lizhenghn@gmail.com. All rights reserved.
// ***********************************************************************
#ifndef ZL_NONCOPY_H
#define ZL_NONCOPY_H

namespace ZL
{
	class NonCopy
	{
	protected:
		NonCopy() {}
		~NonCopy() {}
	private:
		NonCopy(const NonCopy&);
		const NonCopy& operator=(const NonCopy&);
	};
}

#endif /* ZL_NONCOPY_H */