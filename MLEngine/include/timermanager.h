#ifndef TIMERMANAGER_H
#define TIMERMANAGER_H

#include <mlengine_global.h>

#include <timerhandle.h>

#include <set>
#include <vector>

namespace mle{
class MLENGINE_SHARED_EXPORT TimerManager{
public:
    TimerManager();
    ~TimerManager();
    void advanceTime(const double timeToAdd);
    double timePassed() const;

    size_t getCurrentTimersCount() const;
    double getRemainingTime(const double timeout) const;
    double getRemainingTimeForNextTimer() const;
    double getExpectedTimeout(const double duration) const;
    void addTimer(const double duration, std::function<void(void)> onTimeout);
private:
    void checkTimers();
private:
    double m_time;
    std::multiset<TimerHandle> m_timers;
};
};
#endif