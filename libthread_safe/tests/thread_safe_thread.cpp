//
// Created by brothercomplex on 6/11/19.
//

#include <thread_safe/thread_safe.h>
#include <gtest/gtest.h>

#include <vector>
#include <thread>
#include <atomic>

// https://godbolt.org/z/J_DnFv

class Thread_Safe_Thread : public ::testing::Test {
protected:
    struct x {
        std::atomic<int> controlVariable = {0}; // should be thread-safe
        thread_safe<int> thread_safeInteger = {0};
    } x;
public:
    static void LOOPS_one(struct x * x) {
        for (int n = 0; n < 1000; ++n) {
            x->controlVariable.fetch_add(1);
            x->thread_safeInteger.fetch_add(1); // 1000
            x->thread_safeInteger++; // 2000
            x->thread_safeInteger += 1; // 3000
        }
    }
};

TEST_F(Thread_Safe_Thread, Loops) {
    std::vector<std::thread> v;
    for (int n = 0; n < 10; ++n) {
        v.emplace_back(std::thread(LOOPS_one, &x));
    }
    for (auto& t : v) {
        t.join();
    }
    ASSERT_EQ(x.controlVariable, 10000);
    ASSERT_EQ(10000, x.controlVariable);
    ASSERT_EQ(x.thread_safeInteger, x.thread_safeInteger);
    ASSERT_EQ(x.thread_safeInteger, x.thread_safeInteger);
    ASSERT_EQ(x.thread_safeInteger, 30000);
    ASSERT_EQ(30000, x.thread_safeInteger);
    ASSERT_EQ(x.thread_safeInteger, x.controlVariable*3);
    ASSERT_EQ(x.controlVariable*3, x.thread_safeInteger);
}