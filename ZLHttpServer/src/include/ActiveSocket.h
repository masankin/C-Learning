// ***********************************************************************
// Filename         : ActiveSocket.h
// Author           : LIZHENG
// Created          : 2014-07-04
// Description      : ���������������������socket�࣬����client��server������
//
// Last Modified By : LIZHENG
// Last Modified On : 2014-07-04
// 
// Copyright (c) lizhenghn@gmail.com. All rights reserved.
// ***********************************************************************
#ifndef ZL_ACTIVESOCKET_H
#define ZL_ACTIVESOCKET_H
#include "Socket.h"

NAMESPACE_ZL_START

class ActiveSocket : public Socket
{
public:
	ActiveSocket(const char* host, int port);
	~ActiveSocket();

public:
	const ActiveSocket& operator << (const std::string&) const;
	const ActiveSocket& operator >> (std::string&) const;
};

NAMESPACE_ZL_END

#endif  /* ZL_ACTIVESOCKET_H */