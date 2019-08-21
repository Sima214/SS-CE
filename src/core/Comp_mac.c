/**
 * Implement pthreads spin lock using OSSpinLock.
 */
#include "PosixThreads.h"

#include <Macros.h>

#include <errno.h>
#include <os/lock.h>

int pthread_spin_init(pthread_spinlock_t *lock, MARK_UNUSED int pshared) {
  *lock = OS_UNFAIR_LOCK_INIT;
  return 0;
}

int pthread_spin_lock(pthread_spinlock_t *lock) {
  os_unfair_lock_lock(lock);
  return 0;
}

int pthread_spin_trylock(pthread_spinlock_t *lock) {
  bool lock_taken = os_unfair_lock_trylock(lock);
  return lock_taken ? 0 : EBUSY;
}

int pthread_spin_unlock(pthread_spinlock_t *lock) {
  os_unfair_lock_unlock(lock);
  return 0;
}

int pthread_spin_destroy(MARK_UNUSED pthread_spinlock_t *lock) {
  /* NOP */
  return 0;
}