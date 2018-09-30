#include "HawkSpinLock.h"
#include "HawkOSOperator.h"
#include "pthread.h"


/**by masankin**/
#ifndef PTHREAD_SPIN_LOCK_SHIM
#define PTHREAD_SPIN_LOCK_SHIM

typedef int pthread_spinlock_t;

#ifndef PTHREAD_PROCESS_SHARED
# define PTHREAD_PROCESS_SHARED 1
#endif
#ifndef PTHREAD_PROCESS_PRIVATE
# define PTHREAD_PROCESS_PRIVATE 2
#endif

static inline int pthread_spin_init(pthread_spinlock_t *lock, int pshared) {
  __asm__ __volatile__ ("" ::: "memory");
  *lock = 0;
  return 0;
}

static inline int pthread_spin_destroy(pthread_spinlock_t *lock) {
  return 0;
}

static inline int pthread_spin_lock(pthread_spinlock_t *lock) {
  while (1) {
    int i;
    for (i=0; i < 10000; i++) {
      if (__sync_bool_compare_and_swap(lock, 0, 1)) {
        return 0;
      }
    }
    sched_yield();
  }
}

static inline int pthread_spin_trylock(pthread_spinlock_t *lock) {
  if (__sync_bool_compare_and_swap(lock, 0, 1)) {
    return 0;
  }
  return EBUSY;
}

static inline int pthread_spin_unlock(pthread_spinlock_t *lock) {
  __asm__ __volatile__ ("" ::: "memory");
  *lock = 0;
  return 0;
}
#endif
/**by masankin**/

namespace Hawk
{
	HawkSpinLock::Scope::Scope(HawkSpinLock* pSpinLock, const AString& sFile, Int32 iLine) : m_pSpinLock(pSpinLock)
	{
		if (m_pSpinLock) 
			m_pSpinLock->Lock(sFile,iLine);
	}

	HawkSpinLock::Scope::~Scope()
	{
		if (m_pSpinLock)
			m_pSpinLock->Unlock();
	}

	//////////////////////////////////////////////////////////////////////////

	HawkSpinLock::HawkSpinLock() : m_pSpinLock(0)
	{
		m_pSpinLock = HawkMalloc(sizeof(pthread_spinlock_t));	
		if (pthread_spin_init((pthread_spinlock_t*)m_pSpinLock, 0) != HAWK_OK)
		{
			HawkFree(m_pSpinLock);
			m_pSpinLock = 0;
		}

#ifdef _DEBUG
		m_bLocked = false;
		m_sFile   = "";
		m_iLine   = 0;
		m_iThread = 0;
#endif
	}

	HawkSpinLock::~HawkSpinLock()
	{
		if (m_pSpinLock)
		{
			pthread_spin_destroy((pthread_spinlock_t*)m_pSpinLock);
			HawkFree(m_pSpinLock);
			m_pSpinLock = 0;
		}
	}

	Bool HawkSpinLock::Lock(const AString& sFile,Int32 iLine)
	{
		HawkAssert(m_pSpinLock);

		if (pthread_spin_lock((pthread_spinlock_t*)m_pSpinLock) == HAWK_OK)
		{
#ifdef _DEBUG
			m_bLocked = true;
			m_sFile   = sFile;
			m_iLine   = iLine;
			m_iThread = HawkOSOperator::GetThreadId();
#endif
			return true;
		}
		return false;
	}

	Bool HawkSpinLock::TryLock(const AString& sFile,Int32 iLine)
	{
		HawkAssert(m_pSpinLock);

		if (pthread_spin_trylock((pthread_spinlock_t*)m_pSpinLock) == HAWK_OK)
		{
#ifdef _DEBUG
			m_bLocked = true;
			m_sFile   = sFile;
			m_iLine   = iLine;
			m_iThread = HawkOSOperator::GetThreadId();
#endif
			return true;
		}
		return false;
	}

	Bool HawkSpinLock::Unlock()
	{
		HawkAssert(m_pSpinLock);

#ifdef _DEBUG
		m_bLocked = false;
		m_sFile   = "";
		m_iLine   = 0;
		m_iThread = 0;
#endif
		if (pthread_spin_unlock((pthread_spinlock_t*)m_pSpinLock) == HAWK_OK)
			return true;

		return false;
	}
}
