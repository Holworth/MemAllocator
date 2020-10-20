#include "atomic_lock.h"
#include <assert.h>

inline void atomic_spin_init(atomic_lock_t *lock) {
    *lock = 0;
}

inline void atomic_spin_lock(atomic_lock_t *lock) {
    while(__sync_val_compare_and_swap(lock, 0, 1) == 1)
        ;
}

inline void atomic_spin_unlock(atomic_lock_t *lock) {
    assert(__sync_val_compare_and_swap(lock, 1, 0) == 1);
}