#ifndef THREAD_NEW
#define THREAD_NEW

#include <unistd.h>
#include "../stack/stack.h"
class Thread {
    public:
        long int pid = -1; // process itself may be 0, init
};

Thread threadNew(Stack & s, size_t stack_size, int (*f)(void*), void * arg);
Thread threadNew(Stack & s, int (*f)(void*), void * arg);
Thread threadNew(Stack & s, size_t stack_size, void (*f)());
Thread threadNew(Stack & s, void (*f)());
Thread threadNew(bool createSuspended, Stack & s, size_t stack_size, int (*f)(void*), void * arg);
Thread threadNewSuspended(Stack & s, size_t stack_size, int (*f)(void*), void * arg);
Thread threadNewSuspended(Stack & s, int (*f)(void*), void * arg);
Thread threadNewSuspended(Stack & s, size_t stack_size, void (*f)());
Thread threadNewSuspended(Stack & s, void (*f)());
void threadJoin(long int pid);
void threadJoin(long int pid, int & ret);
void threadJoinUntilStopped(long int pid);
void threadJoinUntilStopped(long int pid, int & ret);
void threadWaitUntilStopped(long int pid);
void threadWaitUntilStopped(long int pid, int & ret);
void threadPause(long int pid);
void threadResume(long int pid);

#endif