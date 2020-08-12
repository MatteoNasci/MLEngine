#include <gmock/gmock.h>
#include <enginetime.h>

TEST(EngineTimeTest, checkNowInvalidOutput){
    EXPECT_NO_THROW({
        mle::EngineTime::now(nullptr, 0);
    });
}
TEST(EngineTimeTest, checkNowInvalidOutputSize){
    EXPECT_NO_THROW({
        char a[2];
        a[0] = 'a';
        a[1] = 'b';
        mle::EngineTime::now(a, 0);
        EXPECT_EQ(a[0], 'a');
        EXPECT_EQ(a[1], 'b');
    });
}
TEST(EngineTimeTest, checkNowSmallOutputSize){
    EXPECT_NO_THROW({
        const size_t size = 5;
        char a[size];
        a[0] = 'a';
        mle::EngineTime::now(a, size);
        EXPECT_EQ(a[0], '\0');
    });
}
TEST(EngineTimeTest, checkNowNormalInput){
    EXPECT_NO_THROW({
        const size_t size = 100;
        char a[size];
        a[0] = 'd';
        mle::EngineTime::now(a, size);
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