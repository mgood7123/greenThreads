/*

// NOTE: users should use threadNew to create new threads, everything else in this will be internal

// TODO: implement tid lookup and return obtain
// NOTE: this will be tricky as the thread needs to be recycled when its return code is obtained
// if another thread starts with the same id due to pid recycling, and the previous thread return code has not been obtained
// then this is UB, it is unclear how this could be worked around given intentional abuse of pid recycling

// we cannot manually obtain the return code via the table index as the system releases all resources upon thread completion
*/

//
// Created by brothercomplex on 28/10/19.
//

#include <thread/internal/ExecutionManager.h>

#include <println.h>

//#define println(format, ...) printf(format "\n", __VA_ARGS__)
#include <sys/wait.h>
#include <stdio.h>
#include <sys/types.h>
#include <signal.h>
#include <sched.h>
#include <println.h>
#include <thread/internal/ExecutionManager.h>

int ExecutionManager_instance(void * arg);

void executionManager_SetExecutionManager(atomPointer<class ExecutionManager *> executionManager) {
    executionManager_Current.store(executionManager.load());
}

void setExecutionManager(atomPointer<class ExecutionManager *> executionManager) {
    executionManager_SetExecutionManager(executionManager);
}

atomPointer<class ExecutionManager *> executionManager_GetExecutionManager() {
    return executionManager_Current;
}

atomPointer<class ExecutionManager *> getExecutionManager() {
    return executionManager_GetExecutionManager();
}

void executionManager_Terminate() {
    println("terminating execution manager");
    executionManager_Current.load()->close.store(true);
    ExecutionManager::threadJoin(executionManager_Current.load()->thread);
    println("terminated execution manager");
}

int suspended(void * arg) {
    atomPointer<struct QTS *> q { static_cast<struct QTS *>(arg) };
    assert(q.load() != nullptr);
    assert(q.load()->t.load() != nullptr);
    q.load()->t.load()->died.store(false);
    q.load()->t.load()->killable.store(false);
    atom<long int> pid { getpid() };
    if (executionManager_Current.load()->debug.load()) println("STOPPING THREAD %d", pid.load());
    q.load()->t.load()->status.store(q.load()->t.load()->statusList.STOPPED);
    kill(pid.load(), SIGSTOP);
    q.load()->t.load()->status.store(q.load()->t.load()->statusList.RUNNING);
    if (executionManager_Current.load()->debug.load()) println("RESUMED THREAD %d", pid.load());
    assert(q.load()->f.load() != nullptr);
    assert(q.load()->arg.load() != nullptr);
    assert(q.load()->t.load() != nullptr);

    // main
    atom<int> R { q.load()->f.load()(q.load()->arg.load()) };

    q.load()->t.load()->killable.store(true);
    if (executionManager_Current.load()->debug.load()) println("while (q.load()->t.load()->killable) THREAD %d", pid.load());
    while (q.load()->t.load()->killable.load()) {
        sleep(1);
        println("waiting for killable to be false");
    }
    println("killable is false");
    if (executionManager_Current.load()->debug.load()) println("STOPPING THREAD %d, R = %d", pid.load(), R.load());
    q.load()->t.load()->status.store(q.load()->t.load()->statusList.STOPPED);
    kill(pid.load(), SIGSTOP);
    q.load()->t.load()->status.store(q.load()->t.load()->statusList.RUNNING);
    if (executionManager_Current.load()->debug.load()) println("RESUMED THREAD %d, R = %d, TERMINATING THREAD %d", pid.load(), R.load(), pid.load());
    return R.load();
}

atomPointer<Thread *> threadNew_(atomPointer<struct QTS *> q) {
    if (executionManager_Current.load()->debug.load()) println("threadNew_");
    if (executionManager_Current.load()->debug.load()) println("q.load()->t = new Thread()");
    q.load()->t.store(new Thread());
    if (executionManager_Current.load()->debug.load()) println("clone q.load()->t = %p", q.load()->t.load());
    q.load()->t.load()->suspend.store(q.load()->createSuspended.load());
    q.load()->t.load()->stack.load().alloc(q.load()->stack_size.load() == 0 ? 4096 : q.load()->stack_size.load());
    q.load()->t.load()->pid.store(clone(suspended, q.load()->t.load()->stack.load().top.load(), CLONE_VM|SIGCHLD, q.load()));
    if (executionManager_Current.load()->debug.load()) println("clone q.load()->t.load()->pid.load() = %d", q.load()->t.load()->pid.load());
    if (q.load()->t.load()->pid.load() == -1) {
        if (executionManager_Current.load()->debug.load()) perror("clone");
        q.load()->t.load()->stack.load().free();
    } else {
        if (q.load()->push.load()) {
            executionManager_Current.load()->QTS_VECTOR.load().push_back(q);
            if (executionManager_Current.load()->debug.load()) println("returning 1 %p", executionManager_Current.load()->QTS_VECTOR.load().back().load()->t.load());
            return executionManager_Current.load()->QTS_VECTOR.load().back().load()->t;
        } else {
            if (executionManager_Current.load()->debug.load()) println("returning 2 %p", q.load()->t.load());
            return q.load()->t;
        }
    }
    if (executionManager_Current.load()->debug.load()) println("returning 3 nullptr");
    atomPointer<Thread *> NULLPOINTER { nullptr };
    return NULLPOINTER;
}

atomPointer<Thread *> QINIT() {
    if (executionManager_Current.load()->isRunning.load()) return executionManager_Current.load()->thread;
    if (executionManager_Current.load()->debug.load()) println("initializing QINIT()");
    executionManager_Current.load()->isRunning.store(true);
    // we cannot use threadnew_() as killable will never be triggered

    if (executionManager_Current.load()->debug.load()) println("q.load()->t = new Thread()");
    atomPointer<struct QTS *> q { new struct QTS };
    q.load()->t.store(new Thread());
    if (executionManager_Current.load()->debug.load()) println("q.load()->t = %p", q.load()->t.load());
    q.load()->t.load()->stack.load().alloc(4096);
    q.load()->t.load()->pid.store(clone(ExecutionManager_instance, q.load()->t.load()->stack.load().top.load(), CLONE_VM|SIGCHLD, executionManager_Current.load()));
    if (q.load()->t.load()->pid.load() == -1) {
        if (executionManager_Current.load()->debug.load()) perror("clone");
        q.load()->t.load()->stack.load().free();
    } else {
        kill(q.load()->t.load()->pid.load(), SIGSTOP);
        executionManager_Current.load()->QTS_VECTOR.load().push_back(q);
        executionManager_Current.load()->thread.store(executionManager_Current.load()->QTS_VECTOR.load().back().load()->t.load());
        kill(q.load()->t.load()->pid.load(), SIGCONT);
        atexit(executionManager_Terminate);
        if (executionManager_Current.load()->debug.load()) println("initialized QINIT()");
        return executionManager_Current.load()->thread;
    }
    if (executionManager_Current.load()->debug.load()) {
        println("failed to initialize QINIT()");
        println("returning 3 nullptr");
    }
    atomPointer<Thread *> NULLPOINTER { nullptr };
    return NULLPOINTER;
}

atomPointer<void *> ExecutionManager::sendRequest(int request, atomPointer<void *> package) {
    executionManager_Current.load()->REQUEST.load().request.store(request);
    executionManager_Current.load()->REQUEST.load().package.store(package.load());
    if (executionManager_Current.load()->debug.load()) println("uploading");
    executionManager_Current.load()->REQUEST.load().upload.store(true);
    executionManager_Current.load()->REQUEST.load().uploadNotComplete.store(true);
    executionManager_Current.load()->REQUEST.load().processingNotComplete.store(true);
    if (executionManager_Current.load()->debug.load()) println("while(executionManager_Current.load()->REQUEST.uploadNotComplete.load()) start");
    while(executionManager_Current.load()->REQUEST.load().uploadNotComplete.load());
    if (executionManager_Current.load()->debug.load()) println("while(executionManager_Current.load()->REQUEST.uploadNotComplete.load()) done");
    if (executionManager_Current.load()->debug.load()) println("while(executionManager_Current.load()->REQUEST.processingNotComplete.load()) start");
    while(executionManager_Current.load()->REQUEST.load().processingNotComplete.load());
    if (executionManager_Current.load()->debug.load()) println("while(executionManager_Current.load()->REQUEST.processingNotComplete.load()) done");
    if (executionManager_Current.load()->debug.load()) println("upload complete");
    if (executionManager_Current.load()->debug.load()) println("executionManager_Current.load()->REQUEST.reply = %p", executionManager_Current.load()->REQUEST.reply.load());
    return executionManager_Current.load()->REQUEST.load().reply;
}

int staticHandleRequest(atomPointer<class ExecutionManager *> arg) {
    atomPointer<class ExecutionManager *> R { arg.load() } ; //reinterpret_cast<class atomPointer<class ExecutionManager *> >(arg);
    if (R.load()->REQUEST.load().request.load() != -1) {
        assert(R.load()->REQUEST.load().package.load() != nullptr);
        switch (R.load()->REQUEST.load().request.load()) {
            case ExecutionManager::REQUEST_ID.threadCreate:
                if (executionManager_Current.load()->debug.load()) println("request: threadCreate");
                if (executionManager_Current.load()->debug.load()) println("adding new thread");
                R.load()->REQUEST.load().reply.store(threadNew_(R.load()->REQUEST.load().package.recast<atomPointer<struct QTS *>>()).load());
                assert(R.load()->REQUEST.load().reply.load() != nullptr);
                R.load()->REQUEST.load().package.store(nullptr);
                if (executionManager_Current.load()->debug.load()) println("R.load()->REQUEST.reply = %p", R.load()->REQUEST.load().reply.load());
                if (executionManager_Current.load()->debug.load()) println("added new thread");
                if (executionManager_Current.load()->debug.load()) println("waiting for thread stop");
                R.load()->threadWaitUntilStopped(R.load()->REQUEST.load().reply.recast<atomPointer<Thread *>>());
                if (executionManager_Current.load()->debug.load()) println("thread has stopped");
                if (executionManager_Current.load()->debug.load()) println("request complete: threadCreate");
                break;
            case ExecutionManager::REQUEST_ID.threadJoin:
                if (executionManager_Current.load()->debug.load()) println("request: threadJoin");
                R.load()->threadJoin(R.load()->REQUEST.load().package.recast<atomPointer<Thread *>>());
                if (executionManager_Current.load()->debug.load()) println("request complete: threadJoin");
                break;
            case ExecutionManager::REQUEST_ID.threadJoinUntilStopped:
                if (executionManager_Current.load()->debug.load()) println("request: threadJoinUntilStopped");
                R.load()->threadJoinUntilStopped(R.load()->REQUEST.load().package.recast<atomPointer<Thread *>>());
                if (executionManager_Current.load()->debug.load()) println("request complete: threadJoinUntilStopped");
                break;
            case ExecutionManager::REQUEST_ID.threadWaitUntilStopped:
                if (executionManager_Current.load()->debug.load()) println("request: threadWaitUntilStopped");
                R.load()->threadWaitUntilStopped(R.load()->REQUEST.load().package.recast<atomPointer<Thread *>>());
                if (executionManager_Current.load()->debug.load()) println("request complete: threadWaitUntilStopped");
                break;
            case ExecutionManager::REQUEST_ID.threadWaitUntilRunning:
                if (executionManager_Current.load()->debug.load()) println("request: threadWaitUntilRunning");
                R.load()->threadWaitUntilRunning(R.load()->REQUEST.load().package.recast<atomPointer<Thread *>>());
                if (executionManager_Current.load()->debug.load()) println("request complete: threadWaitUntilRunning");
                break;
            case ExecutionManager::REQUEST_ID.threadWaitUntilRunningAndJoin:
                if (executionManager_Current.load()->debug.load()) println("request: threadWaitUntilRunningAndJoin");
                R.load()->threadWaitUntilRunningAndJoin(R.load()->REQUEST.load().package.recast<atomPointer<Thread *>>());
                if (executionManager_Current.load()->debug.load()) println("request complete: threadWaitUntilRunningAndJoin");
                break;
            case ExecutionManager::REQUEST_ID.threadWaitUntilRunningAndJoinUntilStopped:
                if (executionManager_Current.load()->debug.load()) println("request: threadWaitUntilRunningAndJoinUntilStopped");
                R.load()->threadWaitUntilRunningAndJoinUntilStopped(R.load()->REQUEST.load().package.recast<atomPointer<Thread *>>());
                if (executionManager_Current.load()->debug.load()) println("request complete: threadWaitUntilRunningAndJoinUntilStopped");
                break;
            case ExecutionManager::REQUEST_ID.threadPause:
                if (executionManager_Current.load()->debug.load()) println("request: threadPause");
                R.load()->threadPause(R.load()->REQUEST.load().package.recast<atomPointer<Thread *>>());
                if (executionManager_Current.load()->debug.load()) println("request complete: threadPause");
                break;
            case ExecutionManager::REQUEST_ID.threadResume:
                if (executionManager_Current.load()->debug.load()) println("request: threadResume");
                R.load()->threadResume(R.load()->REQUEST.load().package.recast<atomPointer<Thread *>>());
                if (executionManager_Current.load()->debug.load()) println("request complete: threadResume");
                break;
            case ExecutionManager::REQUEST_ID.threadResumeAndJoin:
                if (executionManager_Current.load()->debug.load()) println("request: threadResumeAndJoin");
                R.load()->threadResumeAndJoin(R.load()->REQUEST.load().package.recast<atomPointer<Thread *>>());
                if (executionManager_Current.load()->debug.load()) println("request complete: threadResumeAndJoin");
                break;
            case ExecutionManager::REQUEST_ID.threadTerminate:
                if (executionManager_Current.load()->debug.load()) println("request: threadTerminate");
                R.load()->threadTerminate(R.load()->REQUEST.load().package.recast<atomPointer<Thread *>>());
                if (executionManager_Current.load()->debug.load()) println("request complete: threadTerminate");
                break;
            case ExecutionManager::REQUEST_ID.threadKill:
                if (executionManager_Current.load()->debug.load()) println("request: threadKill");
                R.load()->threadKill(R.load()->REQUEST.load().package.recast<atomPointer<Thread *>>());
                if (executionManager_Current.load()->debug.load()) println("request complete: tthreadKill");
                break;
            default:
                if (executionManager_Current.load()->debug.load()) println("unknown request");
                abort();
                break;
        }
    }
    R.load()->REQUEST.load().request.store(-1);
    R.load()->REQUEST.load().stack.load().free();
    if (executionManager_Current.load()->debug.load()) println("R.load()->REQUEST.processing.store(false);");
    R.load()->REQUEST.load().processing.store(false);
    if (executionManager_Current.load()->debug.load()) println("R.load()->REQUEST.processingNotComplete.store(false);");
    R.load()->REQUEST.load().processingNotComplete.store(false);
    if (executionManager_Current.load()->debug.load()) println("return 0");
    return 0;
}

void ExecutionManager::handleRequest(atomPointer<class ExecutionManager *> EX) {
    if (executionManager_Current.load()->debug.load()) println("REQUEST.processing.store(true);");
    REQUEST.load().processing.store(true);
    if (executionManager_Current.load()->debug.load()) println("REQUEST.upload.store(false);");
    REQUEST.load().upload.store(false);
    if (executionManager_Current.load()->debug.load()) println("REQUEST.uploadNotComplete.store(false);");
    REQUEST.load().uploadNotComplete.store(false);
    staticHandleRequest(EX);
    if (executionManager_Current.load()->debug.load()) println("return");
}

atom<bool> ExecutionManager::errorChecking(atomPointer<struct QTS *> q) {
    atom<bool> r { false };
    if (q.load()->t == nullptr) r.store(true); // skip non existant threads
    if (q.load()->t.load()->died.load()) r.store(true); // skip died threads
    return r;
}

int ExecutionManager_instance(void * arg) {
    // without class, we get error: ‘ExecutionManager’ does not name a type
    atomPointer<class ExecutionManager *> ExecutionManager { reinterpret_cast<class ExecutionManager * >(arg) };

    if (ExecutionManager.load()->debug.load()) println("starting Execution Manager");

    pid_t self = ExecutionManager.load()->thread.load()->pid.load();
    assert(self == ExecutionManager.load()->thread.load()->pid.load());
    assert(ExecutionManager.load()->QTS_VECTOR.load().size() != 0);
    assert(ExecutionManager.load()->QTS_VECTOR.load()[0] != nullptr);
    assert(self == ExecutionManager.load()->QTS_VECTOR.load()[0].load()->t.load()->pid.load());
    if (ExecutionManager.load()->debug.load()) println("EXECUTION MANAGER!");
    atom<bool> e { false };
    while(true) {
        if (e.load()) break;
        atom<int> i { 0 };
        while(i.load() < ExecutionManager.load()->QTS_VECTOR.load().size()) {
            if (ExecutionManager.load()->close.load()) {
                if (i.load() == 0) {
                    if (ExecutionManager.load()->QTS_VECTOR.load().size() == 1) {
                        e.store(true);
                        break;
                    }
                }
                atomPointer<struct QTS *> q = ExecutionManager.load()->QTS_VECTOR.load()[i.load()];
                println("terminating thread %d", q.load()->t.load()->pid.load());
                threadTerminate(q.load()->t);
                continue;
            };
            if (ExecutionManager.load()->REQUEST.load().upload.load() && !ExecutionManager.load()->REQUEST.load().processing.load()) {
                if (ExecutionManager.load()->debug.load()) println("if (ExecutionManager.load()->REQUEST.upload && !ExecutionManager.load()->REQUEST.processing)");
                if (executionManager_Current.load()->debug.load()) println("handling request");
                ExecutionManager.load()->handleRequest(ExecutionManager);
                if (executionManager_Current.load()->debug.load()) println("handled request");
            }
            if (ExecutionManager.load()->debug.load()) println("ALIVE");

            // skip self
            if (i.load() == 0) { i++; continue; }

            // obtain a thread
            atomPointer<struct QTS *> q = ExecutionManager.load()->QTS_VECTOR.load()[i.load()];
//            threadInfo(q.load()->t);
            // error checking
//            if (ExecutionManager.load()->debug.load()) println("error checking");
            if (ExecutionManager.load()->errorChecking(q.load()).load()) { println("error occured, skipping"); i++; continue; }

            assert(q.load()->t.load()->pid.load() != self);

            // check if thread is running
//            if (ExecutionManager.load()->debug.load()) println("threadIsRunning(q.load()->t)");
            if (ExecutionManager.load()->threadIsRunning(q.load()->t.load())) {
//                if (ExecutionManager.load()->debug.load()) println("threadIsRunning(q.load()->t) true");
//                if (ExecutionManager.load()->debug.load()) println("THREAD %d running", q.load()->t.load()->pid.load());
                // cleanup thread if needed
                if (q.load()->t.load()->killable.load()) {
                    if (ExecutionManager.load()->debug.load()) println("THREAD %d is killable", q.load()->t.load()->pid.load());
                    q.load()->t.load()->killable.store(false);
                    if (ExecutionManager.load()->debug.load()) println("THREAD %d threadWaitUntilStopped", q.load()->t.load()->pid.load());
                    ExecutionManager.load()->threadWaitUntilStopped(q.load()->t);
                    if (ExecutionManager.load()->debug.load()) println("THREAD %d stopped", q.load()->t.load()->pid.load());
                    ExecutionManager.load()->threadResumeAndJoin(q.load()->t);
                    if (ExecutionManager.load()->debug.load()) println("THREAD %d has DIED: return code: %d", q.load()->t.load()->pid.load(), q.load()->t.load()->returnCode.load());
                    q.load()->t.load()->died.store(true);
                } else {
//                    if (ExecutionManager.load()->debug.load()) println("THREAD %d is not killable", q.load()->t.load()->pid.load());
                }
//                if (ExecutionManager.load()->debug.load()) println("q.load()->t.load()->reapable");
                if (q.load()->t.load()->reapable.load()) {
                    if (ExecutionManager.load()->debug.load()) println("q.load()->t.load()->reapable true");
                    if (ExecutionManager.load()->debug.load()) println("reaping pid %d", q.load()->t.load()->pid.load());
                    if (ExecutionManager.load()->debug.load()) println("erasing ExecutionManager.load()->QTS_VECTOR.load()[%d]", i);
                    if (ExecutionManager.load()->debug.load()) println("ExecutionManager.load()->QTS_VECTOR.load().size() is %d",
                           ExecutionManager.load()->QTS_VECTOR.load().size());
                    ExecutionManager.load()->QTS_VECTOR.load().erase(ExecutionManager.load()->QTS_VECTOR.load().begin() + i.load());
                    if (ExecutionManager.load()->debug.load()) println("ExecutionManager.load()->QTS_VECTOR.load().size() is %d",
                           ExecutionManager.load()->QTS_VECTOR.load().size());
                    q.load()->t.load()->reaped.store(true);
                } else {
//                    if (ExecutionManager.load()->debug.load()) println("q.load()->t.load()->reapable false");
                }
            } else {
//                if (ExecutionManager.load()->debug.load()) println("threadIsRunning(q.load()->t) false");
            }
            i++;
        }
    }
    if (ExecutionManager.load()->debug.load()) println("exiting Execution Manager");
    ExecutionManager.load()->QTS_VECTOR.load().clear();
    executionManager_Current.store(nullptr);
    return 0;
}

atomPointer<Thread *> ExecutionManager::threadNew(bool createSuspended, size_t stack_size, std::function<int(void*)>, void * arg) {
    atomPointer<struct QTS *> qts { new struct QTS };
    qts.load()->createSuspended.store(createSuspended);
    qts.load()->stack_size.store(stack_size);
    qts.load()->f.store(f);
    qts.load()->arg.store(arg);
    qts.load()->push.store(true);
    if (executionManager_Current.load()->debug.load()) println("atomPointer<Thread *> x = static_cast<Thread*>(sendRequest(ExecutionManager::REQUEST_ID.threadCreate, qts)); starting");
    atomPointer<Thread *> x { sendRequest(ExecutionManager::REQUEST_ID.threadCreate, qts.load()).recast<atomPointer<Thread *>>().load() };
    if (executionManager_Current.load()->debug.load()) println("atomPointer<Thread *> x = static_cast<Thread*>(sendRequest(ExecutionManager::REQUEST_ID.threadCreate, qts)); complete");
    if (executionManager_Current.load()->debug.load()) println("x = %p", x.load());
    assert(x.load() != nullptr);
    if (executionManager_Current.load()->debug.load()) println("createSuspended: %s", boolToString(createSuspended.load()));
    if (!createSuspended.load()) {
        if (executionManager_Current.load()->debug.load()) println("resuming from threadNew");
        sendRequest(ExecutionManager::REQUEST_ID.threadResume, x);
        if (executionManager_Current.load()->debug.load()) println("resumed from threadNew");
    }
    return x;
}

void ExecutionManager::threadJoin(atomPointer<Thread *> t) {
    if (executionManager_Current.load()->debug.load()) println("joining");
    if (t.load()->pid.load() != -1) {
        siginfo_t info;
        for (;;) {
            if (waitid(P_PID, t.load()->pid.load(), &info, WEXITED | WSTOPPED) == -1) {
                if (executionManager_Current.load()->debug.load()) perror("waitid");
                return;
            }
            atom<bool> con { false };
            switch (info.si_code) {
                case CLD_EXITED:
                    t.load()->status.store(t.load()->statusList.EXITED);
                    t.load()->returnCode.store(info.si_status);
                    t.load()->reapable.store(true);
                    break;
                case CLD_KILLED:
                    t.load()->status.store(t.load()->statusList.KILLED_BY_SIGNAL);
                    t.load()->signal.store(info.si_status);
                    t.load()->reapable.store(true);
                    break;
                case CLD_DUMPED:
                    t.load()->status.store(t.load()->statusList.KILLED_BY_SIGNAL);
                    t.load()->signal.store(info.si_status);
                    t.load()->core_dumped.store(true);
                    t.load()->reapable.store(true);
                    break;
                case CLD_STOPPED:
                    con.store(true);
                    break;
                default:
                    t.load()->status.store(t.load()->statusList.UNDEFINED);
                    break;
            }
            if (!con.load()) break;
        }
    }
}

void ExecutionManager::threadJoinUntilStopped(atomPointer<Thread *> t) {
    if (t.load()->pid.load() != -1) {
        siginfo_t info;
        if (waitid(P_PID, t.load()->pid.load(), &info, WEXITED|WSTOPPED) == -1) {
            if (executionManager_Current.load()->debug.load()) perror("waitid");
            return;
        }
        switch (info.si_code) {
            case CLD_EXITED:
                t.load()->status.store(t.load()->statusList.EXITED);
                t.load()->returnCode.store(info.si_status);
                t.load()->reapable.store(true);
                break;
            case CLD_KILLED:
                t.load()->status.store(t.load()->statusList.KILLED_BY_SIGNAL);
                t.load()->signal.store(info.si_status);
                t.load()->reapable.store(true);
                break;
            case CLD_DUMPED:
                t.load()->status.store(t.load()->statusList.KILLED_BY_SIGNAL);
                t.load()->signal.store(info.si_status);
                t.load()->core_dumped.store(true);
                t.load()->reapable.store(true);
                break;
            case CLD_STOPPED:
                t.load()->status.store(t.load()->statusList.STOPPED);
                break;
            default:
                t.load()->status.store(t.load()->statusList.UNDEFINED);
                break;
        }
    }
}

void ExecutionManager::threadWaitUntilStopped(atomPointer<Thread *> t) {
    if (t.load()->pid.load() != -1) {
        siginfo_t info;
        if (executionManager_Current.load()->debug.load()) println("waiting for pid %d", t.load()->pid.load());
        if (waitid(P_PID, t.load()->pid.load(), &info, WSTOPPED) == -1) {
            if (executionManager_Current.load()->debug.load()) perror("waitid");
            return;
        }
        if (executionManager_Current.load()->debug.load()) println("waited for pid %d", t.load()->pid.load());
        switch (info.si_code) {
            case CLD_KILLED:
                t.load()->status.store(t.load()->statusList.KILLED_BY_SIGNAL);
                t.load()->signal.store(info.si_status);
                t.load()->reapable.store(true);
                break;
            case CLD_DUMPED:
                t.load()->status.store(t.load()->statusList.KILLED_BY_SIGNAL);
                t.load()->signal.store(info.si_status);
                t.load()->core_dumped.store(true);
                t.load()->reapable.store(true);
                break;
            case CLD_STOPPED:
                t.load()->status.store(t.load()->statusList.STOPPED);
                break;
            default:
                t.load()->status.store(t.load()->statusList.UNDEFINED);
                break;
        }
    }
}

void ExecutionManager::threadWaitUntilRunning(atomPointer<Thread *> t) {
    if (t.load()->pid.load() != -1) {
        siginfo_t info;
        if (waitid(P_PID, t.load()->pid.load(), &info, WCONTINUED) == -1) {
            if (executionManager_Current.load()->debug.load()) perror("waitid");
            return;
        }
        switch (info.si_code) {
            case CLD_CONTINUED:
                t.load()->status.store(t.load()->statusList.RUNNING);
                break;
            default:
                t.load()->status.store(t.load()->statusList.UNDEFINED);
                break;
        }
    }
}

void ExecutionManager::threadWaitUntilRunningAndJoin(atomPointer<Thread *> t) {
    if (t.load()->pid.load() != -1) {
        siginfo_t info;
        if (waitid(P_PID, t.load()->pid.load(), &info, WCONTINUED) == -1) {
            if (executionManager_Current.load()->debug.load()) perror("waitid");
            return;
        }
        switch (info.si_code) {
            case CLD_CONTINUED:
                t.load()->status.store(t.load()->statusList.RUNNING);
                for (;;) {
                    if (waitid(P_PID, t.load()->pid.load(), &info, WEXITED | WSTOPPED) == -1) {
                        if (executionManager_Current.load()->debug.load()) perror("waitid");
                        return;
                    }
                    atom<bool> con { false };
                    switch (info.si_code) {
                        case CLD_EXITED:
                            t.load()->status.store(t.load()->statusList.EXITED);
                            t.load()->returnCode.store(info.si_status);
                            t.load()->reapable.store(true);
                            break;
                        case CLD_KILLED:
                            t.load()->status.store(t.load()->statusList.KILLED_BY_SIGNAL);
                            t.load()->signal.store(info.si_status);
                            t.load()->reapable.store(true);
                            break;
                        case CLD_DUMPED:
                            t.load()->status.store(t.load()->statusList.KILLED_BY_SIGNAL);
                            t.load()->signal.store(info.si_status);
                            t.load()->core_dumped.store(true);
                            t.load()->reapable.store(true);
                            break;
                        case CLD_STOPPED:
                            con.store(true);
                            break;
                        default:
                            t.load()->status.store(t.load()->statusList.UNDEFINED);
                            break;
                    }
                    if (!con.load()) break;
                }
                break;
            default:
                t.load()->status.store(t.load()->statusList.UNDEFINED);
                break;
        }
    }
}

void ExecutionManager::threadWaitUntilRunningAndJoinUntilStopped(atomPointer<Thread *> t) {
    if (t.load()->pid.load() != -1) {
        siginfo_t info;
        if (waitid(P_PID, t.load()->pid.load(), &info, WCONTINUED) == -1) {
            if (executionManager_Current.load()->debug.load()) perror("waitid");
            return;
        }
        switch (info.si_code) {
            case CLD_CONTINUED:
                t.load()->status.store(t.load()->statusList.RUNNING);
                if (waitid(P_PID, t.load()->pid.load(), &info, WEXITED|WSTOPPED) == -1) {
                    if (executionManager_Current.load()->debug.load()) perror("waitid");
                    return;
                }
                switch (info.si_code) {
                    case CLD_EXITED:
                        t.load()->status.store(t.load()->statusList.EXITED);
                        t.load()->returnCode.store(info.si_status);
                        t.load()->reapable.store(true);
                        break;
                    case CLD_KILLED:
                        t.load()->status.store(t.load()->statusList.KILLED_BY_SIGNAL);
                        t.load()->signal.store(info.si_status);
                        t.load()->reapable.store(true);
                        break;
                    case CLD_DUMPED:
                        t.load()->status.store(t.load()->statusList.KILLED_BY_SIGNAL);
                        t.load()->signal.store(info.si_status);
                        t.load()->core_dumped.store(true);
                        t.load()->reapable.store(true);
                        break;
                    case CLD_STOPPED:
                        t.load()->status.store(t.load()->statusList.STOPPED);
                        break;
                    default:
                        t.load()->status.store(t.load()->statusList.UNDEFINED);
                        break;
                }
                break;
            default:
                t.load()->status.store(t.load()->statusList.UNDEFINED);
                break;
        }
    }
}

atom<bool> ExecutionManager::threadIsStopped(atomPointer<Thread *> t) {
    return t.load()->status.load() == t.load()->statusList.STOPPED;
}

atom<bool> ExecutionManager::threadIsRunning(atomPointer<Thread *> t) {
    return t.load()->status.load() == t.load()->statusList.RUNNING;
}

void ExecutionManager::threadPause(atomPointer<Thread *> t) {
    if (t.load()->pid.load() != -1) {
        if (kill(t.load()->pid.load(), SIGSTOP) == -1) if (executionManager_Current.load()->debug.load()) perror("kill");
        siginfo_t info;
        if (waitid(P_PID, t.load()->pid.load(), &info, WSTOPPED) == -1) {
            if (executionManager_Current.load()->debug.load()) perror("waitid");
            return;
        }
        switch (info.si_code) {
            case CLD_KILLED:
                t.load()->status.store(t.load()->statusList.KILLED_BY_SIGNAL);
                t.load()->signal.store(info.si_status);
                t.load()->reapable.store(true);
                break;
            case CLD_DUMPED:
                t.load()->status.store(t.load()->statusList.KILLED_BY_SIGNAL);
                t.load()->signal.store(info.si_status);
                t.load()->core_dumped.store(true);
                t.load()->reapable.store(true);
                break;
            case CLD_STOPPED:
                t.load()->status.store(t.load()->statusList.STOPPED);
                break;
            default:
                t.load()->status.store(t.load()->statusList.UNDEFINED);
                break;
        }
    }
}

void ExecutionManager::threadResume(atomPointer<Thread *> t) {
    if (t.load()->pid.load() != -1) {
        if (kill(t.load()->pid.load(), SIGCONT) == -1) if (executionManager_Current.load()->debug.load()) perror("kill");
        siginfo_t info;
        if (waitid(P_PID, t.load()->pid.load(), &info, WCONTINUED) == -1) {
            if (executionManager_Current.load()->debug.load()) perror("waitid");
            return;
        }
        switch (info.si_code) {
            case CLD_CONTINUED:
                t.load()->status.store(t.load()->statusList.RUNNING);
                break;
            default:
                t.load()->status.store(t.load()->statusList.UNDEFINED);
                break;
        }
    }
}

void ExecutionManager::threadResumeAndJoin(atomPointer<Thread *> t) {
    if (t.load()->pid.load() != -1) {
        if (kill(t.load()->pid.load(), SIGCONT) == -1) if (executionManager_Current.load()->debug.load()) perror("kill");
        siginfo_t info;
        if (waitid(P_PID, t.load()->pid.load(), &info, WCONTINUED) == -1) {
            if (executionManager_Current.load()->debug.load()) perror("waitid");
            return;
        }
        switch (info.si_code) {
            case CLD_CONTINUED:
                t.load()->status.store(t.load()->statusList.RUNNING);
                for (;;) {
                    if (waitid(P_PID, t.load()->pid.load(), &info, WEXITED | WSTOPPED) == -1) {
                        if (executionManager_Current.load()->debug.load()) perror("waitid");
                        return;
                    }
                    atom<bool> con { false };
                    switch (info.si_code) {
                        case CLD_EXITED:
                            t.load()->status.store(t.load()->statusList.EXITED);
                            t.load()->returnCode.store(info.si_status);
                            t.load()->reapable.store(true);
                            break;
                        case CLD_KILLED:
                            t.load()->status.store(t.load()->statusList.KILLED_BY_SIGNAL);
                            t.load()->signal.store(info.si_status);
                            t.load()->reapable.store(true);
                            break;
                        case CLD_DUMPED:
                            t.load()->status.store(t.load()->statusList.KILLED_BY_SIGNAL);
                            t.load()->signal.store(info.si_status);
                            t.load()->core_dumped.store(true);
                            t.load()->reapable.store(true);
                            break;
                        case CLD_STOPPED:
                            con.store(true);
                            break;
                        default:
                            t.load()->status.store(t.load()->statusList.UNDEFINED);
                            break;
                    }
                    if (!con.load()) break;
                }
                break;
            default:
                t.load()->status.store(t.load()->statusList.UNDEFINED);
                break;
        }
    }
}

void ExecutionManager::threadTerminate(atomPointer<Thread *> t) {
    if (t.load()->pid.load() != -1) {
        if (kill(t.load()->pid.load(), SIGTERM) == -1) if (executionManager_Current.load()->debug.load()) perror("kill");
        siginfo_t info;
        if (waitid(P_PID, t.load()->pid.load(), &info, WSTOPPED) == -1) {
            if (executionManager_Current.load()->debug.load()) perror("waitid");
            return;
        }
        switch (info.si_code) {
            case CLD_KILLED:
                t.load()->status.store(t.load()->statusList.KILLED_BY_SIGNAL);
                t.load()->signal.store(info.si_status);
                t.load()->terminated_by_user.store(true);
                break;
            case CLD_DUMPED:
                t.load()->status.store(t.load()->statusList.KILLED_BY_SIGNAL);
                t.load()->signal.store(info.si_status);
                t.load()->core_dumped.store(true);
                t.load()->terminated_by_user.store(true);
                break;
            case CLD_STOPPED:
                t.load()->status.store(t.load()->statusList.STOPPED);
                break;
            default:
                t.load()->status.store(t.load()->statusList.UNDEFINED);
                break;
        }
    }
}

void ExecutionManager::threadKill(atomPointer<Thread *> t) {
    if (t.load()->pid.load() != -1) {
        if (kill(t.load()->pid.load(), SIGKILL) == -1) if (executionManager_Current.load()->debug.load()) perror("kill");
        siginfo_t info;
        if (waitid(P_PID, t.load()->pid.load(), &info, WSTOPPED) == -1) {
            if (executionManager_Current.load()->debug.load()) perror("waitid");
            return;
        }
        switch (info.si_code) {
            case CLD_KILLED:
                t.load()->status.store(t.load()->statusList.KILLED_BY_SIGNAL);
                t.load()->signal.store(info.si_status);
                t.load()->killed_by_user.store(true);
                break;
            case CLD_DUMPED:
                t.load()->status.store(t.load()->statusList.KILLED_BY_SIGNAL);
                t.load()->signal.store(info.si_status);
                t.load()->core_dumped.store(true);
                t.load()->killed_by_user.store(true);
                break;
            case CLD_STOPPED:
                t.load()->status.store(t.load()->statusList.STOPPED);
                break;
            default:
                t.load()->status.store(t.load()->statusList.UNDEFINED);
                break;
        }
    }
}

int helperVoid(void * v) {
    reinterpret_cast<std::function<void()>>(v)();
    return 0;
}

atomPointer<Thread *> threadNew(atom<bool> createSuspended, size_t stack_size, std::function<int(void*)>, void * arg) {
    if (executionManager_Current.load() != nullptr) QINIT();
    else {
        println("error: an execution manager must be set, use setExecutionManager(YourExecutionManager); to set one");
        abort();
    }
    return executionManager_Current.load()->threadNew(createSuspended, stack_size, f, arg);
}

atomPointer<Thread *> threadNew(std::function<int(void*)>, void * arg) {
    return threadNew(false, 0, f, arg);
}

atomPointer<Thread *> threadNew(size_t stack_size, std::function<int(void*)>, void * arg) {
    return threadNew(false, stack_size, f, arg);
}

atomPointer<Thread *> threadNew(size_t stack_size, void (*f)()) {
    return threadNew(stack_size, helperVoid, reinterpret_cast<void*>(f));
}

atomPointer<Thread *> threadNew(void (*f)()) {
    return threadNew(0, helperVoid, reinterpret_cast<void*>(f));
}

atomPointer<Thread *> threadNewSuspended(std::function<int(void*)>, void * arg) {
    return threadNew(false, 0, f, arg);
}

atomPointer<Thread *> threadNewSuspended(size_t stack_size, std::function<int(void*)>, void * arg) {
    return threadNew(false, stack_size, f, arg);
}

atomPointer<Thread *> threadNewSuspended(size_t stack_size, void (*f)()) {
    return threadNew(stack_size, helperVoid, reinterpret_cast<void*>(f));
}

atomPointer<Thread *> threadNewSuspended(void (*f)()) {
    return threadNew(0, helperVoid, reinterpret_cast<void*>(f));
}

void threadJoin(atomPointer<Thread *> t) {
    while(!t.load()->died.load());
//    ExecutionManager::sendRequest(ExecutionManager::REQUEST_ID.threadJoin, t);
}

void threadJoinUntilStopped(atomPointer<Thread *> t) {
    ExecutionManager::sendRequest(ExecutionManager::REQUEST_ID.threadJoinUntilStopped, t.recast<atom<void*>>());
}

void threadWaitUntilStopped(atomPointer<Thread *> t) {
    ExecutionManager::sendRequest(ExecutionManager::REQUEST_ID.threadWaitUntilStopped, t.recast<atom<void*>>());
}

void threadWaitUntilRunning(atomPointer<Thread *> t) {
    ExecutionManager::sendRequest(ExecutionManager::REQUEST_ID.threadWaitUntilRunning, t.recast<atom<void*>>());
}

void threadWaitUntilRunningAndJoin(atomPointer<Thread *> t) {
    ExecutionManager::sendRequest(ExecutionManager::REQUEST_ID.threadWaitUntilRunningAndJoin, t.recast<atom<void*>>());
}

void threadWaitUntilRunningAndJoinUntilStopped(atomPointer<Thread *> t) {
    ExecutionManager::sendRequest(ExecutionManager::REQUEST_ID.threadWaitUntilRunningAndJoinUntilStopped, t.recast<atom<void*>>());
}

bool threadIsStopped(atomPointer<Thread *> t) {
    return ExecutionManager::threadIsStopped(t);
}

bool threadIsRunning(atomPointer<Thread *> t) {
    return ExecutionManager::threadIsRunning(t);
}

void threadPause(atomPointer<Thread *> t) {
    ExecutionManager::sendRequest(ExecutionManager::REQUEST_ID.threadPause, t.recast<atom<void*>>());
}

void threadResume(atomPointer<Thread *> t) {
    ExecutionManager::sendRequest(ExecutionManager::REQUEST_ID.threadResume, t.recast<atom<void*>>());
}

void threadResumeAndJoin(atomPointer<Thread *> t) {
    ExecutionManager::sendRequest(ExecutionManager::REQUEST_ID.threadResumeAndJoin, t.recast<atom<void*>>());
}

void threadTerminate(atomPointer<Thread *> t) {
    ExecutionManager::sendRequest(ExecutionManager::REQUEST_ID.threadTerminate, t.recast<atom<void*>>());
}

void threadKill(atomPointer<Thread *> t) {
    ExecutionManager::sendRequest(ExecutionManager::REQUEST_ID.threadKill, t.recast<atom<void*>>());
}

void threadInfo(atomPointer<Thread *> t) {
    println("THREAD INFO:");
    atom<bool> threadExists { t != nullptr };
    println("    thread exists = %s", boolToString(threadExists.load()));
    if (threadExists.load()) {
        println("    STACK INFO:");
        atom<bool> stackExists { t.load()->stack.load().stack != nullptr };
        println("        stack exists = %s", boolToString(stackExists.load()));
        if (stackExists.load()) {
            println("        address = %p", t.load()->stack.load().stack);
            println("        top = %p", t.load()->stack.load().top);
            println("        size = %ld", t.load()->stack.load().size);
            println("        direction = %s", t.load()->stack.load().getStackDirectionAsString());
        }
        println("    pid = %d", t.load()->pid.load());
        println("    killable = %s", boolToString(t.load()->killable.load()));
        println("    initially suspended = %s", boolToString(t.load()->suspend.load()));
        println("    dead = %s", boolToString(t.load()->died.load()));
        char * stat = nullptr;
        switch (t.load()->status) {
            case t.load()->statusList.UNDEFINED:
                stat = "UNDEFINED";
                break;
            case t.load()->statusList.RUNNING:
                stat = "running";
                break;
            case t.load()->statusList.STOPPED:
                stat = "stopped";
                break;
            case t.load()->statusList.DEAD:
                stat = "dead";
                break;
            case t.load()->statusList.EXITED:
                stat = "exited";
                break;
            case t.load()->statusList.KILLED_BY_SIGNAL:
                stat = "killed by signal";
                break;
            default:
                stat = "unknown";
                break;
        }
        println("    status = %s", stat);
        println("    signal = %d", t.load()->signal);
        println("    returnCode = %d", t.load()->returnCode);
        println("    core dumped = %s", boolToString(t.load()->core_dumped.load()));
        println("    reapable = %s", boolToString(t.load()->reapable.load()));
        println("    reaped = %s", boolToString(t.load()->reaped.load()));
        println("    terminated by user = %s", boolToString(t.load()->terminated_by_user.load()));
        println("    killed by user = %s", boolToString(t.load()->killed_by_user.load()));
    }
}