#include <unistd.h>
#include "../stack/stack.h"
class thread {
    public:
        pid_t pid = -1; // process itself may be 0, init
};

int gli = 0;

thread threadNew(stack & s, int (*f)(void*), void * arg);
thread threadNew(stack & s, void (*f)());
void threadJoin(thread & t);
void threadPause(thread & t);
void threadResume(thread & t);
