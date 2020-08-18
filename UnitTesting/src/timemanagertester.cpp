#include <gmock/gmock.h>

#include <Engine/Time/timermanager.h>
#include <Engine/Time/timerhandle.h>

#include <iostream>
#include <limits>

static void setValue(double& toChange, const double value){
    toChange = value;
}

TEST(TimeManagerTest, checkInitValue){
    mle::TimerManager m;
    EXPECT_DOUBLE_EQ(0.0, m.timePassed());
}
TEST(TimeManagerTest, checkInitTimersValue){
    mle::TimerManager m;
    EXPECT_EQ(m.getCurrentTimersCount(), 0);
}

TEST(TimeManagerTest, addTimeCheck){
    mle::TimerManager m;
    m.advanceTime(10.0);
    EXPECT_DOUBLE_EQ(10.0, m.timePassed());
}
TEST(TimeManagerTest, addTimeCheck2){
    mle::TimerManager m;
    m.advanceTime(10.0);
    EXPECT_DOUBLE_EQ(10.0, m.timePassed());
    m.advanceTime(15.5);
    EXPECT_DOUBLE_EQ(25.5, m.timePassed());
}
TEST(TimeManagerTest, addTimeCheck3){
    mle::TimerManager m;
    m.advanceTime(0.0);
    EXPECT_DOUBLE_EQ(0.0, m.timePassed());
}
TEST(TimeManagerTest, addTimeCheckNegative){
    mle::TimerManager m;
    m.advanceTime(-10.0);
    EXPECT_DOUBLE_EQ(-10.0, m.timePassed());
}
TEST(TimeManagerTest, getRemainingTime1){
    mle::TimerManager m;
    EXPECT_DOUBLE_EQ(10.0, m.getRemainingTime(10.0));
}
TEST(TimeManagerTest, getRemainingTime2){
    mle::TimerManager m;
    m.advanceTime(15.9);
    EXPECT_DOUBLE_EQ(10.0, m.getRemainingTime(25.9));
}
TEST(TimeManagerTest, getRemainingTime3){
    mle::TimerManager m;
    m.advanceTime(0.0);
    EXPECT_DOUBLE_EQ(0.0, m.getRemainingTime(0.0));
}
TEST(TimeManagerTest, getRemainingTime4){
    mle::TimerManager m;
    m.advanceTime(10.0);
    EXPECT_DOUBLE_EQ(0.0, m.getRemainingTime(10.0));
}
TEST(TimeManagerTest, getRemainingTime5){
    mle::TimerManager m;
    m.advanceTime(11.0);
    EXPECT_DOUBLE_EQ(-1.0, m.getRemainingTime(10.0));
}
TEST(TimeManagerTest, getExpectedTimeoutTime1){
    mle::TimerManager m;
    EXPECT_DOUBLE_EQ(10.0, m.getExpectedTimeout(10.0));
}
TEST(TimeManagerTest, getExpectedTimeoutTime2){
    mle::TimerManager m;
    m.advanceTime(15.9);
    EXPECT_DOUBLE_EQ(25.9, m.getExpectedTimeout(10.0));
}
TEST(TimeManagerTest, getExpectedTimeoutTime3){
    mle::TimerManager m;
    m.advanceTime(0.0);
    EXPECT_DOUBLE_EQ(0.0, m.getExpectedTimeout(0.0));
}
TEST(TimeManagerTest, getExpectedTimeoutTime4){
    mle::TimerManager m;
    m.advanceTime(10.0);
    EXPECT_DOUBLE_EQ(20.0, m.getExpectedTimeout(10.0));
}
TEST(TimeManagerTest, getExpectedTimeoutTime5){
    mle::TimerManager m;
    m.advanceTime(-5.0);
    EXPECT_DOUBLE_EQ(5.0, m.getExpectedTimeout(10.0));
}
TEST(TimeManagerTest, getExpectedTimeoutTime6){
    mle::TimerManager m;
    m.advanceTime(-5.0);
    EXPECT_DOUBLE_EQ(-10.0, m.getExpectedTimeout(-5.0));
}
TEST(TimeManagerTest, getFirstTimerRemainingTime1){
    mle::TimerManager m;
    EXPECT_DOUBLE_EQ(std::numeric_limits<double>::max(), m.getRemainingTimeForNextTimer());
}
TEST(TimeManagerTest, getFirstTimerRemainingTime2){
    mle::TimerManager m;
    m.advanceTime(15.9);
    EXPECT_DOUBLE_EQ(std::numeric_limits<double>::max(), m.getRemainingTimeForNextTimer());
}
TEST(TimeManagerTest, getFirstTimerRemainingTime3){
    mle::TimerManager m;
    m.addTimer(1.0, nullptr);
    m.advanceTime(0.0);
    EXPECT_DOUBLE_EQ(1.0, m.getRemainingTimeForNextTimer());
}
TEST(TimeManagerTest, getFirstTimerRemainingTime4){
    mle::TimerManager m;
    m.addTimer(9.0, nullptr);
    m.advanceTime(10.0);
    EXPECT_DOUBLE_EQ(std::numeric_limits<double>::max(), m.getRemainingTimeForNextTimer());
}
TEST(TimeManagerTest, getFirstTimerRemainingTime5){
    mle::TimerManager m;
    m.addTimer(0.0, nullptr);
    m.advanceTime(-5.0);
    EXPECT_DOUBLE_EQ(5.0, m.getRemainingTimeForNextTimer());
}
TEST(TimeManagerTest, getFirstTimerRemainingTime6){
    mle::TimerManager m;
    m.addTimer(-5.0, nullptr);
    m.advanceTime(-5.0);
    EXPECT_DOUBLE_EQ(std::numeric_limits<double>::max(), m.getRemainingTimeForNextTimer());
}
TEST(TimeManagerTest, getFirstTimerRemainingTime7){
    mle::TimerManager m;
    m.addTimer(0.0, nullptr);
    m.addTimer(1.0, nullptr);
    m.addTimer(2.0, nullptr);
    m.advanceTime(0.0);
    EXPECT_DOUBLE_EQ(1.0, m.getRemainingTimeForNextTimer());
}
TEST(TimeManagerTest, getFirstTimerRemainingTime8){
    mle::TimerManager m;
    m.addTimer(0.0, nullptr);
    m.addTimer(1.0, nullptr);
    m.addTimer(2.0, nullptr);
    EXPECT_DOUBLE_EQ(0.0, m.getRemainingTimeForNextTimer());
}
TEST(TimeManagerTest, getFirstTimerRemainingTime9){
    mle::TimerManager m;
    m.addTimer(0.0, nullptr);
    m.addTimer(1.0, nullptr);
    m.addTimer(2.0, nullptr);
    m.addTimer(2.0, nullptr);
    m.addTimer(3.0, nullptr);
    m.addTimer(3.1, nullptr);
    m.addTimer(4.0, nullptr);
    EXPECT_NEAR(0.0, m.getRemainingTimeForNextTimer(), 0.0000001);
    m.advanceTime(0.0);
    EXPECT_NEAR(1.0, m.getRemainingTimeForNextTimer(), 0.0000001);
    m.advanceTime(1.0);
    EXPECT_NEAR(1.0, m.getRemainingTimeForNextTimer(), 0.0000001);
    m.advanceTime(1.5);
    EXPECT_NEAR(0.5, m.getRemainingTimeForNextTimer(), 0.0000001);
}
TEST(TimeManagerTest, getFirstTimerRemainingTime10){
    mle::TimerManager m;
    m.addTimer(0.0, nullptr);
    m.addTimer(1.0, nullptr);
    m.addTimer(2.0, nullptr);
    m.addTimer(2.0, nullptr);
    m.addTimer(3.0, nullptr);
    m.addTimer(3.1, nullptr);
    m.addTimer(4.0, nullptr);
    EXPECT_NEAR(0.0, m.getRemainingTimeForNextTimer(), 0.0000001);
    m.advanceTime(0.0);
    EXPECT_NEAR(1.0, m.getRemainingTimeForNextTimer(), 0.0000001);
    m.advanceTime(1.0);
    EXPECT_NEAR(1.0, m.getRemainingTimeForNextTimer(), 0.0000001);
    m.advanceTime(1.5);
    EXPECT_NEAR(0.5, m.getRemainingTimeForNextTimer(), 0.0000001);
    m.advanceTime(0.5);
    EXPECT_NEAR(0.1, m.getRemainingTimeForNextTimer(), 0.0000001);
}
TEST(TimeManagerTest, getFirstTimerRemainingTime11){
    mle::TimerManager m;
    m.addTimer(0.0, nullptr);
    m.addTimer(1.0, nullptr);
    m.addTimer(2.0, nullptr);
    m.addTimer(2.0, nullptr);
    m.addTimer(3.0, nullptr);
    m.addTimer(3.1, nullptr);
    m.addTimer(4.0, nullptr);
    EXPECT_NEAR(0.0, m.getRemainingTimeForNextTimer(), 0.0000001);
    m.advanceTime(0.0);
    EXPECT_NEAR(1.0, m.getRemainingTimeForNextTimer(), 0.0000001);
    m.advanceTime(1.0);
    EXPECT_NEAR(1.0, m.getRemainingTimeForNextTimer(), 0.0000001);
    m.advanceTime(1.5);
    EXPECT_NEAR(0.5, m.getRemainingTimeForNextTimer(), 0.0000001);
    m.advanceTime(0.5);
    EXPECT_NEAR(0.1, m.getRemainingTimeForNextTimer(), 0.0000001);
    m.advanceTime(1.0);
    EXPECT_DOUBLE_EQ(std::numeric_limits<double>::max(), m.getRemainingTimeForNextTimer());
}
TEST(TimeManagerTest, getFirstTimerRemainingTime12){
    mle::TimerManager m;
    m.addTimer(2.0, nullptr);
    EXPECT_DOUBLE_EQ(2.0, m.getRemainingTimeForNextTimer());
    m.addTimer(1.0, nullptr);
    EXPECT_DOUBLE_EQ(1.0, m.getRemainingTimeForNextTimer());
    m.addTimer(0.0, nullptr);
    EXPECT_DOUBLE_EQ(0.0, m.getRemainingTimeForNextTimer());
    m.advanceTime(0.0);
    EXPECT_DOUBLE_EQ(1.0, m.getRemainingTimeForNextTimer());
}
TEST(TimeManagerTest, addTimerCheckSize){
    mle::TimerManager m;
    m.addTimer(1.0, nullptr);
    EXPECT_EQ(m.getCurrentTimersCount(), 1);
}
TEST(TimeManagerTest, addTimerCheckSize2){
    mle::TimerManager m;
    m.addTimer(1.0, nullptr);
    m.addTimer(1.0, nullptr);
    m.addTimer(1.0, nullptr);
    m.addTimer(1.0, nullptr);
    EXPECT_EQ(m.getCurrentTimersCount(), 4);
}
TEST(TimeManagerTest, addTimerCheckTriggerSize1){
    mle::TimerManager m;
    m.addTimer(1.0, nullptr);
    m.addTimer(2.0, nullptr);
    m.addTimer(3.0, nullptr);
    m.addTimer(4.0, nullptr);

    m.advanceTime(0.5);
    EXPECT_EQ(m.getCurrentTimersCount(), 4);
}
TEST(TimeManagerTest, addTimerCheckTriggerSize2){
    mle::TimerManager m;
    m.addTimer(1.0, nullptr);
    m.addTimer(2.0, nullptr);
    m.addTimer(3.0, nullptr);
    m.addTimer(4.0, nullptr);

    m.advanceTime(2.5);
    EXPECT_EQ(m.getCurrentTimersCount(), 2);
}
TEST(TimeManagerTest, addTimerCheckTriggerSize3){
    mle::TimerManager m;
    m.addTimer(1.0, nullptr);
    m.addTimer(2.0, nullptr);
    m.addTimer(3.0, nullptr);
    m.addTimer(4.0, nullptr);

    m.advanceTime(4.5);
    EXPECT_EQ(m.getCurrentTimersCount(), 0);
}
TEST(TimeManagerTest, addTimerCheckTriggerFunc1){
    mle::TimerManager m;
    double value = -1.0;
    std::function<void(void)> a = [&value]() {value = 0.0;};
    std::function<void(void)> b = [&value]() {value = 1.0;};
    std::function<void(void)> c = [&value]() {value = 2.0;};
    std::function<void(void)> d = [&value]() {value = 3.0;};

    m.addTimer(1.0, a);
    m.addTimer(2.0, b);
    m.addTimer(3.0, c);
    m.addTimer(4.0, d);

    m.advanceTime(0.5);

    EXPECT_DOUBLE_EQ(-1.0, value);
}
TEST(TimeManagerTest, addTimerCheckTriggerFunc2){
    mle::TimerManager m;
    double value1 = -1.0;
    double value2 = -1.0;
    double value3 = -1.0;
    double value4 = -1.0;

    std::function<void(void)> a = [&]() {value1 = 0.0;};
    std::function<void(void)> b = [&]() {value2 = 1.0;};
    std::function<void(void)> c = [&]() {value3 = 2.0;};
    std::function<void(void)> d = [&]() {value4 = 3.0;};

    m.addTimer(2.0, b);
    m.addTimer(3.0, c);
    m.addTimer(1.0, a);
    EXPECT_DOUBLE_EQ(1.0, m.getRemainingTime(1.0));
    EXPECT_DOUBLE_EQ(1.0, m.getRemainingTime(1.0));
    m.addTimer(4.0, d);

    m.advanceTime(1.5);

    EXPECT_EQ(3, m.getCurrentTimersCount());
    EXPECT_DOUBLE_EQ(-0.5, m.getRemainingTime(1.0));
    EXPECT_DOUBLE_EQ(0.5, m.getRemainingTimeForNextTimer());
    EXPECT_DOUBLE_EQ(0.0, value1);
    EXPECT_DOUBLE_EQ(-1.0, value2);
    EXPECT_DOUBLE_EQ(-1.0, value3);
    EXPECT_DOUBLE_EQ(-1.0, value4);
}
TEST(TimeManagerTest, addTimerCheckTriggerFunc3){
    mle::TimerManager m;
    double value1 = -1.0;
    double value2 = -1.0;
    double value3 = -1.0;
    double value4 = -1.0;
    m.addTimer(2.0, [&value1]() {value1 = 0.0;});
    m.addTimer(1.0, [&value2]() {value2 = 1.0;});
    m.addTimer(3.0, [&value3]() {value3 = 2.0;});
    m.addTimer(4.0, [&value4]() {value4 = 3.0;});

    m.advanceTime(2.5);
    EXPECT_EQ(2, m.getCurrentTimersCount());
    EXPECT_DOUBLE_EQ(-0.5, m.getRemainingTime(2.0));
    EXPECT_DOUBLE_EQ(0.5, m.getRemainingTimeForNextTimer());
    EXPECT_DOUBLE_EQ(0.0, value1);
    EXPECT_DOUBLE_EQ(1.0, value2);
    EXPECT_DOUBLE_EQ(-1.0, value3);
    EXPECT_DOUBLE_EQ(-1.0, value4);
}
TEST(TimeManagerTest, addTimerCheckTriggerFunc4){
    mle::TimerManager m;
    double value1 = -1.0;
    double value2 = -1.0;
    double value3 = -1.0;
    double value4 = -1.0;
    m.addTimer(1.0, [&value1]() {value1 = 0.0;});
    m.addTimer(2.0, [&value2]() {value2 = 1.0;});
    m.addTimer(3.0, [&value3]() {value3 = 2.0;});
    m.addTimer(4.0, [&value4]() {value4 = 3.0;});

    m.advanceTime(3.5);
    EXPECT_EQ(1, m.getCurrentTimersCount());
    EXPECT_DOUBLE_EQ(0.5, m.getRemainingTimeForNextTimer());
    EXPECT_DOUBLE_EQ(0.0, value1);
    EXPECT_DOUBLE_EQ(1.0, value2);
    EXPECT_DOUBLE_EQ(2.0, value3);
    EXPECT_DOUBLE_EQ(-1.0, value4);
}
TEST(TimeManagerTest, addTimerCheckTriggerFunc5){
    mle::TimerManager m;
    double value1 = -1.0;
    double value2 = -1.0;
    double value3 = -1.0;
    double value4 = -1.0;
    m.addTimer(1.0, [&value1]() {value1 = 0.0;});
    m.addTimer(2.0, [&value2]() {value2 = 1.0;});
    m.addTimer(3.0, [&value3]() {value3 = 2.0;});
    m.addTimer(4.0, [&value4]() {value4 = 3.0;});

    m.advanceTime(4.5);
    EXPECT_EQ(0, m.getCurrentTimersCount());
    EXPECT_DOUBLE_EQ(std::numeric_limits<double>::max(), m.getRemainingTimeForNextTimer());
    EXPECT_DOUBLE_EQ(0.0, value1);
    EXPECT_DOUBLE_EQ(1.0, value2);
    EXPECT_DOUBLE_EQ(2.0, value3);
    EXPECT_DOUBLE_EQ(3.0, value4);
}
TEST(TimeManagerTest, addTimerCheckTriggerFunc6){
    mle::TimerManager m;
    double value6 = -1.0;
    m.addTimer(1.0, std::move([&value6]() {value6 = 0.0;}));

    m.advanceTime(4.5);
    EXPECT_EQ(0, m.getCurrentTimersCount());
    EXPECT_DOUBLE_EQ(std::numeric_limits<double>::max(), m.getRemainingTimeForNextTimer());
    EXPECT_DOUBLE_EQ(0.0, value6);
}
