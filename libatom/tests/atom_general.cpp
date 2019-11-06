//
// Created by brothercomplex on 6/11/19.
//

#include <atom/atom.h>
#include <gtest/gtest.h>

class core : public ::testing::Test {
protected:
    atom<int> AtomInteger;
};

#define ATOM_EQUALITY(AtomInteger1, AtomInteger2) int five = 5; \
const int fiveConst = 5; \
EXPECT_EQ(AtomInteger1, AtomInteger2); \
EXPECT_EQ(AtomInteger2, AtomInteger1); \
EXPECT_EQ(AtomInteger1, 5); \
EXPECT_EQ(5, AtomInteger1); \
EXPECT_EQ(AtomInteger1, five); \
EXPECT_EQ(five, AtomInteger1); \
EXPECT_EQ(AtomInteger1, fiveConst); \
EXPECT_EQ(fiveConst, AtomInteger1); \
EXPECT_TRUE(AtomInteger1 == AtomInteger2); \
EXPECT_TRUE(AtomInteger2 == AtomInteger1); \
EXPECT_TRUE(AtomInteger1 == 5); \
EXPECT_TRUE(5 == AtomInteger1); \
EXPECT_TRUE(AtomInteger1 == five); \
EXPECT_TRUE(five == AtomInteger1); \
EXPECT_TRUE(AtomInteger1 == fiveConst); \
EXPECT_TRUE(fiveConst == AtomInteger1)

TEST_F(core, initializationViaAssignment) {
    AtomInteger = 5;
    EXPECT_EQ(AtomInteger.stored, true);
    EXPECT_EQ(AtomInteger.value, 5);
    atom <int> AtomInteger2 = 5;
    EXPECT_EQ(AtomInteger2.stored, true);
    EXPECT_EQ(AtomInteger2.value, 5);
}

TEST_F(core, initializationViaAssignmentBraces) {
    AtomInteger = {5};
    EXPECT_EQ(AtomInteger.stored, true);
    EXPECT_EQ(AtomInteger.value, 5);
    atom <int> AtomInteger2 {5};
    EXPECT_EQ(AtomInteger2.stored, true);
    EXPECT_EQ(AtomInteger2.value, 5);
}

TEST_F(core, loadUnstored) {
    EXPECT_THROW(AtomInteger.load(), atomExceptions::NoValueStored);
}

TEST_F(core, store) {
    AtomInteger.store(5);
    EXPECT_EQ(AtomInteger.stored, true);
    EXPECT_EQ(AtomInteger.value, 5);
}

TEST_F(core, loadStored) {
    AtomInteger.store(5);
    EXPECT_EQ(AtomInteger.load(), 5);
}

TEST_F(core, initializationViaAssignmentEquality) {
    AtomInteger = 5;
    ATOM_EQUALITY(AtomInteger, AtomInteger);
}

TEST_F(core, initializationViaAssignmentBracesEquality) {
    AtomInteger = {5};
    ATOM_EQUALITY(AtomInteger, AtomInteger);
}

TEST_F(core, equality) {
    AtomInteger.store(5);
    ATOM_EQUALITY(AtomInteger, AtomInteger);
}

#undef ATOM_EQUALITY

// if these work the rest should too, this is idiosm and is not required as it is assumed to work

class maths : public ::testing::Test {
protected:
    atom<int> AtomInteger {2};
};

TEST_F(maths, additionViaPlusNumber) {
    EXPECT_EQ(AtomInteger + 1, 3);
    EXPECT_EQ(AtomInteger, 2);
}

TEST_F(maths, additionViaPlusPlus) {
    EXPECT_EQ(AtomInteger++, 2);
    EXPECT_EQ(AtomInteger, 3);
}

TEST_F(maths, additionViaPlusEqualNumber) {
    EXPECT_EQ(AtomInteger += 1, 3);
    EXPECT_EQ(AtomInteger, 3);
}

TEST_F(maths, subtractionViaMinusNumber) {
    EXPECT_EQ(AtomInteger - 1, 1);
    EXPECT_EQ(AtomInteger, 2);
}

TEST_F(maths, subtractionViaMinusMinus) {
    EXPECT_EQ(AtomInteger--, 2);
    EXPECT_EQ(AtomInteger, 1);
}

TEST_F(maths, subtractionViaMinusEqualNumber) {
    EXPECT_EQ(AtomInteger -= 1, 1);
    EXPECT_EQ(AtomInteger, 1);
}

TEST_F(maths, multiplicationViaMultiplierNumber) {
    EXPECT_EQ(AtomInteger * 2, 4);
    EXPECT_EQ(AtomInteger, 2);
}

TEST_F(maths, multiplicationViaMultiplierEqualNumber) {
    EXPECT_EQ(AtomInteger *= 2, 4);
    EXPECT_EQ(AtomInteger, 4);
}

TEST_F(maths, divisionViaDivisorNumber) {
    EXPECT_EQ(AtomInteger / 2, 1);
    EXPECT_EQ(AtomInteger, 2);
}

TEST_F(maths, divisionViaDivisorEqualNumber) {
    EXPECT_EQ(AtomInteger /= 2, 1);
    EXPECT_EQ(AtomInteger, 1);
}

TEST_F(maths, fetchAdd) {
    EXPECT_EQ(AtomInteger.fetch_add(1), 2);
    EXPECT_EQ(AtomInteger, 3);
}

TEST_F(maths, fetchSub) {
    EXPECT_EQ(AtomInteger.fetch_sub(1), 2);
    EXPECT_EQ(AtomInteger, 1);
}

TEST_F(maths, fetchMul) {
    EXPECT_EQ(AtomInteger.fetch_mul(2), 2);
    EXPECT_EQ(AtomInteger, 4);
}

TEST_F(maths, fetchDiv) {
    EXPECT_EQ(AtomInteger.fetch_div(2), 2);
    EXPECT_EQ(AtomInteger, 1);
}

class pointers : public ::testing::Test {
protected:
    atom<int*> AtomPointerToInteger;
    atom<atom<int>*> AtomPointerToAtomInteger;
};

TEST_F(pointers, initializationViaAssignment) {
    AtomPointerToInteger = new int;
    ASSERT_NE(AtomPointerToInteger, nullptr);
    EXPECT_EQ(AtomPointerToInteger.stored, true);
    ASSERT_NE(AtomPointerToInteger.value, nullptr);
    AtomPointerToInteger.value[0] = 5;
    EXPECT_EQ(AtomPointerToInteger.value[0], 5);

    atom<int*> AtomPointerToInteger2 = new int;
    ASSERT_NE(AtomPointerToInteger2, nullptr);
    EXPECT_EQ(AtomPointerToInteger2.stored, true);
    ASSERT_NE(AtomPointerToInteger2.value, nullptr);
    AtomPointerToInteger2.value[0] = 5;
    EXPECT_EQ(AtomPointerToInteger2.value[0], 5);

    EXPECT_NE(AtomPointerToInteger, AtomPointerToInteger2);
    EXPECT_NE(AtomPointerToInteger.value, AtomPointerToInteger2.value);
    EXPECT_EQ(AtomPointerToInteger.value[0], AtomPointerToInteger2.value[0]);

    AtomPointerToAtomInteger = new atom<int>;
    ASSERT_NE(AtomPointerToAtomInteger, nullptr);
    EXPECT_EQ(AtomPointerToAtomInteger.stored, true);
    ASSERT_NE(AtomPointerToAtomInteger.value, nullptr);
    AtomPointerToAtomInteger.value[0] = 5;
    EXPECT_EQ(AtomPointerToAtomInteger.value->stored, true);
    EXPECT_EQ(AtomPointerToAtomInteger.value->value, 5);

    atom<atom<int>*> AtomPointerToAtomInteger2 = new atom<int>;
    ASSERT_NE(AtomPointerToAtomInteger2, nullptr);
    EXPECT_EQ(AtomPointerToAtomInteger2.stored, true);
    ASSERT_NE(AtomPointerToAtomInteger2.value, nullptr);
    AtomPointerToAtomInteger2.value[0] = 5;
    EXPECT_EQ(AtomPointerToAtomInteger2.value->stored, true);
    EXPECT_EQ(AtomPointerToAtomInteger2.value->value, 5);

    EXPECT_NE(AtomPointerToAtomInteger, AtomPointerToAtomInteger2);
    EXPECT_NE(AtomPointerToAtomInteger.value, AtomPointerToAtomInteger2.value);
    EXPECT_EQ(AtomPointerToAtomInteger.value[0], AtomPointerToAtomInteger2.value[0]);
    EXPECT_EQ(AtomPointerToAtomInteger.value->value, AtomPointerToAtomInteger2.value->value);
    delete(AtomPointerToInteger.value);
    delete(AtomPointerToInteger2.value);
    delete(AtomPointerToAtomInteger.value);
    delete(AtomPointerToAtomInteger2.value);
}

TEST_F(pointers, initializationViaAssignmentBraces) {
    AtomPointerToInteger = { new int };
    ASSERT_NE(AtomPointerToInteger, nullptr);
    EXPECT_EQ(AtomPointerToInteger.stored, true);
    ASSERT_NE(AtomPointerToInteger.value, nullptr);
    AtomPointerToInteger.value[0] = 5;
    EXPECT_EQ(AtomPointerToInteger.value[0], 5);

    atom<int*> AtomPointerToInteger2 = { new int };
    ASSERT_NE(AtomPointerToInteger2, nullptr);
    EXPECT_EQ(AtomPointerToInteger2.stored, true);
    ASSERT_NE(AtomPointerToInteger2.value, nullptr);
    AtomPointerToInteger2.value[0] = 5;
    EXPECT_EQ(AtomPointerToInteger2.value[0], 5);

    EXPECT_NE(AtomPointerToInteger, AtomPointerToInteger2);
    EXPECT_NE(AtomPointerToInteger.value, AtomPointerToInteger2.value);
    EXPECT_EQ(AtomPointerToInteger.value[0], AtomPointerToInteger2.value[0]);

    AtomPointerToAtomInteger = { new atom<int> };
    ASSERT_NE(AtomPointerToAtomInteger, nullptr);
    EXPECT_EQ(AtomPointerToAtomInteger.stored, true);
    ASSERT_NE(AtomPointerToAtomInteger.value, nullptr);
    AtomPointerToAtomInteger.value[0] = { 5 };
    EXPECT_EQ(AtomPointerToAtomInteger.value->stored, true);
    EXPECT_EQ(AtomPointerToAtomInteger.value->value, 5);

    atom<atom<int>*> AtomPointerToAtomInteger2 = { new atom<int> };
    ASSERT_NE(AtomPointerToAtomInteger2, nullptr);
    EXPECT_EQ(AtomPointerToAtomInteger2.stored, true);
    ASSERT_NE(AtomPointerToAtomInteger2.value, nullptr);
    AtomPointerToAtomInteger2.value[0] = { 5 };
    EXPECT_EQ(AtomPointerToAtomInteger2.value->stored, true);
    EXPECT_EQ(AtomPointerToAtomInteger2.value->value, 5);

    EXPECT_NE(AtomPointerToAtomInteger, AtomPointerToAtomInteger2);
    EXPECT_NE(AtomPointerToAtomInteger.value, AtomPointerToAtomInteger2.value);
    EXPECT_EQ(AtomPointerToAtomInteger.value[0], AtomPointerToAtomInteger2.value[0]);
    EXPECT_EQ(AtomPointerToAtomInteger.value->value, AtomPointerToAtomInteger2.value->value);
    delete(AtomPointerToInteger.value);
    delete(AtomPointerToInteger2.value);
    delete(AtomPointerToAtomInteger.value);
    delete(AtomPointerToAtomInteger2.value);
}

TEST_F(pointers, loadUnstored) {
    EXPECT_EQ(AtomPointerToInteger.load(), nullptr);
    EXPECT_EQ(AtomPointerToAtomInteger.load(), nullptr);
}

TEST_F(pointers, store) {
    AtomPointerToInteger.store(new int);
    EXPECT_EQ(AtomPointerToInteger.stored, true);
    EXPECT_NE(AtomPointerToInteger.value, nullptr);
    delete (AtomPointerToInteger.value);

    AtomPointerToAtomInteger.store(new atom<int>);
    EXPECT_EQ(AtomPointerToAtomInteger.stored, true);
    EXPECT_NE(AtomPointerToAtomInteger.value, nullptr);
    delete (AtomPointerToAtomInteger.value);
}

TEST_F(pointers, loadStored) {
    AtomPointerToInteger.store(new int);
    EXPECT_NE(AtomPointerToInteger.load(), nullptr);
    delete (AtomPointerToInteger.value);

    AtomPointerToAtomInteger.store(new atom<int>);
    EXPECT_NE(AtomPointerToAtomInteger.load(), nullptr);
    delete (AtomPointerToAtomInteger.value);
}

TEST_F(pointers, addressAddition) {
    AtomPointerToInteger.store(new int);
    int * initial = AtomPointerToInteger.value;
    EXPECT_EQ(initial, &AtomPointerToInteger.value[0]);
    EXPECT_EQ(AtomPointerToInteger + 5, &initial[5]);
    EXPECT_EQ(AtomPointerToInteger++, &initial[0]);
    EXPECT_EQ(AtomPointerToInteger, &initial[1]);
}