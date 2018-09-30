#pragma once
#include <queue>
#include <list>

#ifdef _WINDOWS
#include <windows.h>
#else
#include <pthread.h>
#endif//WINDOWS


#include "Singleton.h" 


#ifndef ULONG
typedef unsigned long ULONG;
#endif //ULONG


//thread entrance is different caused by different platform
#ifndef THREAD_ENTRANCE_RETURN
#ifdef _WINDOWS
typedef unsigned int THREAD_ENTRANCE_RETURN;
#else
typedef void* THREAD_ENTRANCE_RETURN;
#endif//WINDOWS
#endif//THREAD_ENTRANCE_RETURN

#ifndef THREAD_ENTRANCE_CALL
#ifdef _WINDOWS
#define THREAD_ENTRANCE_CALL __stdcall
#else
#define THREAD_ENTRANCE_CALL 
#endif//WINDOWS
#endif//THREAD_ENTRANCE_CALL

#ifndef PTR_THREAD_ENTRANCE
typedef THREAD_ENTRANCE_RETURN(THREAD_ENTRANCE_CALL *PTR_THREAD_ENTRANCE)(void*);
#endif//PTR_THREAD_ENTRANCE


namespace GameCom
{

	static void sleeping(float ts);

	//========================================================================================
	//Mutex, lock protected for multi-thread
	class Mutex
	{
	public:
		Mutex();
		virtual ~Mutex();

		virtual void lock();
		virtual void unlock();

	protected:
#ifdef _WINDOWS
		HANDLE _mutex;
#else
		pthread_mutex_t _mutex;
#endif//WINDOWS
	};

	//========================================================================================
	// AutoReleaseLock
	class AutoReleaseLock
	{
	protected:
		Mutex& mMutex;
	public:
		AutoReleaseLock(Mutex& _mutex)
			:mMutex(_mutex)
		{
			_mutex.lock();
		}
		virtual ~AutoReleaseLock()
		{
			mMutex.unlock();
		}
	};

	class ThreadBase;

	//========================================================================================
	// thread state
	class ThreadState// : public Singleton<ThreadState>
	{
	public:
		ThreadState(){}
		virtual ~ThreadState(){}
		virtual int start(PTR_THREAD_ENTRANCE pEntrance, void* params, ThreadBase* tb){ return 0; }
		virtual int stop(ThreadBase* tb){ return 0; }
	};

	class ThreadStateRunning : public ThreadState//, public Singleton<ThreadStateRunning>
	{
	public:
		ThreadStateRunning(){}
		virtual ~ThreadStateRunning(){}
		virtual int stop(ThreadBase* tb);

		static ThreadStateRunning* instance(){ return new ThreadStateRunning(); }
	};

	class ThreadStateStoped : public ThreadState//, public Singleton<ThreadStateStoped>
	{
	public:
		ThreadStateStoped(){}
		virtual ~ThreadStateStoped(){}
		virtual int start(PTR_THREAD_ENTRANCE pEntrance, void* params, ThreadBase* tb);

		static ThreadStateStoped* instance(){ return new ThreadStateStoped(); }
	};


	//========================================================================================
	//base thread
	class ThreadBase
	{
	public:
		ThreadBase();
		virtual ~ThreadBase();

		virtual int start(PTR_THREAD_ENTRANCE pEntrance, void* params);
		virtual int stop();
		virtual void setState(ThreadState* state);

		void	destroy();

		friend class ThreadState;
		friend class ThreadStateRunning;
		friend class ThreadStateStoped;
	protected:
		//Mutex _mutex;
		ThreadState* _state;
#ifdef _WINDOWS
		HANDLE _mthread;
		unsigned int _tid;
#else
		pthread_t _mthread;
#endif //WINDOWS
	};


	//========================================================================================
	//thread task interface
	class ThreadTask
	{
	public:
		ThreadTask(){}
		virtual ~ThreadTask(){}

		virtual int run(){ return 0; }
	};

	//execute interface
	class ThreadExecuter
	{
	public:
		ThreadExecuter(){}
		virtual ~ThreadExecuter(){}

		virtual int execute(ThreadTask* task) = 0;
		virtual void shutdown() = 0;
		virtual ThreadTask* fetchWork() = 0;
	};

	//service
	class ThreadService : public ThreadExecuter
	{
	public:
		ThreadService();
		virtual ~ThreadService();

		virtual int execute(ThreadTask* task);
		virtual void shutdown();
		virtual ThreadTask* fetchWork(){ return _mTask; }
		virtual void resetWork(){ _mTask = 0; }
		static THREAD_ENTRANCE_RETURN THREAD_ENTRANCE_CALL entrance(void* params);
	protected:
		Mutex _mutex;
		ThreadTask* _mTask;
		ThreadBase _mThread;
	};

	class SingleThreadExecuter : public ThreadExecuter
	{
	public:
		SingleThreadExecuter();
		virtual ~SingleThreadExecuter();

		virtual int execute(ThreadTask* task);
		virtual void shutdown();
		virtual ThreadTask* fetchWork();
		void assignWork(ThreadTask *task);
		static THREAD_ENTRANCE_RETURN THREAD_ENTRANCE_CALL entrance(void* params);

		bool isRunning();

	protected:
		void clearWork();
	protected:
		Mutex _mutexWork;
		Mutex _mutex;
		std::queue<ThreadTask*> _mWorkQueue;
		ThreadBase _mThread;
		bool _running;
	};
}
