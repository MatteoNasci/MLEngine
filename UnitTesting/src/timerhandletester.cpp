#include <gmock/gmock.h>

#include <Engine/Time/timerhandle.h>

#include <functional>
#include <iostream>

TEST(TimerHandleTest, checkInitializationValue){
    mle::TimerHandle handle;
    EXPECT_DOUBLE_EQ(handle.triggerAt, 0.0);
    EXPECT_FALSE(handle.onTimeout);
}
TEST(TimerHandleTest, checkInitializationCustomValue){
    mle::TimerHandle handle(10.0, std::function<void(void)>());
    EXPECT_DOUBLE_EQ(handle.triggerAt, 10.0);
    EXPECT_FALSE(handle.onTimeout);
}
TEST(TimerHandleTest, checkInitializationCustomFunc){
    mle::TimerHandle handle(10.0, [](){int i = 5;});
    EXPECT_DOUBLE_EQ(handle.triggerAt, 10.0);
    EXPECT_TRUE(handle.onTimeout);
}
TEST(TimerHandleTest, checkIsGreaterThan1){
    mle::TimerHandle handle1(10.0, std::function<void(void)>());
    mle::TimerHandle handle2(15.0, std::function<void(void)>());
    EXPECT_FALSE(handle1 > handle2);
}
TEST(TimerHandleTest, checkIsGreaterThan2){
    mle::TimerHandle handle1(15.0, std::function<void(void)>());
    mle::TimerHandle handle2(15.0, std::function<void(void)>());
    EXPECT_FALSE(handle1 > handle2);
}
TEST(TimerHandleTest, checkIsGreaterThan3){
    mle::TimerHandle handle1(10.0, std::function<void(void)>());
    mle::TimerHandle handle2(9.0, std::function<void(void)>());
    EXPECT_TRUE(handle1 > handle2);
}
TEST(TimerHandleTest, checkIsGreaterThan4){
    mle::TimerHandle handle1(-55.3, std::function<void(void)>());
    mle::TimerHandle handle2(-55, std::function<void(void)>());
    EXPECT_FALSE(handle1 > handle2);
}

TEST(TimerHandleTest, checkIsLessThan1){
    mle::TimerHandle handle1(10.0, std::function<void(void)>());
    mle::TimerHandle handle2(15.0, std::function<void(void)>());
    EXPECT_TRUE(handle1 < handle2);
}
TEST(TimerHandleTest, checkIsLessThan2){
    mle::TimerHandle handle1(15.0, std::function<void(void)>());
    mle::TimerHandle handle2(15.0, std::function<void(void)>());
    EXPECT_FALSE(handle1 < handle2);
}
TEST(TimerHandleTest, checkIsLessThan3){
    mle::TimerHandle handle1(10.0, std::function<void(void)>());
    mle::TimerHandle handle2(9.0, std::function<void(void)>());
    EXPECT_FALSE(handle1 < handle2);
}
TEST(TimerHandleTest, checkIsLessThan4){
    mle::TimerHandle handle1(-55.3, std::function<void(void)>());
    mle::TimerHandle handle2(-55, std::function<void(void)>());
    EXPECT_TRUE(handle1 < handle2);
}