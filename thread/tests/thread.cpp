#include <thread.h>
#include <string.h>

int tf(void * a) {
    int saved;
    memcpy(&saved, a, sizeof(int));
    puts("test thread");
    printf("a = %p\n", a);
    int * b = static_cast<int*>(a);
    printf("*a = %d\n", *b);
    printf("saved = %d\n", saved);
    printf("setting gli from %d to %d\n", gli, *b);
    gli = *b;
    printf("exiting %d\n", *b);
    printf("exiting (saved) %d\n", saved);
    return 0;
}

void threadMonitor() {
    puts("hi");
}

int main(void) {
    stack s = stack();
    int a = 5;
    puts("executing thread");
    threadNew(s, tf, &a);
    puts("executed thread");
//     puts("attempting to start thread monitor");
//     newThread(s, threadMonitor);
    puts("executing 10 threads");
    for (int i = 1; i < 11; i++) {
        auto v = stack();
        threadNew(v, tf, &i);
    }
    puts("executed 10 threads");
    return 0;
}
