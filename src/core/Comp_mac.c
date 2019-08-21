/**
 * Implement pthreads spin lock using OSSpinLock.
 */
#include "PosixThreads.h"

#include <Macros.h>

#include <errno.h>
#include <libkern/OSAtomic.h>

int pthread_spin_init(pthread_spinlock_t *lock, MARK_UNUSED int pshared) {
  *lock = OS_SPINLOCK_INIT;
  return 0;
}

int pthread_spin_lock(pthread_spinlock_t *lock) {
  OSSpinLockLock(lock);
  return 0;
}

int pthread_spin_trylock(pthread_spinlock_t *lock) {
  bool lock_taken = OSSpinLockTry(lock);
  return lock_taken ? 0 : EBUSY;
}

int pthread_spin_unlock(pthread_spinlock_t *lock) {
  OSSpinLockUnlock(lock);
  return 0;
}

int pthread_spin_destroy(MARK_UNUSED pthread_spinlock_t *lock) {
  /* NOP */
  return 0;
}