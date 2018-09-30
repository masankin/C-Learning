// ***********************************************************************
// Filename         : Utils.h
// Author           : LIZHENG
// Created          : 2014-07-01
// Description      : ¹¤¾ßº¯Êý
//
// Last Modified By : LIZHENG
// Last Modified On : 2014-06-23
// 
// Copyright (c) lizhenghn@gmail.com. All rights reserved.
// ***********************************************************************
#ifndef ZL_UTILS_H
#define ZL_UTILS_H
#include "Define.h"

NAMESPACE_ZL_START

// ------------------------  file utils  ------------------------
bool  IsFileExist(const char* filepath);
long  GetFileSize(FILE *file);
long  GetFileSize(const char* filepath);
bool  GetFileData(const char* filepath, std::string& buf);

// ------------------------ string utils ------------------------
template<typename T> 
std::string ToStr(const T& value)
{
	std::ostringstream oss;
	oss << value;
	return oss.str();
}

template<typename T> 
T ToValue(const std::string& str)
{
	T value;
	std::istringstream iss(str);
	iss >> value;
	return value;
}

inline std::string TrimLeft(const std::string& str, const std::string& token)
{
	std::string t = str;
	t.erase(0, t.find_first_not_of(token));
	return t;
}

inline std::string TrimRight(const std::string& str, const std::string& token)
{
	std::string t = str;
	t.erase(t.find_last_not_of(token) + 1);
	return t;
}

inline std::string Trim(const std::string& str, const std::string& token)
{
	std::string t = str;
	t.erase(0, t.find_first_not_of(token));
	t.erase(t.find_last_not_of(token) + 1);
	return t;
}

inline std::string ToLower(const std::string& str)
{
	std::string t = str;
	std::transform(t.begin(), t.end(), t.begin(), tolower);
	return t;
}

inline std::string ToUpper(const std::string& str)
{
	std::string t = str;
	transform(t.begin(), t.end(), t.begin(), toupper);
	return t;
}

inline bool	StartsWith(const std::string& str, const std::string& substr)
{
	return str.find(substr) == 0;
}

inline bool EndsWith(const std::string& str, const std::string& substr)
{
	return str.rfind(substr) == (str.length() - substr.length());
}

inline bool EqualsIgnoreCase(const std::string& str1, const std::string& str2)
{
	return ToLower(str1) == ToLower(str2);
}


NAMESPACE_ZL_END

#endif /* ZL_UTILS_H */