#include <thread.h>
#include <string.h>
#include <println.h>

int tf(void * a) {
    return 1;
}

int main(void) {
    ExecutionManager * e = new ExecutionManager();
    e->debug = true;
    setExecutionManager(e);
    /* Flush each printf() as it happens. */
    setvbuf(stdout, 0, _IOLBF, 0);
    setvbuf(stderr, 0, _IOLBF, 0);
    int a = 5;
    println("executing 10 threads");
    for (int i = 1; i < 11; i++) {
        println("creating thread %d", i);
        Thread * x = threadNew(tf, &i);
        threadJoin(x);
        println("executed thread %d, returned %d", i, x->returnCode);
    }
    println("executed 10 threads");
    return 0;
}
