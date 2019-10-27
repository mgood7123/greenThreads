#include "threadNew.h"
#define _GNU_SOURCE
#include <sched.h>

thread newThread(stack & s, int (*f)(void*), void * arg) {
    thread t = thread();
    s = stack();
    s.alloc(4096);
    t.pid = clone(f, s.top, 0, arg);
    if (t.pid == -1) {
        perror("clone");
        s.free();
        t.pid = 0;
    }
    return t;
}
