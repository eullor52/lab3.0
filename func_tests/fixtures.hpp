#pragma once
#include <gtest/gtest.h>
#include "../headers/uniq_ptr.hpp"
#include "../headers/shared_ptr.hpp"
#include <utility>

namespace {

struct TrackedObject {
    static int instances;
    int value;
    explicit TrackedObject(int v = 0) : value(v) { ++instances; }
    ~TrackedObject() { --instances; }
};
int TrackedObject::instances = 0;

struct Point {
    int x;
    int y;
    Point(int a, int b) : x(a), y(b) {}
};

}

class UniqPtrFixture : public testing::Test {
    protected:
        int* number;
        char* character;
        int* array;
        UniqPtr<int> uniqInt;
        UniqPtr<char> uniqChar;
        UniqPtr<int[]> uniqArray;

        void SetUp() override {
            number = new int(1);
            character = new char('1');
            array = new int[3]{1, 2, 3};
            uniqInt.reset(number);
            uniqChar.reset(character);
            uniqArray.reset(array);
        }
};

class UniqPtrLeakFixture : public testing::Test {
    protected:
        void SetUp() override {
            TrackedObject::instances = 0;
        }
};

class SharedPtrFixture : public testing::Test {
    protected:
        int* number;
        char* character;
        int* array;
        SharedPtr<int> sharedInt;
        SharedPtr<char> sharedChar;
        SharedPtr<int[]> sharedArray;

        void SetUp() override {
            number = new int(1);
            character = new char('1');
            array = new int[3]{1, 2, 3};
            sharedInt.reset(number);
            sharedChar.reset(character);
            sharedArray.reset(array);
        }
};

class SharedPtrLeakFixture : public testing::Test {
    protected:
        void SetUp() override {
            TrackedObject::instances = 0;
        }
};