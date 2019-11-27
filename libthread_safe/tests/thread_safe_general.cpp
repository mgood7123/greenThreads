//
// Created by brothercomplex on 6/11/19.
//

#include <thread_safe/thread_safe.h>
#include <gtest/gtest.h>

class Thread_Safe_Core : public ::testing::Test {
protected:
    thread_safe<int> Thread_SafeInteger;

    void ATOM_EQUALITY(thread_safeParameterType <int> & Thread_SafeInteger1, thread_safeParameterType <int> & Thread_SafeInteger2) {
        int five = 5;
        const int fiveConst = 5;
        ASSERT_EQ(Thread_SafeInteger1, Thread_SafeInteger2);
        ASSERT_EQ(Thread_SafeInteger2, Thread_SafeInteger1);
        ASSERT_EQ(Thread_SafeInteger1, 5);
        ASSERT_EQ(5, Thread_SafeInteger1);
        ASSERT_EQ(Thread_SafeInteger1, five);
        ASSERT_EQ(five, Thread_SafeInteger1);
        ASSERT_EQ(Thread_SafeInteger1, fiveConst);
        ASSERT_EQ(fiveConst, Thread_SafeInteger1);
        ASSERT_TRUE(Thread_SafeInteger1 == Thread_SafeInteger2);
        ASSERT_TRUE(Thread_SafeInteger2 == Thread_SafeInteger1);
        ASSERT_TRUE(Thread_SafeInteger1 == 5);
        ASSERT_TRUE(5 == Thread_SafeInteger1);
        ASSERT_TRUE(Thread_SafeInteger1 == five);
        ASSERT_TRUE(five == Thread_SafeInteger1);
        ASSERT_TRUE(Thread_SafeInteger1 == fiveConst);
        ASSERT_TRUE(fiveConst == Thread_SafeInteger1);
    }
};

TEST_F(Thread_Safe_Core, initializationViaAssignment) {
    Thread_SafeInteger = 5;
    ASSERT_EQ(Thread_SafeInteger.stored, true);
    ASSERT_EQ(Thread_SafeInteger.value, 5);
    thread_safe <int> Thread_SafeInteger2 = 5;
    ASSERT_EQ(Thread_SafeInteger2.stored, true);
    ASSERT_EQ(Thread_SafeInteger2.value, 5);
}

TEST_F(Thread_Safe_Core, initializationViaAssignmentBraces) {
    Thread_SafeInteger = {5};
    ASSERT_EQ(Thread_SafeInteger.stored, true);
    ASSERT_EQ(Thread_SafeInteger.value, 5);
    thread_safe <int> Thread_SafeInteger2 {5};
    ASSERT_EQ(Thread_SafeInteger2.stored, true);
    ASSERT_EQ(Thread_SafeInteger2.value, 5);
}

TEST_F(Thread_Safe_Core, loadUnstored) {
    ASSERT_THROW(Thread_SafeInteger.load(), thread_safeExceptions::NoValueStored);
}

TEST_F(Thread_Safe_Core, store) {
    Thread_SafeInteger.store(5);
    ASSERT_EQ(Thread_SafeInteger.stored, true);
    ASSERT_EQ(Thread_SafeInteger.value, 5);
}

TEST_F(Thread_Safe_Core, loadStored) {
    Thread_SafeInteger.store(5);
    ASSERT_EQ(Thread_SafeInteger.load(), 5);
}

TEST_F(Thread_Safe_Core, initializationViaAssignmentEquality) {
    Thread_SafeInteger = 5;
}

TEST_F(Thread_Safe_Core, initializationViaAssignmentBracesEquality) {
    Thread_SafeInteger = {5};
    ATOM_EQUALITY(Thread_SafeInteger, Thread_SafeInteger);
}

TEST_F(Thread_Safe_Core, equality) {
    Thread_SafeInteger.store(5);
    ATOM_EQUALITY(Thread_SafeInteger, Thread_SafeInteger);
}

TEST_F(Thread_Safe_Core, Multi_Lock_Modification) {
    thread_safe<int> a;
    thread_safe<int> b;
    a.lock.lock(b.lock);
    a = 5;
    b = 7;
    a.lock.unlock(b.lock);
    ASSERT_EQ(a, 5);
    ASSERT_EQ(b, 7);
}

#undef ATOM_EQUALITY

// if these work the rest should too, this is idiosm and is not required as it is assumed to work

class Thread_Safe_Maths : public ::testing::Test {
protected:
    thread_safe<int> Thread_SafeInteger {2};
};

TEST_F(Thread_Safe_Maths, additionViaPlusNumber) {
    ASSERT_EQ(Thread_SafeInteger + 1, 3);
    ASSERT_EQ(Thread_SafeInteger, 2);
}

TEST_F(Thread_Safe_Maths, additionViaPlusPlus) {
    ASSERT_EQ(Thread_SafeInteger++, 2);
    ASSERT_EQ(Thread_SafeInteger, 3);
}

TEST_F(Thread_Safe_Maths, additionViaPlusEqualNumber) {
    ASSERT_EQ(Thread_SafeInteger += 1, 3);
    ASSERT_EQ(Thread_SafeInteger, 3);
}

TEST_F(Thread_Safe_Maths, subtractionViaMinusNumber) {
    ASSERT_EQ(Thread_SafeInteger - 1, 1);
    ASSERT_EQ(Thread_SafeInteger, 2);
}

TEST_F(Thread_Safe_Maths, subtractionViaMinusMinus) {
    ASSERT_EQ(Thread_SafeInteger--, 2);
    ASSERT_EQ(Thread_SafeInteger, 1);
}

TEST_F(Thread_Safe_Maths, subtractionViaMinusEqualNumber) {
    ASSERT_EQ(Thread_SafeInteger -= 1, 1);
    ASSERT_EQ(Thread_SafeInteger, 1);
}

TEST_F(Thread_Safe_Maths, multiplicationViaMultiplierNumber) {
    ASSERT_EQ(Thread_SafeInteger * 2, 4);
    ASSERT_EQ(Thread_SafeInteger, 2);
}

TEST_F(Thread_Safe_Maths, multiplicationViaMultiplierEqualNumber) {
    ASSERT_EQ(Thread_SafeInteger *= 2, 4);
    ASSERT_EQ(Thread_SafeInteger, 4);
}

TEST_F(Thread_Safe_Maths, divisionViaDivisorNumber) {
    ASSERT_EQ(Thread_SafeInteger / 2, 1);
    ASSERT_EQ(Thread_SafeInteger, 2);
}

TEST_F(Thread_Safe_Maths, divisionViaDivisorEqualNumber) {
    ASSERT_EQ(Thread_SafeInteger /= 2, 1);
    ASSERT_EQ(Thread_SafeInteger, 1);
}

TEST_F(Thread_Safe_Maths, fetchAdd) {
    ASSERT_EQ(Thread_SafeInteger.fetch_add(1), 2);
    ASSERT_EQ(Thread_SafeInteger, 3);
}

TEST_F(Thread_Safe_Maths, fetchSub) {
    ASSERT_EQ(Thread_SafeInteger.fetch_sub(1), 2);
    ASSERT_EQ(Thread_SafeInteger, 1);
}

TEST_F(Thread_Safe_Maths, fetchMul) {
    ASSERT_EQ(Thread_SafeInteger.fetch_mul(2), 2);
    ASSERT_EQ(Thread_SafeInteger, 4);
}

TEST_F(Thread_Safe_Maths, fetchDiv) {
    ASSERT_EQ(Thread_SafeInteger.fetch_div(2), 2);
    ASSERT_EQ(Thread_SafeInteger, 1);
}

class Thread_Safe_Pointers : public ::testing::Test {
protected:
    thread_safe<int*> Thread_SafePointerToInteger;
    thread_safe<thread_safe<int>*> Thread_SafePointerToThread_SafeInteger;
};

TEST_F(Thread_Safe_Pointers, initializationViaAssignment) {
    Thread_SafePointerToInteger = new int;
    ASSERT_NE(Thread_SafePointerToInteger, nullptr);
    ASSERT_EQ(Thread_SafePointerToInteger.stored, true);
    ASSERT_NE(Thread_SafePointerToInteger.value, nullptr);
    Thread_SafePointerToInteger.value[0] = 5;
    ASSERT_EQ(Thread_SafePointerToInteger.value[0], 5);

    thread_safe<int*> Thread_SafePointerToInteger2 = new int;
    ASSERT_NE(Thread_SafePointerToInteger2, nullptr);
    ASSERT_EQ(Thread_SafePointerToInteger2.stored, true);
    ASSERT_NE(Thread_SafePointerToInteger2.value, nullptr);
    Thread_SafePointerToInteger2.value[0] = 5;
    ASSERT_EQ(Thread_SafePointerToInteger2.value[0], 5);

    ASSERT_NE(Thread_SafePointerToInteger, Thread_SafePointerToInteger2);
    ASSERT_NE(Thread_SafePointerToInteger.value, Thread_SafePointerToInteger2.value);
    ASSERT_EQ(Thread_SafePointerToInteger.value[0], Thread_SafePointerToInteger2.value[0]);

    Thread_SafePointerToThread_SafeInteger = new thread_safe<int>;
    ASSERT_NE(Thread_SafePointerToThread_SafeInteger, nullptr);
    ASSERT_EQ(Thread_SafePointerToThread_SafeInteger.stored, true);
    ASSERT_NE(Thread_SafePointerToThread_SafeInteger.value, nullptr);
    Thread_SafePointerToThread_SafeInteger.value[0] = 5;
    ASSERT_EQ(Thread_SafePointerToThread_SafeInteger.value->stored, true);
    ASSERT_EQ(Thread_SafePointerToThread_SafeInteger.value->value, 5);

    thread_safe<thread_safe<int>*> Thread_SafePointerToThread_SafeInteger2 = new thread_safe<int>;
    ASSERT_NE(Thread_SafePointerToThread_SafeInteger2, nullptr);
    ASSERT_EQ(Thread_SafePointerToThread_SafeInteger2.stored, true);
    ASSERT_NE(Thread_SafePointerToThread_SafeInteger2.value, nullptr);
    Thread_SafePointerToThread_SafeInteger2.value[0] = 5;
    ASSERT_EQ(Thread_SafePointerToThread_SafeInteger2.value->stored, true);
    ASSERT_EQ(Thread_SafePointerToThread_SafeInteger2.value->value, 5);

    ASSERT_NE(Thread_SafePointerToThread_SafeInteger, Thread_SafePointerToThread_SafeInteger2);
    ASSERT_NE(Thread_SafePointerToThread_SafeInteger.value, Thread_SafePointerToThread_SafeInteger2.value);
    ASSERT_EQ(Thread_SafePointerToThread_SafeInteger.value[0], Thread_SafePointerToThread_SafeInteger2.value[0]);
    ASSERT_EQ(Thread_SafePointerToThread_SafeInteger.value->value, Thread_SafePointerToThread_SafeInteger2.value->value);
    delete(Thread_SafePointerToInteger.value);
    delete(Thread_SafePointerToInteger2.value);
    delete(Thread_SafePointerToThread_SafeInteger.value);
    delete(Thread_SafePointerToThread_SafeInteger2.value);
}

TEST_F(Thread_Safe_Pointers, initializationViaAssignmentBraces) {
    Thread_SafePointerToInteger = { new int };
    ASSERT_NE(Thread_SafePointerToInteger, nullptr);
    ASSERT_EQ(Thread_SafePointerToInteger.stored, true);
    ASSERT_NE(Thread_SafePointerToInteger.value, nullptr);
    Thread_SafePointerToInteger.value[0] = 5;
    ASSERT_EQ(Thread_SafePointerToInteger.value[0], 5);

    thread_safe<int*> Thread_SafePointerToInteger2 = { new int };
    ASSERT_NE(Thread_SafePointerToInteger2, nullptr);
    ASSERT_EQ(Thread_SafePointerToInteger2.stored, true);
    ASSERT_NE(Thread_SafePointerToInteger2.value, nullptr);
    Thread_SafePointerToInteger2.value[0] = 5;
    ASSERT_EQ(Thread_SafePointerToInteger2.value[0], 5);

    ASSERT_NE(Thread_SafePointerToInteger, Thread_SafePointerToInteger2);
    ASSERT_NE(Thread_SafePointerToInteger.value, Thread_SafePointerToInteger2.value);
    ASSERT_EQ(Thread_SafePointerToInteger.value[0], Thread_SafePointerToInteger2.value[0]);

    Thread_SafePointerToThread_SafeInteger = { new thread_safe<int> };
    ASSERT_NE(Thread_SafePointerToThread_SafeInteger, nullptr);
    ASSERT_EQ(Thread_SafePointerToThread_SafeInteger.stored, true);
    ASSERT_NE(Thread_SafePointerToThread_SafeInteger.value, nullptr);
    Thread_SafePointerToThread_SafeInteger.value[0] = { 5 };
    ASSERT_EQ(Thread_SafePointerToThread_SafeInteger.value->stored, true);
    ASSERT_EQ(Thread_SafePointerToThread_SafeInteger.value->value, 5);

    thread_safe<thread_safe<int>*> Thread_SafePointerToThread_SafeInteger2 = { new thread_safe<int> };
    ASSERT_NE(Thread_SafePointerToThread_SafeInteger2, nullptr);
    ASSERT_EQ(Thread_SafePointerToThread_SafeInteger2.stored, true);
    ASSERT_NE(Thread_SafePointerToThread_SafeInteger2.value, nullptr);
    Thread_SafePointerToThread_SafeInteger2.value[0] = { 5 };
    ASSERT_EQ(Thread_SafePointerToThread_SafeInteger2.value->stored, true);
    ASSERT_EQ(Thread_SafePointerToThread_SafeInteger2.value->value, 5);

    ASSERT_NE(Thread_SafePointerToThread_SafeInteger, Thread_SafePointerToThread_SafeInteger2);
    ASSERT_NE(Thread_SafePointerToThread_SafeInteger.value, Thread_SafePointerToThread_SafeInteger2.value);
    ASSERT_EQ(Thread_SafePointerToThread_SafeInteger.value[0], Thread_SafePointerToThread_SafeInteger2.value[0]);
    ASSERT_EQ(Thread_SafePointerToThread_SafeInteger.value->value, Thread_SafePointerToThread_SafeInteger2.value->value);
    delete(Thread_SafePointerToInteger.value);
    delete(Thread_SafePointerToInteger2.value);
    delete(Thread_SafePointerToThread_SafeInteger.value);
    delete(Thread_SafePointerToThread_SafeInteger2.value);
}

TEST_F(Thread_Safe_Pointers, loadUnstored) {
    ASSERT_EQ(Thread_SafePointerToInteger.load(), nullptr);
    ASSERT_EQ(Thread_SafePointerToThread_SafeInteger.load(), nullptr);
}

TEST_F(Thread_Safe_Pointers, store) {
    Thread_SafePointerToInteger.store(new int);
    ASSERT_EQ(Thread_SafePointerToInteger.stored, true);
    ASSERT_NE(Thread_SafePointerToInteger.value, nullptr);
    delete (Thread_SafePointerToInteger.value);

    Thread_SafePointerToThread_SafeInteger.store(new thread_safe<int>);
    ASSERT_EQ(Thread_SafePointerToThread_SafeInteger.stored, true);
    ASSERT_NE(Thread_SafePointerToThread_SafeInteger.value, nullptr);
    delete (Thread_SafePointerToThread_SafeInteger.value);
}

TEST_F(Thread_Safe_Pointers, loadStored) {
    Thread_SafePointerToInteger.store(new int);
    ASSERT_NE(Thread_SafePointerToInteger.load(), nullptr);
    delete (Thread_SafePointerToInteger.value);

    Thread_SafePointerToThread_SafeInteger.store(new thread_safe<int>);
    ASSERT_NE(Thread_SafePointerToThread_SafeInteger.load(), nullptr);
    delete (Thread_SafePointerToThread_SafeInteger.value);
}

TEST_F(Thread_Safe_Pointers, addressAddition) {
    Thread_SafePointerToInteger.store(new int);
    int * initial = Thread_SafePointerToInteger.value;
    ASSERT_EQ(initial, Thread_SafePointerToInteger.value);
    ASSERT_EQ(Thread_SafePointerToInteger + 5, &initial[5]);
    ASSERT_EQ(Thread_SafePointerToInteger++, &initial[0]);
    ASSERT_EQ(initial+1, Thread_SafePointerToInteger.value);
    ASSERT_TRUE(Thread_SafePointerToInteger == initial+1);
}