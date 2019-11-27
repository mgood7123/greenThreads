//
// Created by brothercomplex on 15/11/19.
//

#include <thread_safe/thread_safe_lock.h>

void Lock::lock() {
    INTERNAL_SCOPED_LOCK THIS_PROTECTED_MUTEX (this);

    // do not lock if locked from the same thread
    std::thread::id id = std::this_thread::get_id();
    if (LockedFromSameThread(This_Lock_Locked_From_Thread_Id, id)) return;
    This_Lock.lock();
    This_Lock_Locked.store(true);
    This_Lock_Locked_From_Thread_Id.store(id);
}

void Lock::unlock() {
    INTERNAL_SCOPED_LOCK THIS_PROTECTED_MUTEX (this);

    // do not unlock if locked from the same thread
    std::thread::id id = std::this_thread::get_id();
    if (LockedFromSameThread(This_Lock_Locked_From_Thread_Id, id)) return;
    This_Lock.unlock();
    This_Lock_Locked.store(false);
    This_Lock_Locked_From_Thread_Id.store(std::nullopt);
}

const char * Lock::Exceptions::PermissionDenied::what() const noexcept {
    return "no access permission has been granted for a lock";
}