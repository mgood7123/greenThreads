#include <unistd.h>
#include "../stack/stack.h"
class thread {
    public:
        pid_t pid = 0;
};

thread newThread(stack & s, int (*f)(void*), void * arg);
