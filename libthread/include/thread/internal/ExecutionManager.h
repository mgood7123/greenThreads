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
    // TODO: function pointer template specilization: https://godbolt.org/z/h3kE3D
    atom<std::function<int(void*)>> f { nullptr };
    atomPointer<void *> arg { nullptr };
    atomPointer<Thread *> t { nullptr };
    atom<bool> push { false };
};

class ExecutionManager {
    public:
        atom<bool> debug { false };
        atom<bool> close { false };

        atomPointer<Thread *> thread { nullptr };
        atom<bool> isRunning { false };
        atom<std::vector<atomPointer<struct QTS *>>> QTS_VECTOR;
        atom<bool> errorChecking(atomPointer<struct QTS *> q);
        static atomPointer<void *> sendRequest(int request, atomPointer<void *> package);
        void handleRequest(atomPointer<ExecutionManager *> EX);

        class REQUEST {
            public:
                atom<int> request { -1  };
                atomPointer<void *> package { nullptr };
                atom<bool> upload { false };
                atom<bool> uploadNotComplete { false };
                atom<bool> processing { false };
                atom<bool> processingNotComplete { false };
                atomPointer<void *> reply = { nullptr };
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
        static void * sendRequest(atom<int> request, atomPointer<void *> package);

        // threads
        atomPointer<Thread *> threadNew(atom<size_t> stack_size, atom<std::function<int(void*)>> f, atomPointer<void *> arg);
        atomPointer<Thread *> threadNew(atom<std::function<int(void*)>> f, atomPointer<void *> arg);
        atomPointer<Thread *> threadNew(atom<size_t> stack_size, atom<std::function<void()>> f);
        atomPointer<Thread *> threadNew(atom<std::function<void()>> f);
        atomPointer<Thread *> threadNew(bool createSuspended, size_t stack_size, int (*f)(void*), void * arg);
        atomPointer<Thread *> threadNewSuspended(atom<size_t> stack_size, atom<std::function<int(void*)>> f, atomPointer<void *> arg);
        atomPointer<Thread *> threadNewSuspended(atom<std::function<int(void*)>> f, atomPointer<void *> arg);
        atomPointer<Thread *> threadNewSuspended(atom<size_t> stack_size, atom<std::function<void()>> f);
        atomPointer<Thread *> threadNewSuspended(atom<std::function<void()>> f);
        static void threadJoin(atomPointer<Thread *> t);
        static void threadJoinUntilStopped(atomPointer<Thread *> t);
        static void threadWaitUntilStopped(atomPointer<Thread *> t);
        static void threadWaitUntilRunning(atomPointer<Thread *> t);
        static void threadWaitUntilRunningAndJoin(atomPointer<Thread *> t);
        static void threadWaitUntilRunningAndJoinUntilStopped(atomPointer<Thread *> t);
        static bool threadIsRunning(atomPointer<Thread *> t);
        static bool threadIsStopped(atomPointer<Thread *> t);
        static void threadPause(atomPointer<Thread *> t);
        static void threadResume(atomPointer<Thread *> t);
        static void threadResumeAndJoin(atomPointer<Thread *> t);
        static void threadTerminate(atomPointer<Thread *> t);
        static void threadKill(atomPointer<Thread *> t);
};



void executionManager_Terminate();

atomPointer<ExecutionManager *> executionManager_Current;

void executionManager_SetExecutionManager(atomPointer<ExecutionManager *> executionManager);
atomPointer<ExecutionManager *> executionManager_GetExecutionManager();

void setExecutionManager(atomPointer<ExecutionManager *> executionManager);
atomPointer<ExecutionManager *> getExecutionManager();

atomPointer<Thread *> threadNew(atom<size_t> stack_size, atom<std::function<int(void*)>> f, atomPointer<void *> arg);
atomPointer<Thread *> threadNew(atom<std::function<int(void*)>> f, atomPointer<void *> arg);
atomPointer<Thread *> threadNew(atom<size_t> stack_size, atom<std::function<void()>> f);
atomPointer<Thread *> threadNew(atom<std::function<void()>> f);
atomPointer<Thread *> threadNew(atom<bool> createSuspended, atom<size_t> stack_size, atom<std::function<int(void*)>> f, atomPointer<void *> arg);
atomPointer<Thread *> threadNewSuspended(atom<size_t> stack_size, atom<std::function<int(void*)>> f, atomPointer<void *> arg);
atomPointer<Thread *> threadNewSuspended(atom<std::function<int(void*)>> f, atomPointer<void *> arg);
atomPointer<Thread *> threadNewSuspended(atom<size_t> stack_size, atom<std::function<void()>> f);
atomPointer<Thread *> threadNewSuspended(atom<std::function<void()>> f);
void threadJoin(atomPointer<Thread *> t);
void threadJoinUntilStopped(atomPointer<Thread *> t);
void threadWaitUntilStopped(atomPointer<Thread *> t);
void threadWaitUntilRunning(atomPointer<Thread *> t);
void threadWaitUntilRunningAndJoin(atomPointer<Thread *> t);
void threadWaitUntilRunningAndJoinUntilStopped(atomPointer<Thread *> t);
bool threadIsRunning(atomPointer<Thread *> t);
bool threadIsStopped(atomPointer<Thread *> t);
void threadPause(atomPointer<Thread *> t);
void threadResume(atomPointer<Thread *> t);
void threadResumeAndJoin(atomPointer<Thread *> t);
void threadInfo(atomPointer<Thread *> t);
void threadTerminate(atomPointer<Thread *> t);
void threadKill(atomPointer<Thread *> t);

#endif //THREAD_EXECUTIONMANAGER_H
