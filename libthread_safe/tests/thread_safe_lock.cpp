//
// Created by brothercomplex on 11/11/19.
//

#include <thread_safe/thread_safe.h>
#include <thread_safe/thread_safe_lock.h>
#include <gtest/gtest.h>
#include <thread>
#include <pthread.h>

/*
//#define THREADSAFEANDROIDARCH __ANDROID__

#if defined(THREADSAFEANDROIDARCH)
#warning certain tests are not yet supported on Android due to pthread_cancel being unimplemented
#warning these tests, when compiled on Android, are prepended with UNSUPPORTED_ON_ANDROID_ and will blindly succeed
#endif

extern int pthread_cancel(pthread_t thread); // for Android Studio

#if defined(THREADSAFEANDROIDARCH)
#define UNSUPPORTED(name) UNSUPPORTED_ON_ANDROID_##name
    #define UNSUPPORTED_CODE(WHAT_IF_NOT_SUPPORTED, code) WHAT_IF_NOT_SUPPORTED
#else
#define UNSUPPORTED(name) name
#define UNSUPPORTED_CODE(WHAT_IF_NOT_SUPPORTED, code) code
#endif
*/

class Thread_Safe_Locker_Core : public ::testing::Test {
protected:
    Lock a, b, c;
public:
    struct x {
        Lock a, b;
        bool exited_a = false, exited_b = false;
    } x;
    static void * deadlock_test_1_a(void * a) {
        std::cout << "deadlock A start" << std::endl;
        struct x * x = reinterpret_cast<struct x *>(a);
        std::cout << "lock AAA" << std::endl;
        auto AAA = x->a.scoped_lock();
        sleep(2); // x->b.lock(); sleep 2
        std::cout << "lock BBB" << std::endl;
        auto BBB = x->b.scoped_lock();
        std::cout << "deadlock A end" << std::endl;
        x->exited_a = true; // this should not be reached
        return (void*) 0;
    }
    static void * deadlock_test_1_b(void * a) {
        std::cout << "deadlock B start" << std::endl;
        struct x * x = reinterpret_cast<struct x *>(a);
        sleep(1);
        std::cout << "lock BBB" << std::endl;
        auto BBB = x->b.scoped_lock();
        sleep(2);
        std::cout << "lock AAA" << std::endl;
        auto AAA = x->a.scoped_lock();
        std::cout << "deadlock B end" << std::endl;
        x->exited_b = true; // this should not be reached
        return (void*) 0;
    }
};

TEST_F(Thread_Safe_Locker_Core, instance) {
    ASSERT_EQ(a.Locked.load(), false);
    ASSERT_EQ(b.Locked.load(), false);
    ASSERT_EQ(c.Locked.load(), false);
}

TEST_F(Thread_Safe_Locker_Core, lock) {
    a.lock();
    ASSERT_EQ(a.Locked.load(), true);
}

TEST_F(Thread_Safe_Locker_Core, unlock) {
    a.lock();
    a.unlock();
    ASSERT_EQ(a.Locked.load(), false);
}

TEST_F(Thread_Safe_Locker_Core, Multi_Lock) {
    a.lock(b);
    ASSERT_EQ(a.Locked.load(), true);
    ASSERT_EQ(b.Locked.load(), true);
    a.unlock(b);
    ASSERT_EQ(a.Locked.load(), false);
    ASSERT_EQ(b.Locked.load(), false);
}

TEST_F(Thread_Safe_Locker_Core, Shared_Access) {
    ASSERT_EQ(a.Shared_Locks.size(), 0);
    ASSERT_EQ(b.Shared_Locks.size(), 0);
    a.grant_access_to(b);
    ASSERT_EQ(a.Shared_Locks.size(), 1);
    ASSERT_EQ(b.Shared_Locks.size(), 1);
    ASSERT_EQ(a.Shared_Locks[0], &b);
    ASSERT_EQ(b.Shared_Locks[0], &a);
    a.revoke_access_to(b);
    ASSERT_EQ(a.Shared_Locks.size(), 0);
    ASSERT_EQ(b.Shared_Locks.size(), 0);
}

TEST_F(Thread_Safe_Locker_Core, Secure_Lock_Without_Access) {
    ASSERT_THROW(a.secure_lock(b), Lock::Exceptions::PermissionDenied);
}

TEST_F(Thread_Safe_Locker_Core, Secure_Lock_With_Access) {
    a.grant_access_to(b);
    ASSERT_NO_THROW(a.secure_lock(b));
}

TEST_F(Thread_Safe_Locker_Core, Secure_Unlock_Without_Access) {
    ASSERT_THROW(a.secure_lock(b), Lock::Exceptions::PermissionDenied);
    ASSERT_THROW(a.secure_unlock(b), Lock::Exceptions::PermissionDenied);
}

TEST_F(Thread_Safe_Locker_Core, Secure_Unlock_With_Access) {
    a.grant_access_to(b);
    ASSERT_NO_THROW(a.secure_lock(b));
    ASSERT_NO_THROW(a.secure_unlock(b));
}

TEST_F(Thread_Safe_Locker_Core, Scoped_Lock) {
    // scope 1
    ASSERT_EQ(a.Locked.load(), false);
    {
        // scope 2
        ASSERT_EQ(a.Locked.load(), false);
        Lock::SCOPED_LOCK guard = a.scoped_lock();
        ASSERT_EQ(a.Locked.load(), true);
    }
    ASSERT_EQ(a.Locked.load(), false);
    // scope 1
}