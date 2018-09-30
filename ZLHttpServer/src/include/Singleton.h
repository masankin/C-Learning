// ***********************************************************************
// Filename         : Singleton.h
// Author           : LIZHENG
// Created          : 2014-07-01
// Description      : 线程安全的单例模板类.可用于多线程中，可手动或自动释放单例实例
//                    使用方式：
//                    class TestSingleton : public SingleTon<TestSingleton>
//                    {  
//                         DECLARE_SINGLETON_CLASS(type);
//                    }
//
// Last Modified By : LIZHENG
// Last Modified On : 2014-07-01
// 
// Copyright (c) lizhenghn@gmail.com. All rights reserved.
// ***********************************************************************
#ifndef ZL_SINGLETON_H
#define ZL_SINGLETON_H

#include "Define.h"
#include <mutex>

NAMESPACE_ZL_START

#define DECLARE_SINGLETON_CLASS(type)  friend class Singleton< type >

template<class T>
class Singleton
{
public:
	static T *GetInstancePtr()
	{
		if (0 == proxy_.instance_)
		{
			CreateInstance();
		}
		return proxy_.instance_;
	}

	static T& GetInstanceRef()
	{
		if (0 == proxy_.instance_)
		{
			CreateInstance();
		}
		return *(proxy_.instance_);
	}

	static bool	CleanInstance()
	{
		if (proxy_.instance_)
		{
			delete proxy_.instance_;
			proxy_.instance_ = 0;
		}
		return true;
	}
private:
	static void CreateInstance()
	{
		proxy_.CreateInstance();
	}
private:
	struct Proxy
	{
		Proxy() : instance_(0)
		{	}

		~Proxy()
		{
			if (instance_)
			{
				delete instance_;
				instance_ = 0;
			}
		}

		void CreateInstance()
		{
			if (0 == instance_)
			{
				std::lock_guard<std::mutex> guard(lock_);
				if (0 == instance_)
				{
					instance_ = new T;
				}
			}
		}

		T *instance_;	
		std::mutex lock_;
	};
protected:
	Singleton()  {	}
	~Singleton() {	}
private:
	static Proxy proxy_;
};

template < typename T >
typename Singleton<T>::Proxy Singleton<T>::proxy_;

NAMESPACE_ZL_END

#endif /* ZL_SINGLETON_H */