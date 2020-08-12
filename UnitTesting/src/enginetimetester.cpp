#include <gmock/gmock.h>
#include <enginetime.h>
#include <chrono>

TEST(EngineTimeTest, checkNowInvalidOutput){
    EXPECT_NO_THROW({
        mle::EngineTime::nowString(nullptr, 0);
    });
}
TEST(EngineTimeTest, checkNowInvalidOutputSize){
    EXPECT_NO_THROW({
        char a[2];
        a[0] = 'a';
        a[1] = 'b';
        mle::EngineTime::nowString(a, 0);
        EXPECT_EQ(a[0], 'a');
        EXPECT_EQ(a[1], 'b');
    });
}
TEST(EngineTimeTest, checkNowSmallOutputSize){
    EXPECT_NO_THROW({
        const size_t size = 5;
        char a[size];
        a[0] = 'a';
        mle::EngineTime::nowString(a, size);
        EXPECT_EQ(a[0], '\0');
    });
}
TEST(EngineTimeTest, checkNowNormalInput){
    EXPECT_NO_THROW({
        const size_t size = 100;
        char a[size];
        a[0] = 'd';
        mle::EngineTime::nowString(a, size);
        EXPECT_FALSE(a[0] == '\0');
        EXPECT_FALSE(a[0] == 'd');
    });
}

TEST(EngineTimeTest, checkTimeStringInvalidOutput){
    EXPECT_NO_THROW({
        mle::EngineTime::timeString(time_t(0), nullptr, 0);
    });
}
TEST(EngineTimeTest, checkTimeStringInvalidOutputSize){
    EXPECT_NO_THROW({
        char a[2];
        a[0] = 'a';
        a[1] = 'b';
        mle::EngineTime::timeString(time_t(0), a, 0);
        EXPECT_EQ(a[0], 'a');
        EXPECT_EQ(a[1], 'b');
    });
}
TEST(EngineTimeTest, checkTimeStringSmallOutputSize){
    EXPECT_NO_THROW({
        const size_t size = 5;
        char a[size];
        a[0] = 'a';
        mle::EngineTime::timeString(time_t(0), a, size);
        EXPECT_EQ(a[0], '\0');
    });
}
TEST(EngineTimeTest, checkTimeStringNormalInput){
    EXPECT_NO_THROW({
        const size_t size = 100;
        char a[size];
        a[0] = 'd';
        mle::EngineTime::timeString(time_t(0), a, size);
        EXPECT_FALSE(a[0] == '\0');
        EXPECT_FALSE(a[0] == 'd');
    });
}
TEST(EngineTimeTest, checkTimeStringNegativeTimeInput){
    EXPECT_NO_THROW({
        const size_t size = 100;
        char a[size];
        a[0] = 'd';
        mle::EngineTime::timeString(time_t(-100000), a, size);
        EXPECT_FALSE(a[0] == '\0');
        EXPECT_FALSE(a[0] == 'd');
    });
}
TEST(EngineTimeTest, checkTimeStringPositiveTimeInput){
    EXPECT_NO_THROW({
        const size_t size = 100;
        char a[size];
        a[0] = 'd';
        mle::EngineTime::timeString(time_t(100000), a, size);
        EXPECT_FALSE(a[0] == '\0');
        EXPECT_FALSE(a[0] == 'd');
    });
}
TEST(EngineTimeTest, checkPreciseIntervallZero){
    auto a = mle::EngineTime::getTimeFromOffset<std::chrono::steady_clock::time_point>(std::chrono::microseconds(0));
    EXPECT_DOUBLE_EQ(0.0, (std::chrono::duration<double, std::micro>(a - a).count()));
    EXPECT_DOUBLE_EQ(0.0, mle::EngineTime::getInterval<std::micro>(a, a));
}
TEST(EngineTimeTest, checkPreciseIntervallValid){
    auto a = mle::EngineTime::getTimeFromOffset<std::chrono::steady_clock::time_point>(std::chrono::microseconds(0));
    auto b = mle::EngineTime::getTimeFromOffset<std::chrono::steady_clock::time_point>(std::chrono::microseconds(10000));
    EXPECT_DOUBLE_EQ(10000.0, (std::chrono::duration<double, std::micro>(b - a).count()));
    EXPECT_DOUBLE_EQ(10000.0, mle::EngineTime::getInterval<std::micro>(a, b));
}
TEST(EngineTimeTest, checkPreciseIntervallValid2){
    auto a = mle::EngineTime::getTimeFromOffset<std::chrono::steady_clock::time_point>(std::chrono::microseconds(50));
    auto b = mle::EngineTime::getTimeFromOffset<std::chrono::steady_clock::time_point>(std::chrono::microseconds(10000));
    EXPECT_DOUBLE_EQ(9950.0, (std::chrono::duration<double, std::micro>(b - a).count()));
    EXPECT_DOUBLE_EQ(9950.0, mle::EngineTime::getInterval<std::micro>(a, b));
}
TEST(EngineTimeTest, checkPreciseIntervallValidInverted){
    auto a = mle::EngineTime::getTimeFromOffset<std::chrono::steady_clock::time_point>(std::chrono::microseconds(0));
    auto b = mle::EngineTime::getTimeFromOffset<std::chrono::steady_clock::time_point>(std::chrono::microseconds(550));
    EXPECT_DOUBLE_EQ(-550.0, (std::chrono::duration<double, std::micro>(a - b).count()));
    EXPECT_DOUBLE_EQ(-550.0, mle::EngineTime::getInterval<std::micro>(b, a));
}
TEST(EngineTimeTest, checkPreciseNowSequence){
    auto a = mle::EngineTime::now<std::chrono::steady_clock>();
    auto b = mle::EngineTime::now<std::chrono::steady_clock>();
    EXPECT_TRUE(b >= a);
}
TEST(EngineTimeTest, checkGetTimeCountZero){
    auto a = mle::EngineTime::getTimeCount<std::micro>(std::chrono::steady_clock::time_point() + std::chrono::microseconds(0));
    EXPECT_DOUBLE_EQ(0.0, a);
}
TEST(EngineTimeTest, checkGetTimeCountNonZero){
    auto a = mle::EngineTime::getTimeCount<std::micro>(std::chrono::steady_clock::time_point() + std::chrono::microseconds(5687));
    EXPECT_DOUBLE_EQ(5687.0, a);
}
TEST(EngineTimeTest, checkGetTimeCountNegative){
    auto a = mle::EngineTime::getTimeCount<std::micro>(std::chrono::steady_clock::time_point() + std::chrono::microseconds(-5687));
    EXPECT_DOUBLE_EQ(-5687.0, a);
}
TEST(EngineTimeTest, checkGetTimeOffZero){
    std::chrono::steady_clock::time_point a = mle::EngineTime::getTimeFromOffset<std::chrono::steady_clock::time_point>(std::chrono::microseconds(0));
    double c = (std::chrono::duration<double, std::micro>(a.time_since_epoch()).count());
    EXPECT_DOUBLE_EQ(0.0, c);
    EXPECT_DOUBLE_EQ(0.0, mle::EngineTime::getTimeCount<std::micro>(a));
}
TEST(EngineTimeTest, checkGetTimeOffNonZero){
    std::chrono::steady_clock::time_point a = mle::EngineTime::getTimeFromOffset<std::chrono::steady_clock::time_point>(std::chrono::microseconds(5687));
    double c = (std::chrono::duration<double, std::micro>(a.time_since_epoch()).count());
    EXPECT_DOUBLE_EQ(5687.0, c);
    EXPECT_DOUBLE_EQ(5687.0, mle::EngineTime::getTimeCount<std::micro>(a));
}
TEST(EngineTimeTest, checkGetTimeOffNegative){
    std::chrono::steady_clock::time_point a = mle::EngineTime::getTimeFromOffset<std::chrono::steady_clock::time_point>(std::chrono::microseconds(-5687));
    double c = (std::chrono::duration<double, std::micro>(a.time_since_epoch()).count());
    EXPECT_DOUBLE_EQ(-5687.0, c);
    EXPECT_DOUBLE_EQ(-5687.0, mle::EngineTime::getTimeCount<std::micro>(a));
}
