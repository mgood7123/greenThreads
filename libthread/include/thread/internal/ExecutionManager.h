//
// Created by brothercomplex on 28/10/19.
//

#ifndef THREAD_EXECUTIONMANAGER_H
#define THREAD_EXECUTIONMANAGER_H

#include <unistd.h>
#include <vector>
#include <cassert>

#include <stack/stack.h>
class Thread {
public:
    atom<long int> pid { -1 }; // process itself may be 0, init
    atom<Stack> stack { Stack() };
    atom<bool> died { false };
    atom<int> returnCode { 0 };
    atom<bool> suspend { false };
    atom<bool> killable { false};
    atom<int> status { 0 };
    atom<int> signal = -1;
    atom<bool> core_dumped { false };
    atom<bool> reaped { false };
    atom<bool> reapable { false };
    atom<bool> terminated_by_user { false };
    atom<bool> killed_by_user { false };
    const class {
    public:
        enum {
            UNDEFINED,
            RUNNING,
            STOPPED,
            DEAD,
            EXITED,
            KILLED_BY_SIGNAL
        };
    } statusList; // cannot be modified, no need to atomize
};

struct QTS {
    atom<bool> createSuspended { false };
    atom<size_t> stack_size { 0 };
    atom<int (*)(void*)> f { nullptr };
    atom<void *> arg { nullptr };
    atom<Thread *> t { nullptr };
    atom<bool> push { false };
};

class ExecutionManager {
    public:
        atom<bool> debug { false };
        atom<bool> close { false };

        atom<Thread *> thread { nullptr };
        atom<bool> isRunning { false };
        atom<std::vector<atom<struct QTS *>>> QTS_VECTOR;
        atom<bool> errorChecking(atom<struct QTS *> q);
        atom<void *> sendRequest(int request, atom<void *> package);
        void handleRequest(atom<class ExecutionManager *> EX);

        class REQUEST {
            public:
                atom<int> request { -1  };
                atom<void *> package { nullptr };
                atom<bool> upload { false };
                atom<bool> uploadNotComplete { false };
                atom<bool> processing { false };
                atom<bool> processingNotComplete { false };
                atom<void *> reply = { nullptr };
                atom<Stack> stack = { Stack() };
        };

        static const class {
            public:
                enum {
                    threadCreate,
                    threadJoin,
                    threadJoinUntilStopped,
                    threadWaitUntilStopped,
                    threadWaitUntilRunning,
                    threadWaitUntilRunningAndJoin,
                    threadWaitUntilRunningAndJoinUntilStopped,
                    threadPause,
                    threadResume,
                    threadResumeAndJoin,
                    threadTerminate,
                    threadKill
                };
        } REQUEST_ID;

        atom<REQUEST> REQUEST { REQUEST() };
        static void * sendRequest(atom<int> request, atom<void *> package);

        // threads
        atom<Thread *> threadNew(atom<size_t> stack_size, atom<int (*)(void*)> f, atom<void *> arg);
        atom<Thread *> threadNew(atom<int (*)(void*)> f, atom<void *> arg);
        atom<Thread *> threadNew(atom<size_t> stack_size, atom<void (*)()> f);
        atom<Thread *> threadNew(atom<void (*)()> f);
        atom<Thread *> threadNew(atom<bool> createSuspended, atom<size_t> stack_size, atom<int (*)(void*)> f, atom<void *> arg);
        atom<Thread *> threadNewSuspended(atom<size_t> stack_size, atom<int (*)(void*)> f, atom<void *> arg);
        atom<Thread *> threadNewSuspended(atom<int (*)(void*)> f, atom<void *> arg);
        atom<Thread *> threadNewSuspended(atom<size_t> stack_size, atom<void (*)()> f);
        atom<Thread *> threadNewSuspended(atom<void (*)()> f);
        static void threadJoin(atom<Thread *> t);
        static void threadJoinUntilStopped(atom<Thread *> t);
        static void threadWaitUntilStopped(atom<Thread *> t);
        static void threadWaitUntilRunning(atom<Thread *> t);
        static void threadWaitUntilRunningAndJoin(atom<Thread *> t);
        static void threadWaitUntilRunningAndJoinUntilStopped(atom<Thread *> t);
        static bool threadIsRunning(atom<Thread *> t);
        static bool threadIsStopped(atom<Thread *> t);
        static void threadPause(atom<Thread *> t);
        static void threadResume(atom<Thread *> t);
        static void threadResumeAndJoin(atom<Thread *> t);
        static void threadTerminate(atom<Thread *> t);
        static void threadKill(atom<Thread *> t);
};



void executionManager_Terminate();

atom<ExecutionManager *> executionManager_Current;

void executionManager_SetExecutionManager(atom<ExecutionManager *> executionManager);
atom<ExecutionManager *> executionManager_GetExecutionManager();

void setExecutionManager(atom<ExecutionManager *> executionManager);
atom<ExecutionManager *> getExecutionManager();

atom<Thread *> threadNew(atom<size_t> stack_size, atom<int (*)(void*)> f, atom<void *> arg);
atom<Thread *> threadNew(atom<int (*)(void*)> f, atom<void *> arg);
atom<Thread *> threadNew(atom<size_t> stack_size, atom<void (*)()> f);
atom<Thread *> threadNew(atom<void (*)()> f);
atom<Thread *> threadNew(atom<bool> createSuspended, atom<size_t> stack_size, atom<int (*)(void*)> f, atom<void *> arg);
atom<Thread *> threadNewSuspended(atom<size_t> stack_size, atom<int (*)(void*)> f, atom<void *> arg);
atom<Thread *> threadNewSuspended(atom<int (*)(void*)> f, atom<void *> arg);
atom<Thread *> threadNewSuspended(atom<size_t> stack_size, atom<void (*)()> f);
atom<Thread *> threadNewSuspended(atom<void (*)()> f);
void threadJoin(atom<Thread *> t);
void threadJoinUntilStopped(atom<Thread *> t);
void threadWaitUntilStopped(atom<Thread *> t);
void threadWaitUntilRunning(atom<Thread *> t);
void threadWaitUntilRunningAndJoin(atom<Thread *> t);
void threadWaitUntilRunningAndJoinUntilStopped(atom<Thread *> t);
bool threadIsRunning(atom<Thread *> t);
bool threadIsStopped(atom<Thread *> t);
void threadPause(atom<Thread *> t);
void threadResume(atom<Thread *> t);
void threadResumeAndJoin(atom<Thread *> t);
void threadInfo(atom<Thread *> t);
void threadTerminate(atom<Thread *> t);
void threadKill(atom<Thread *> t);

#endif //THREAD_EXECUTIONMANAGER_H
