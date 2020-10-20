#ifndef ATOMIC_LOCK_H_
#define ATOMIC_LOCK_H_

typedef int atomic_lock_t;
void atomic_spin_init(atomic_lock_t *lock);
void atomic_spin_lock(atomic_lock_t *lock);
void atomic_spin_unlock(atomic_lock_t *lock);

#endif