#ifndef TIMEMANAGER_H
#define TIMEMANAGER_H

#include <mlengine_global.h>

#include <timerhandle.h>

#include <queue>
#include <vector>

namespace mle{
class MLENGINE_SHARED_EXPORT TimeManager{
public:
    TimeManager();
    ~TimeManager();
    void advanceTime(const double timeToAdd);
    double timePassed() const;

    size_t getCurrentTimersCount() const;
    double getRemainingTime(const double timeout) const;
    double getRemainingTimeForNextTimer() const;
    double getExpectedTimeout(const double duration) const;
    template<typename T>
    void addTimer(const double duration, T onTimeout){
        std::function<void(void)> func = onTimeout;
        pushTimer(duration, func);
    }
private:
    void checkTimers();
    void pushTimer(const double duration, std::function<void(void)> onTimeout);
private:
    double m_time;
    std::priority_queue<TimerHandle, std::vector<TimerHandle>, std::greater<TimerHandle>> m_timers;
};
};
#endif