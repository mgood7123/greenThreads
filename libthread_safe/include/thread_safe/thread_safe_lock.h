//
// Created by brothercomplex on 15/11/19.
//

#ifndef FOREGROUND_SERVICE_THREAD_SAFE_LOCK_H
#define FOREGROUND_SERVICE_THREAD_SAFE_LOCK_H

#include <mutex>         // std::mutex
#include <iostream>      // std::std::cout
#include <algorithm>     // std::reverse
#include <vector>        // std::vector
#include <unordered_set> // std::unordered_set
#include <atomic>        // std::atomic
#include <optional>      // std::optional
#include <thread>        // std::this_thread
#include <exception>     // std::exception

template <typename T>
bool vectorContainsElement(std::vector<T> & v, T element) {
    for (int i = 0; i < v.size(); i++) {
        if (v[i] == element) return true;
    }
    return false;
}

template <typename T>
void vectorRemoveElements(std::vector<T> & v, T element) {
    // remove all elements matching element
    for (int i = 0; i < v.size(); i++) {
        if (v[i] == element) v.erase(v.begin() + i);
    }
}

template <typename T>
void vectorRemoveElement(std::vector<T> & v, T element) {
    // remove the first element matching element
    for (int i = 0; i < v.size(); i++) {
        if (v[i] == element) {
            v.erase(v.begin() + i);
            break;
        }
    }
}

template <typename T>
std::vector<T> vectorSort(std::vector<T> & v1, std::vector<T> & v2) {
    // return a copy of the vector, sorted
    std::vector<T> a;
    for (T element : v1) a.emplace_back(element);
    for (T element : v2) a.emplace_back(element);
    std::sort( a.begin(), a.end() );
    return a;
}

template <typename T>
void vectorRemoveMatching(std::vector<T> & what, std::vector<T> & from) {
    for (T element : what) vectorRemoveElements(from, element);
}

template <typename T>
std::vector<T> vectorUnique(std::vector<T> & v1) {
    // return a copy of the vector, sorted
    std::vector<T> a;
    for (T element : v1) a.emplace_back(element);
    std::unordered_set<std::mutex *> s;
    for (T i : v1) s.insert(i);
    a.assign( s.begin(), s.end() );
    std::sort( a.begin(), a.end() );
    return a;
}

template <typename T>
std::vector<T> vectorReverse(std::vector<T> & v) {
    // return a copy of the vector, reversed
    std::vector<T> a;
    a.reserve(v.size());
    for (T element : v) a.emplace_back(element);
    std::cout << "reversing vector" << std::endl;
    std::reverse(a.begin(), a.end());
    std::cout << "reversed vector" << std::endl;
    return a;
}

template <typename T>
std::vector<T> vectorReverse(std::unordered_set<T> & v) {
    // return a set in the form of a vector, reversed
    std::vector<T> a;
    a.reserve(v.size());
    for (T element : v) a.emplace_back(element);
    std::cout << "reversing vector" << std::endl;
    std::reverse(a.begin(), a.end());
    std::cout << "reversed vector" << std::endl;
    return a;
}

template <class T, size_t N, class Op, class... Ts>
auto vectorUnpack(size_t i, size_t n, array<T, N> a, Op&& op, Ts&... ts) {
    if (sizeof...(ts) < N) {
        if (i < n) {
            return vectorUnpack(i + 1, n, a, std::forward<Op>(op), ts..., a[i].get());
        }
        else {
            return op(ts...);
        }
    }
    else {
        return op(ts...);
    }
}

template <typename... Ts>
void PERAM(Ts&... ts) {
    std::array pack = {std::ref(ts)...};
    size_t n = 0;
    for (size_t i = 0, e = pack.size(); i < e; ++i) {
        if (pack[i] != 2) {
            pack[n++] = pack[i];
        }
    }
    vectorUnpack(0, n, pack, [&](auto&... args) { return PERAM(args...); } );
}

/*
#include <array>
#include <tuple>
#include <utility>
#include <vector>
#include <iostream>

template <typename... Ts>
void PERAM_(Ts&... ts) {
    ((std::cout << ts << "\n"), ...);
}

template <class T, size_t N, class Op, class... Ts>
auto unpack(size_t i, size_t n, std::array<T, N> a, Op&& op, Ts&... ts) {
    if constexpr (sizeof...(ts) < N) {
        if (i < n) {
            return unpack(i + 1, n, a, std::forward<Op>(op), ts..., a[i].get());
        }
        else {
            return op(ts...);
        }
    }
    else {
        return op(ts...);
    }
}

template <typename... Ts>
void PERAM(Ts&... ts) {
    std::array pack = {std::ref(ts)...};
    size_t n = 0;
    for (size_t i = 0, e = pack.size(); i < e; ++i) {
        if (pack[i] != 2) {
            pack[n++] = pack[i];
        }
    }
    unpack(0, n, pack, [&](auto&... args) { return PERAM_(args...); });
}

int a = 1, b = 2, c = 3, d = 4, e = 5, f = 6;

int main() {
    PERAM(a,b,c,d,e,f);
}
 */

/*
 * declaring this as mutable ensures it can still be locked if declared const:
 *
 * mutable Lock variable;
 */

struct Lock {

    std::mutex This_Lock, THIS_INSTANCE_PROTECTED;
    std::atomic_bool This_Lock_Locked { false }, THIS_INSTANCE_PROTECTED_LOCKED { false };
    std::atomic<std::optional<std::thread::id>>
            This_Lock_Locked_From_Thread_Id {std::nullopt},
            THIS_INSTANCE_PROTECTED_LOCKED_FROM_THREAD_ID {std::nullopt};
    std::vector<Lock *> Shared_Locks;
    std::vector<void *> User_Data;

    static bool LockedFromSameThread(std::atomic<std::optional<std::thread::id>> & whichThread, std::thread::id & id) {
        std::optional<std::thread::id> id_opt = whichThread.load();
        if (id_opt.has_value) {
            if (id_opt == id) return true;
        }
        return false;
    }

    class INTERNAL_SCOPED_LOCK {
        Lock * lock;
        bool single = false;
    public:
        INTERNAL_SCOPED_LOCK (Lock * LOCK) {
            lock = LOCK;
            // do not lock if locked from the same thread
            std::thread::id id = std::this_thread::get_id();
            if (LockedFromSameThread(lock->THIS_INSTANCE_PROTECTED_LOCKED_FROM_THREAD_ID, id)) return;
            lock->THIS_INSTANCE_PROTECTED.lock();
            lock->THIS_INSTANCE_PROTECTED_LOCKED.store(true);
            lock->THIS_INSTANCE_PROTECTED_LOCKED_FROM_THREAD_ID.store(id);
        }
        ~INTERNAL_SCOPED_LOCK() {
            // do not unlock if locked from the same thread
            std::thread::id id = std::this_thread::get_id();
            if (LockedFromSameThread(lock->THIS_INSTANCE_PROTECTED_LOCKED_FROM_THREAD_ID, id)) return;
            lock->THIS_INSTANCE_PROTECTED.unlock();
            lock->THIS_INSTANCE_PROTECTED_LOCKED.store(false);
            lock->THIS_INSTANCE_PROTECTED_LOCKED_FROM_THREAD_ID.store(std::nullopt);
        }
    };

    void lock();
    void unlock();

    // http://eel.is/c++draft/atomics
    // http://eel.is/c++draft/intro.multithread
    // http://eel.is/c++draft/thread.lock.algorithm
    // http://eel.is/c++draft/thread.mutex#requirements.mutex

template <typename... types> // this function expects Lock type
void lock(Lock & LOCK, types & ... LOCK_) {
    INTERNAL_SCOPED_LOCK THIS_PROTECTED_MUTEX (this);
    std::lock(This_Lock, LOCK.This_Lock, LOCK_.This_Lock ...);
    std::vector<Lock*> u = { this, &LOCK, &LOCK_...};
    for (Lock *U : u) U->This_Lock_Locked.store(true);
}

    struct Exceptions {
        struct PermissionDenied : public std::exception {
            const char *what() const noexcept;
        };
    };

    // denies access to a lock if permission has not been granted to it, throwing Lock::Exceptions::PermissionDenied
    template <typename... types> // this function expects Lock type
    void secure_lock(Lock & LOCK, types & ... LOCK_) {
        INTERNAL_SCOPED_LOCK THIS_PROTECTED_MUTEX (this);
        std::vector<Lock*> u = { &LOCK, &LOCK_...};
        for (Lock *U : u) if (!vectorContainsElement(U->Shared_Locks, this)) throw Lock::Exceptions::PermissionDenied();
        std::lock(This_Lock, LOCK.This_Lock, LOCK_.This_Lock ...);
        u.emplace_back(this);
        for (Lock *U : u) U->This_Lock_Locked.store(true);
    }

    template <typename... types> // this function expects Lock type
    void unlock(Lock & LOCK, types & ... LOCK_) {
        INTERNAL_SCOPED_LOCK THIS_PROTECTED_MUTEX (this);
        std::vector<Lock*> u = { this, &LOCK, &LOCK_...};
        for (Lock *U : u) {
            U->This_Lock.unlock();
            U->This_Lock_Locked.store(false);
        }
    }

    // denies access to a lock if permission has not been granted to it, throwing Lock::Exceptions::PermissionDenied
    template <typename... types> // this function expects Lock type
    void secure_unlock(Lock & LOCK, types & ... LOCK_) {
        INTERNAL_SCOPED_LOCK THIS_PROTECTED_MUTEX (this);
        std::vector<Lock*> u = { &LOCK, &LOCK_...};
        for (Lock *U : u) if (!vectorContainsElement(U->Shared_Locks, this)) throw Lock::Exceptions::PermissionDenied();
        u.emplace_back(this);
        for (Lock *U : u) {
            U->This_Lock.unlock();
            U->This_Lock_Locked.store(false);
        }
    }

    template <typename... types> // this function expects Lock type
    class SCOPED_LOCK {
        std::vector<Lock *> locks;
        bool single = false;
    public:
        SCOPED_LOCK (Lock * LOCK) {
            // do not lock if locked from the same thread
            std::thread::id id = std::this_thread::get_id();
            if (LockedFromSameThread(LOCK->This_Lock_Locked_From_Thread_Id, id)) return;
            single = true;
            locks = { LOCK };
            LOCK->This_Lock.lock();
            LOCK->This_Lock_Locked.store(true);
            LOCK->This_Lock_Locked_From_Thread_Id.store(id);
        }
        SCOPED_LOCK (Lock * LOCK, types * ... LOCK_) {
            locks = { LOCK, LOCK_... };
            std::lock(LOCK->This_Lock, LOCK_->This_Lock ...);
            for (Lock *U : locks) U->This_Lock_Locked.store(true);
        }
        SCOPED_LOCK (Lock * LOCK, types & ... LOCK_) {
            SCOPED_LOCK(LOCK, &LOCK_...);
        }
        ~SCOPED_LOCK() {
            if (single) {
                // do not unlock if locked from the same thread
                std::thread::id id = std::this_thread::get_id();
                if (LockedFromSameThread(locks[0]->This_Lock_Locked_From_Thread_Id, id)) return;
                locks[0]->This_Lock.unlock();
                locks[0]->This_Lock_Locked.store(false);
                locks[0]->This_Lock_Locked_From_Thread_Id.store(std::nullopt);
            } else for (Lock *U : locks) {
                U->This_Lock.unlock();
                U->This_Lock_Locked.store(false);
                U->This_Lock_Locked_From_Thread_Id.store(std::nullopt);
            }
        }
    };

    // denies access to a lock if permission has not been granted to it, throwing Lock::Exceptions::PermissionDenied
    template <typename... types> // this function expects Lock type
    class SECURE_SCOPED_LOCK {
        std::vector<Lock *> locks;
    public:
        SECURE_SCOPED_LOCK (Lock * LOCK, types * ... LOCK_) {
            locks = { LOCK, LOCK_... };
            vectorRemoveElement(locks, this);
            for (Lock *U : locks) if (!vectorContainsElement(U->Shared_Locks, this)) throw Lock::Exceptions::PermissionDenied();
            locks.emplace_back(this);
            std::lock(LOCK->This_Lock, LOCK_->This_Lock ...);
            for (Lock *U : locks) U->This_Lock_Locked.store(true);
        }
        SECURE_SCOPED_LOCK (Lock * LOCK, types & ... LOCK_) {
            SECURE_SCOPED_LOCK(LOCK, &LOCK_...);
        }
        ~SECURE_SCOPED_LOCK() {
            vectorRemoveElement(locks, this);
            for (Lock *U : locks) if (!vectorContainsElement(U->Shared_Locks, this)) throw Lock::Exceptions::PermissionDenied();
            locks.emplace_back(this);
            for (Lock *U : locks) {
                U->This_Lock.unlock();
                U->This_Lock_Locked.store(false);
            }
        }
    };

    template <typename... types> // this function expects Lock type
    SCOPED_LOCK<Lock> scoped_lock(Lock * LOCK, types * ... LOCK_) {
        INTERNAL_SCOPED_LOCK THIS_PROTECTED_MUTEX (this);
        return SCOPED_LOCK<Lock>(this, LOCK, LOCK_...);
    }

    template <typename... types> // this function expects Lock type
    SCOPED_LOCK<Lock> scoped_lock(Lock & LOCK, types & ... LOCK_) {
        return scoped_lock(&LOCK, &LOCK_...);
    }

    template <typename... types> // this function expects Lock type
    SCOPED_LOCK<Lock> scoped_lock() {
        INTERNAL_SCOPED_LOCK THIS_PROTECTED_MUTEX (this);
        return SCOPED_LOCK<Lock>(this);
    }

    // denies access to a lock if permission has not been granted to it, throwing Lock::Exceptions::PermissionDenied
    template <typename... types> // this function expects Lock type
    SECURE_SCOPED_LOCK<Lock> secure_scoped_lock(Lock * LOCK, types * ... LOCK_) {
        INTERNAL_SCOPED_LOCK THIS_PROTECTED_MUTEX (this);
        return SECURE_SCOPED_LOCK<Lock>(this, LOCK, LOCK_...);
    }

    // denies access to a lock if permission has not been granted to it, throwing Lock::Exceptions::PermissionDenied
    template <typename... types> // this function expects Lock type
    SECURE_SCOPED_LOCK<Lock> secure_scoped_lock(Lock & LOCK, types & ... LOCK_) {
        return secure_scoped_lock(&LOCK, &LOCK_...);
    }

    template <typename... types> // this function expects Lock type
    void grant_access_to(Lock & LOCK, types & ... LOCK_) {
        INTERNAL_SCOPED_LOCK THIS_PROTECTED_MUTEX (this);
        std::vector<Lock*> u = { &LOCK, &LOCK_...};
        for (Lock *U : u) U->Shared_Locks.emplace_back(this);
        Shared_Locks.emplace_back(&LOCK, &LOCK_ ...);
    }

    template <typename... types> // this function expects Lock type
    void revoke_access_to(Lock & LOCK, types & ... LOCK_) {
        INTERNAL_SCOPED_LOCK THIS_PROTECTED_MUTEX (this);
        std::vector<Lock*> u = { &LOCK, &LOCK_...};
        for (Lock *U : u) {
            vectorRemoveElement(U->Shared_Locks, this);
            vectorRemoveElement(Shared_Locks, U);
        }
    }
};

#endif //FOREGROUND_SERVICE_THREAD_SAFE_LOCK_H