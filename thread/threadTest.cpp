#include "thread.h"

int tf(void * a) {
    printf("a = %p\n", a);
    int * b = static_cast<int*>(a);
    printf("*a = %d\n", *b);
    return 0;
}

int main(void) {
    stack s = stack();
    int a = 5;
    newThread(s, tf, &a);
    return 0;
}
