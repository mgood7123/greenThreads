#include <thread.h>

int tf(void * a) {
    puts("test thread");
    printf("a = %p\n", a);
    int * b = static_cast<int*>(a);
    printf("*a = %d\n", *b);
    return 0;
}

int main(void) {
    stack s = stack();
    int a = 5;
    puts("executing thread");
    newThread(s, tf, &a);
    puts("executed thread");
    return 0;
}
