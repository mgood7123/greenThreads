#include "stack.h"
int main(void) {
    stack s = stack();
    s.alloc(100);
    s.free();
    return 0;
}
