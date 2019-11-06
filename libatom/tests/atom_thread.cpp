//
// Created by brothercomplex on 6/11/19.
//

#include <atom/atom.h>
#include <gtest/gtest.h>

#include <vector>
#include <thread>
#include <atomic>

class AtomThread : public ::testing::Test {
protected:
    struct x {
        std::atomic<int> controlVariable = {0}; // should be thread-safe
        atom<int> atomInteger = {0};
    } x;
public:
    static void LOOPS_one(struct x * x) {
        for (int n = 0; n < 1000; ++n) {
            x->controlVariable.fetch_add(1);
            x->atomInteger.fetch_add(1); // 1000
            x->atomInteger++; // 2000
            x->atomInteger += 1; // 3000
        }
    }
};

TEST_F(AtomThread, Loops) {
    std::vector<std::thread> v;
    for (int n = 0; n < 10; ++n) {
        v.emplace_back(std::thread(LOOPS_one, &x));
    }
    for (auto& t : v) {
        t.join();
    }
    EXPECT_EQ(x.controlVariable, 10000);
    EXPECT_EQ(10000, x.controlVariable);
    EXPECT_EQ(x.atomInteger, x.atomInteger);
    EXPECT_EQ(x.atomInteger, x.atomInteger);
    EXPECT_EQ(x.atomInteger, 30000);
    EXPECT_EQ(30000, x.atomInteger);
    EXPECT_EQ(x.atomInteger, x.controlVariable*3);
    EXPECT_EQ(x.controlVariable*3, x.atomInteger);
}