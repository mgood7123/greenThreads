#include "new.h"
#include "functionPointer.h"
#define _GNU_SOURCE
#include <sched.h>

#include <sys/types.h>
#include <signal.h>

#include <sys/wait.h>

void threadJoin(thread & t) {
    if (t.pid != -1) if (waitpid(t.pid, NULL, __WCLONE) == -1) perror("waitpid");
}

void threadPause(thread & t) {
    if (t.pid != -1) if (kill(t.pid, SIGSTOP) == -1) perror("kill");
}

void threadResume(thread & t) {
    if (t.pid != -1) if (kill(t.pid, SIGCONT) == -1) perror("kill");
}

thread threadNew(stack & s, int (*f)(void*), void * arg) {
    thread t = thread();
    s = stack();
    s.alloc(4096);
    printf("gli before = %d\n", gli);
    t.pid = clone(f, s.top, CLONE_VM, arg);
    // pause the thread as soon as possible
    threadPause(t);
    if (t.pid == -1) {
        perror("clone");
        s.free();
    }
    printf("pid: %d\n", t.pid);
    threadResume(t);
    printf("waiting for pid: %d\n", t.pid);
    threadJoin(t);
    printf("child has terminated\n");
    printf("gli after = %d\n", gli);
    return t;
}

int helperVoid(void * v) {
    functionPointerDeclareAndAssign0(void, f, v);
    f();
    return 0;
}

thread threadNew(stack & s, void (*f)()) {
    thread t = thread();
    s = stack();
    s.alloc(4096);
    t.pid = clone(helperVoid, s.top, 0, reinterpret_cast<void*>(f));
    if (t.pid == -1) {
        perror("clone");
        s.free();
        t.pid = 0;
    }
    printf("pid: %d\n", t.pid);
    printf("sending SIGSTOP\n");
    if (kill(t.pid, SIGSTOP) == -1) {
        perror("kill");
    }
    return t;
}
