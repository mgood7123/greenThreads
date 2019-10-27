#include <stdlib.h>

class stack {
    public:
        char * stack = nullptr;
        char * top = nullptr; // points to top of stack
        size_t size = 0;
        int direction = 0;
        void alloc(size_t size);
        void free();
};
