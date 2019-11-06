#include<stdio.h>
#include <stdlib.h>
#include <unistd.h> // sleep
#include <sys/wait.h> // wait*
#include <sched.h> // clone
#include <thread> // std::this_thread
#include <chrono>
#include <iostream>
using namespace std::chrono_literals;


class Stack {
    public:
        char * stack = nullptr;
        char * top = nullptr; // points to top of stack
        size_t size = 0;
        int direction = 0;
        void alloc(size_t size);
        void free();
        class internal {
            public:
                int getStackDirection(int *addr);
        };
        int getStackDirection();
        char * getStackDirectionAsString();
};

class Thread {
    public:
        long int pid = -1; // process itself may be 0, init
        Stack stack = Stack();
        bool died = false;
        int returnCode = 0;
        bool suspend = false;
        bool killable = false;
        int status = 0;
        int signal = -1;
        bool core_dumped = false;
        bool reaped = false;
        bool reapable = false;
        class {
            public:
                enum {
                    UNDEFINED,
                    RUNNING,
                    STOPPED,
                    DEAD,
                    EXITED,
                    KILLED_BY_SIGNAL
                };
        } statusList;
};

int timer(void * arg);

long long int count = 0;

void printExecutionRate(long long int count);

void threadInfo(Thread * t);

void CALCRATE() {
    std::chrono::time_point<std::chrono::high_resolution_clock> t1 = std::chrono::high_resolution_clock::now();
    sleep(0);
    std::chrono::time_point<std::chrono::high_resolution_clock> t2 = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> duration = t2-t1;

    std::time_t now_c = std::chrono::high_resolution_clock::to_time_t(t1);

    std::cout << now_c << std::endl;

//    std::cout << "start = " << t1.rep << " end = " << t2.rep << "duration = " << duration.count() << std::endl;

}

int main(void) {
    /* Flush each printf() as it happens. */
    setvbuf(stdout, 0, _IOLBF, 0);
    setvbuf(stderr, 0, _IOLBF, 0);

    Thread * t = new Thread();
    t->stack.alloc(4096);
    t->pid = clone(timer, t->stack.top, CLONE_VM|SIGCHLD, nullptr);
    if (t->pid == -1) {
        perror("clone");
        t->stack.free();
        delete t;
        return -1;
    } else {
        puts("synchronizing thread");
        siginfo_t info;
        printf("waiting for pid %d\n", t->pid);
        if (waitid(P_PID, t->pid, &info, WSTOPPED) == -1) {
            perror("waitid");
            puts("failed to wait for thread");
            threadInfo(t);
            t->stack.free();
            delete t;
            return -1;
        }
        printf("waited for pid %d\n", t->pid);
        switch (info.si_code) {
            case CLD_KILLED:
                t->status = t->statusList.KILLED_BY_SIGNAL;
                t->signal = info.si_status;
                t->reapable = true;
                break;
            case CLD_DUMPED:
                t->status = t->statusList.KILLED_BY_SIGNAL;
                t->signal = info.si_status;
                t->core_dumped = true;
                t->reapable = true;
                break;
            case CLD_STOPPED:
                t->status = t->statusList.STOPPED;
                puts("synchronized thread");
                if (kill(t->pid, SIGCONT) == -1) perror("kill");
                siginfo_t info;
                if (waitid(P_PID, t->pid, &info, WCONTINUED) == -1) {
                    perror("waitid");
                    puts("failed to synchronize thread");
                    threadInfo(t);
                    t->stack.free();
                    delete t;
                    return -1;
                }
                switch (info.si_code) {
                    case CLD_CONTINUED:
                        t->status = t->statusList.RUNNING;
                        puts("sleep 0");
                        CALCRATE();
//                        puts("sleeping for 1 second");
//
//                        std::this_thread::sleep_for(1s);
//
//                        puts("slept for 1 second");
//                        printExecutionRate(count);
//                        printExecutionRate(count);
//                        printExecutionRate(count);
//                        printExecutionRate(count);
//                        printExecutionRate(count);
//                        printExecutionRate(count);
//                        printExecutionRate(count);
//                        printExecutionRate(count);
//                        printExecutionRate(count);
//                        printExecutionRate(count);
//                        printExecutionRate(count);
//                        printExecutionRate(count);
//                        printExecutionRate(count);
//                        printExecutionRate(count);
//                        printExecutionRate(count);
                        puts("killing thread");
                        kill(t->pid, SIGTERM);
                        siginfo_t info;
                        for (;;) {
                            if (waitid(P_PID, t->pid, &info, WEXITED | WSTOPPED) == -1) {
                                perror("waitid");
                                puts("failed to kill thread");
                                threadInfo(t);
                                t->stack.free();
                                delete t;
                                return -1;
                            }
                            bool con = false;
                            switch (info.si_code) {
                                case CLD_EXITED:
                                    t->status = t->statusList.EXITED;
                                    t->returnCode = info.si_status;
                                    t->reapable = true;
                                    puts("killed thread");
                                    break;
                                case CLD_KILLED:
                                    t->status = t->statusList.KILLED_BY_SIGNAL;
                                    t->signal = info.si_status;
                                    t->reapable = true;
                                    break;
                                case CLD_DUMPED:
                                    t->status = t->statusList.KILLED_BY_SIGNAL;
                                    t->signal = info.si_status;
                                    t->core_dumped = true;
                                    t->reapable = true;
                                    break;
                                case CLD_STOPPED:
                                    con = true;
                                    break;
                                default:
                                    t->status = t->statusList.UNDEFINED;
                                    break;
                            }
                            if (!con) break;
                        }
                        break;
                    default:
                        t->status = t->statusList.UNDEFINED;
                        break;
                }
                if (t->status != t->statusList.EXITED && !(t->status == t->statusList.KILLED_BY_SIGNAL && t->signal == SIGTERM)) {
                    puts("failed to kill thread");
                    threadInfo(t);
                    t->stack.free();
                    delete t;
                    return -1;
                }
                printExecutionRate(count);
                break;
            default:
                t->status = t->statusList.UNDEFINED;
                break;
        }
        if (!t->reapable && t->status != t->statusList.STOPPED) {
            puts("failed to synchronize thread");
            threadInfo(t);
            t->stack.free();
            delete t;
            return -1;
        }
        t->stack.free();
        delete t;
        return 0;
    }
}

int timer(void * arg) {
    kill(getpid(), SIGSTOP);
    while(true) count++;
}

void printExecutionRate(long long int count) {
    printf("executing at a rate of ");
    if (!(count < 0) && count != 0) {
        if (count >= 10) {
            if (count >= 100) {
                if (count >= 1000) {
                    if (count >= 10000) {
                        if (count >= 100000) {
                            if (count >= 1000000) {
                                if (count >= 10000000) {
                                    if (count >= 100000000) {
                                        if (count >= 1000000000) {
                                            if (count >= 10000000000) {
                                                if (count >= 100000000000) {
                                                    if (count >= 1000000000000) {
                                                        printf(" error: value is smaller than a picosecond (count is %lld)\n", count);
                                                    } else {
                                                        printf("%lld picosecond", count);
                                                        if (count != 100000000000) printf("s");
                                                        printf("\n");
                                                    }
                                                } else {
                                                    printf("%lld tenth", count);
                                                    if (count != 10000000000) printf("s");
                                                    printf(" of a picosecond\n");
                                                }
                                            } else {
                                                printf("%lld hundreth", count);
                                                if (count != 1000000000) printf("s");
                                                printf(" of a picosecond\n");
                                            }
                                        } else {
                                            printf("%lld nanosecond", count);
                                            if (count != 100000000) printf("s");
                                            printf("\n");
                                        }
                                    } else {
                                        printf("%lld tenth", count);
                                        if (count != 10000000) printf("s");
                                        printf(" of a nanosecond\n");
                                    }
                                } else {
                                    printf("%lld hundreth", count);
                                    if (count != 1000000) printf("s");
                                    printf(" of a nanosecond\n");
                                }
                            } else {
                                printf("%lld microsecond", count);
                                if (count != 100000) printf("s");
                                printf("\n");
                            }
                        } else {
                            printf("%lld tenth", count);
                            if (count != 10000) printf("s");
                            printf(" of a microsecond\n");
                        }
                    } else {
                        printf("%lld hundreth", count);
                        if (count != 1000) printf("s");
                        printf(" of a microsecond\n");
                    }
                } else {
                    printf("%lld every millisecond", count);
                    if (count != 100) printf("s");
                    printf("\n");
                }
            } else {
                printf("%lld tenth", count);
                if (count != 10) printf("s");
                printf(" of a millisecond\n");
            }
        } else {
            printf("%lld hundreth", count);
            if (count != 1) printf("s");
            printf(" of a millisecond\n");
        }
    } else {
        printf("%lld seconds\n", count);
    }
}

#define STACK_DIRECTION_UP 11
#define SDU STACK_DIRECTION_UP
#define STACK_DIRECTION_DOWN 12
#define SDD STACK_DIRECTION_DOWN

int Stack::internal::getStackDirection(int *addr) {
    int fun_local;
    if (addr < &fun_local) {
        printf("Stack grows upward\n");
        return SDU;
    }
    printf("Stack grows downward\n");
    return SDD;
}

int Stack::getStackDirection() {
    int main_local;
    return internal().getStackDirection(&main_local);
}

char * Stack::getStackDirectionAsString() {
    if (direction == SDU) return "Stack grows upwards";
    else if (direction == SDD) return "Stack grows downwards";
    else return "Stack grows in an unknown direction";
}

void Stack::alloc(size_t size) {
    stack = new char[size];
    size = size;
    direction = getStackDirection();
    if (direction == SDU) top = stack;
    else top = stack + (size*sizeof(char));;
}

void Stack::free() {
    if (stack == nullptr) return;
    delete[] stack;
    stack = nullptr;
    top = nullptr;
    size = 0;
    direction = 0;
}

void threadInfo(Thread * t) {
    printf("THREAD INFO:\n");
    bool threadExists = t != nullptr;
    printf("    thread exists = %s\n", threadExists ? "true" : "false");
    if (threadExists) {
        printf("    STACK INFO:\n");
        bool stackExists = t->stack.stack != nullptr;
        printf("        stack exists = %s\n", stackExists ? "true" : "false");
        if (stackExists) {
            printf("        address = %p\n", t->stack.stack);
            printf("        top = %p\n", t->stack.top);
            printf("        size = %ld\n", t->stack.size);
            printf("        direction = %s\n", t->stack.getStackDirectionAsString());
            printf("        size = %ld\n", t->stack);
        }
        printf("    pid = %d\n", t->pid);
        printf("    killable = %s\n", t->killable ? "true" : "false");
        printf("    initially suspended = %s\n", t->suspend ? "true" : "false");
        printf("    dead = %s\n", t->died ? "true" : "false");
        char * stat = nullptr;
        switch (t->status) {
            case t->statusList.UNDEFINED:
                stat = "UNDEFINED";
                break;
            case t->statusList.RUNNING:
                stat = "running";
                break;
            case t->statusList.STOPPED:
                stat = "stopped";
                break;
            case t->statusList.DEAD:
                stat = "dead";
                break;
            case t->statusList.EXITED:
                stat = "exited";
                break;
            case t->statusList.KILLED_BY_SIGNAL:
                stat = "killed by signal";
                break;
            default:
                stat = "unknown";
                break;
        }
        printf("    status = %s\n", stat);
        printf("    signal = %d\n", t->signal);
        printf("    returnCode = %d\n", t->returnCode);
        printf("    core dumped = %s\n", t->core_dumped ? "true" : "false");
        printf("    reapable = %s\n", t->reapable ? "true" : "false");
        printf("    reaped = %s\n", t->reaped ? "true" : "false");
    }
}
