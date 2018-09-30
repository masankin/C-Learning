// ***********************************************************************
// Filename         : HttpKeyValue.h
// Author           : LIZHENG
// Created          : 2014-07-02
// Description      : 
//
// Last Modified By : LIZHENG
// Last Modified On : 2014-07-02
// 
// Copyright (c) lizhenghn@gmail.com. All rights reserved.
// ***********************************************************************
#ifndef ZL_HTTPKEYVALUE_H
#define ZL_HTTPKEYVALUE_H
#include "Define.h"
#include "Singleton.h"

NAMESPACE_ZL_START

class HttpKeyValue : public ZL::Singleton < HttpKeyValue >
{
	DECLARE_SINGLETON_CLASS(HttpKeyValue);

public:
	std::string GetReasonDesc(int code);
	std::string GetContentType(const std::string& file_type);
private:
	void Initialise();

private:
	std::map<int, std::string>          code_desc_;
	std::map<std::string, std::string>  content_type_;
	
private:
	HttpKeyValue();
};

NAMESPACE_ZL_END

#endif /* ZL_HTTPKEYVALUE_H */