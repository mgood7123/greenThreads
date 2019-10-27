#include "new.h"
#include "functionPointer.h"
#define _GNU_SOURCE
#include <sched.h>

#include <sys/types.h>
#include <signal.h>

#include <sys/wait.h>

void threadJoin(long int pid) {
    if (pid != -1) if (waitid(P_PID, pid, NULL, __WCLONE) == -1) perror("waitpid");
}

void threadJoin(long int pid, int & ret) {
    if (pid != -1) {
        siginfo_t info;
        if (waitid(P_PID, pid, &info, __WCLONE|WEXITED) == -1) {
            perror("waitpid");
            ret = -1;
        } else {
            ret = info.si_status;
        }
    }
}

void threadJoinUntilStopped(long int pid) {
    if (pid != -1) if (waitid(P_PID, pid, NULL, __WCLONE|WEXITED|WSTOPPED) == -1) perror("waitpid");
};

void threadJoinUntilStopped(long int pid, int & ret) {
    if (pid != -1) {
        siginfo_t info;
        if (waitid(P_PID, pid, &info, __WCLONE|WEXITED|WSTOPPED) == -1) {
            perror("waitpid");
            ret = -1;
        } else {
            ret = info.si_status;
        }
    }
};

void threadPause(long int pid) {
    if (pid != -1) if (kill(pid, SIGSTOP) == -1) perror("kill");
}

void threadResume(long int pid) {
    if (pid != -1) if (kill(pid, SIGCONT) == -1) perror("kill");
}

int helperVoid(void * v) {
    functionPointerDeclareAndAssign0(void, f, v);
    f();
    return 0;
}

int suspended(void * arg) {
    kill(getpid(), SIGSTOP);
    struct AB {
        int (*f)(void*);
        void * arg;
        bool suspended;
    } * x = static_cast<AB *>(arg);
    int ret = x->f(x->arg);
    delete x;
    x = nullptr;
    return ret;
}

Thread threadNew(bool createSuspended, Stack & s, size_t stack_size, int (*f)(void*), void * arg) {
    Thread t = Thread();
    s = Stack();
    s.alloc(stack_size == 0 ? 4096 : stack_size);
    struct AB {
        int (*f)(void*);
        void * arg;
        bool suspended;
    } * x = new struct AB;
    x->f = f;
    x->arg = arg;
    x->suspended = createSuspended;
    t.pid = clone(suspended, s.top, CLONE_VM, x);
    if (t.pid == -1) {
        perror("clone");
        s.free();
    } else {
        if (waitid(P_PID, t.pid, NULL, __WCLONE|WSTOPPED) == -1) perror("waitpid");
        if (!x->suspended) if (kill(t.pid, SIGCONT) == -1) perror("kill");
        printf("pid: %d\n", t.pid);
    }
    return t;
}

Thread threadNew(Stack & s, int (*f)(void*), void * arg) {
    return threadNew(false, s, 0, f, arg);
}

Thread threadNew(Stack & s, size_t stack_size, int (*f)(void*), void * arg) {
    return threadNew(false, s, stack_size, f, arg);
}

Thread threadNew(Stack & s, size_t stack_size, void (*f)()) {
    return threadNew(s, stack_size, helperVoid, reinterpret_cast<void*>(f));
}

Thread threadNew(Stack & s, void (*f)()) {
    return threadNew(s, 0, helperVoid, reinterpret_cast<void*>(f));
}

Thread threadNewSuspended(Stack & s, int (*f)(void*), void * arg) {
    return threadNew(false, s, 0, f, arg);
}

Thread threadNewSuspended(Stack & s, size_t stack_size, int (*f)(void*), void * arg) {
    return threadNew(false, s, stack_size, f, arg);
}

Thread threadNewSuspended(Stack & s, size_t stack_size, void (*f)()) {
    return threadNew(s, stack_size, helperVoid, reinterpret_cast<void*>(f));
}

Thread threadNewSuspended(Stack & s, void (*f)()) {
    return threadNew(s, 0, helperVoid, reinterpret_cast<void*>(f));
}
