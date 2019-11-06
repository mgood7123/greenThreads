//
// Created by brothercomplex on 3/11/19.
//

#include <thread.h>
#include <println.h>

int tf(void * a) {
    return 1;
}

int main(void) {
    ExecutionManager * e = new ExecutionManager(); setExecutionManager(e);
    e->debug = true;
    /* Flush each printf() as it happens. */
    setvbuf(stdout, 0, _IOLBF, 0);
    setvbuf(stderr, 0, _IOLBF, 0);
    int a = 5;
    println("executing thread");
    Thread * x = threadNew(tf, &a);
    threadJoin(x);
    println("executed thread");
    return 0;
}
