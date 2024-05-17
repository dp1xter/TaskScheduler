#include "gtest/gtest.h"
#include "task_scheduler.h"
#include <cmath>
#include <algorithm>

using namespace TaskScheduler;

TEST(ArgumentLessTaskTestSuite, OneSimpleTaskTest) {
    TTaskScheduler scheduler;

    auto i1 = scheduler.add([]() {return 1;});

    ASSERT_EQ(scheduler.getResult<int>(i1), 1);
}

TEST(ArgumentLessTaskTestSuite, TaskWithFutureResultTest) {
    TTaskScheduler scheduler;

    auto i1 = scheduler.add([]() {return std::string("123");});
    auto i2 = scheduler.add([](const std::string& s) {return s + "45";}, scheduler.getFutureResult<std::string>(i1));

    ASSERT_EQ(scheduler.getResult<std::string>(i2), "12345");
}

TEST(ArgumentLessTaskTestSuite, ManyNestedTasksTest) {
    TTaskScheduler scheduler;

    auto i1 = scheduler.add([]() {return 1;});
    auto i2 = scheduler.add([](int a) {return a + 2;}, scheduler.getFutureResult<int>(i1));
    auto i3 = scheduler.add([](int a, int b) {return a + b;}, scheduler.getFutureResult<int>(i1), scheduler.getFutureResult<int>(i2));

    ASSERT_EQ(scheduler.getResult<int>(i3), 4);
}

TEST(UnaryTaskTestSuite, OneSimpleTaskTest) {
    TTaskScheduler scheduler;

    auto i1 = scheduler.add([](int a) {return a * 2;}, 2);

    ASSERT_EQ(scheduler.getResult<int>(i1), 4);
}

TEST(UnaryTaskTestSuite, TaskWithFutureResultTest) {
    TTaskScheduler scheduler;

    auto i1 = scheduler.add([](int a) {return std::string(a, 'a');}, 5);
    auto i2 = scheduler.add([](const std::string& s) {return s + "bc";}, scheduler.getFutureResult<std::string>(i1));

    ASSERT_EQ(scheduler.getResult<std::string>(i2), "aaaaabc");
}

TEST(UnaryTaskTestSuite, ManyNestedTasksTest) {
    TTaskScheduler scheduler;

    auto i1 = scheduler.add([](const std::string& s) {return s;}, "a");
    auto i2 = scheduler.add([](const std::string& s) {return s + 'b';}, scheduler.getFutureResult<std::string>(i1));
    auto i3 = scheduler.add([](const std::string& s) {return s + 'c';}, scheduler.getFutureResult<std::string>(i2));
    auto i4 = scheduler.add([](const std::string& s) {return s + 'd';}, scheduler.getFutureResult<std::string>(i3));
    auto i5 = scheduler.add([](const std::string& s) {return s + 'e';}, scheduler.getFutureResult<std::string>(i4));

    ASSERT_EQ(scheduler.getResult<std::string>(i5), "abcde");
}

TEST(BinaryTaskTestSuite, OneSimpleTaskTest) {
    TTaskScheduler scheduler;

    auto i1 = scheduler.add([](int a, int b) {return a + b;}, 300, 200);

    ASSERT_EQ(scheduler.getResult<int>(i1), 500);
}

TEST(BinaryTaskTestSuite, TaskWithFutureResultTest) {
    TTaskScheduler scheduler;

    auto i1 = scheduler.add([](char a, int b) {return std::string(b, a);}, 'x', 3);
    auto i2 = scheduler.add([](const std::string& s, int count) {return std::count(s.begin(), s.end(), 'x') == count;}, scheduler.getFutureResult<std::string>(i1), 3);

    ASSERT_TRUE(scheduler.getResult<bool>(i2));
}

TEST(BinaryTaskTestSuite, ManyNestedTasksTest) {
    float a = 1;
    float b = -2;
    float c = 0;

    TTaskScheduler scheduler;

    auto i1 = scheduler.add([](float a, float c) {return -4 * a * c;}, a, c);
    auto i2 = scheduler.add([](float b, float v) {return b * b + v;}, b, scheduler.getFutureResult<float>(i1));
    auto i3 = scheduler.add([](float b, float d) {return -b + std::sqrt(d);}, b, scheduler.getFutureResult<float>(i2));
    auto i4 = scheduler.add([](float b, float d) {return -b - std::sqrt(d);}, b, scheduler.getFutureResult<float>(i2));
    auto i5 = scheduler.add([](float a, float v) {return v / (2 * a);}, a, scheduler.getFutureResult<float>(i3));
    auto i6 = scheduler.add([](float a, float v) {return v / (2 * a);}, a, scheduler.getFutureResult<float>(i4));

    ASSERT_TRUE(scheduler.getResult<float>(i5) == 2 && scheduler.getResult<float>(i6) == 0);
}

TEST(MixedTaskTestSuite, MathWithStringTest) {
    TTaskScheduler scheduler;

    auto i1 = scheduler.add([](int x) {return static_cast<int> (log2(x));}, 9);
    auto i2 = scheduler.add([](int x, int y) {return (x * y) / (x + y);}, scheduler.getFutureResult<int>(i1), 6);
    auto i3 = scheduler.add([](int a, char b) {return std::string(a, b);}, scheduler.getFutureResult<int>(i2), 'x');

    ASSERT_EQ(scheduler.getResult<std::string>(i3), "xx");
}

TEST(MixedTaskTestSuite, FractionOperationsTest) {
    TTaskScheduler scheduler;

    auto i1 = scheduler.add([](double a, double b) {return a / b;}, 10.0, 2.0);
    auto i2 = scheduler.add([](double base, double exp) {return std::pow(base, exp);}, scheduler.getFutureResult<double>(i1), 2.0);
    auto i3 = scheduler.add([](double value) {return std::round(value);}, scheduler.getFutureResult<double>(i2));

    ASSERT_EQ(scheduler.getResult<double>(i3), 25.0);
}

TEST(MixedTaskTestSuite, StringOperationsTest) {
    TTaskScheduler scheduler;

    auto i1 = scheduler.add([](const std::string& s1, const std::string& s2) {return s1 + s2;}, "Hello, ", "world!");
    auto i2 = scheduler.add([](const std::string& s) {
        std::string result = s;
        std::transform(result.begin(), result.end(), result.begin(), ::toupper);
        return result;
    }, scheduler.getFutureResult<std::string>(i1));
    auto i3 = scheduler.add([](const std::string& s) {return s.length();}, scheduler.getFutureResult<std::string>(i2));

    ASSERT_EQ(scheduler.getResult<std::size_t>(i3), 13);
}

TEST(HeavyTestSuite, MatrixOperationsTest) {
    TTaskScheduler scheduler;

    using Matrix = std::vector<std::vector<int>>;
    Matrix mat1 = {{1, 2}, {3, 4}};

    auto i1 = scheduler.add([](const Matrix& m1, const Matrix& m2) {
        Matrix result(2, std::vector<int>(2));
        for (int i = 0; i < 2; ++i) {
            for (int j = 0; j < 2; ++j) {
                result[i][j] = m1[i][j] + m2[i][j];
            }
        }
        return result;
    }, mat1, Matrix{{5, 6}, {7, 8}});

    auto i2 = scheduler.add([](const Matrix& m) {
        Matrix result(2, std::vector<int>(2));
        for (int i = 0; i < 2; ++i) {
            for (int j = 0; j < 2; ++j) {
                result[j][i] = m[i][j];
            }
        }
        return result;
    }, scheduler.getFutureResult<Matrix>(i1));

    Matrix expectedTransposed = {{6, 10}, {8, 12}};

    ASSERT_EQ(scheduler.getResult<Matrix>(i2), expectedTransposed);
}

TEST(HeavyTestSuite, CollectionOperationsTest) {
    TTaskScheduler scheduler;

    std::vector<int> list1 = {1, 2, 3, 4};
    std::vector<int> list2 = {5, 6, 7, 8};

    auto i1 = scheduler.add([](const std::vector<int>& l1, const std::vector<int>& l2) {
        std::vector<int> result = l1;
        result.insert(result.end(), l2.begin(), l2.end());
        return result;
    }, list1, list2);

    auto i2 = scheduler.add([](const std::vector<int>& lst) {
        return *std::max_element(lst.begin(), lst.end());
    }, scheduler.getFutureResult<std::vector<int>>(i1));

    ASSERT_EQ(scheduler.getResult<int>(i2), 8);
}

TEST(HeavyTestSuite, ComplexTaskSequenceTest) {
    TTaskScheduler scheduler;

    auto i1 = scheduler.add([](int x) { return x + 5; }, 10);
    auto i2 = scheduler.add([](int x) { return x * 2; }, scheduler.getFutureResult<int>(i1));
    auto i3 = scheduler.add([](double x) { return x / 2.0; }, 20.0);
    auto i4 = scheduler.add([](double x) { return x * 3; }, scheduler.getFutureResult<double>(i3));
    auto i5 = scheduler.add([](const std::string& s1, const std::string& s2) { return s1 + s2; }, "Hello, ", "world!");

    auto i6 = scheduler.add([](const std::string& s) {
        std::string result = s;
        std::transform(result.begin(), result.end(), result.begin(), ::toupper);
        return result;
    }, scheduler.getFutureResult<std::string>(i5));

    auto i7 = scheduler.add([](int i, double d) {
        return std::to_string(i) + " " + std::to_string(d);
    }, scheduler.getFutureResult<int>(i2), scheduler.getFutureResult<double>(i4));

    auto i8 = scheduler.add([](const std::string& s1, const std::string& s2) {
        return s1 + " " + s2;
    }, scheduler.getFutureResult<std::string>(i6), scheduler.getFutureResult<std::string>(i7));

    ASSERT_EQ(scheduler.getResult<std::string>(i8), "HELLO, WORLD! 30 30.000000");
}