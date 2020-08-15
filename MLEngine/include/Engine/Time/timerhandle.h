#ifndef TIMERHANDLE_H
#define TIMERHANDLE_H

#include <mlengine_global.h>
#include <functional>

namespace mle{
struct MLENGINE_SHARED_EXPORT TimerHandle{
    double triggerAt;
    std::function<void(void)> onTimeout;

    TimerHandle();
    TimerHandle(const double in_triggerAt, std::function<void(void)> in_onTimeout);
    TimerHandle(const TimerHandle& rhs);
    TimerHandle(TimerHandle&& rhs);

    TimerHandle& operator=(const TimerHandle& rhs);
    TimerHandle& operator=(TimerHandle&& rhs);

    friend bool operator<(const TimerHandle& lhs, const TimerHandle& rhs){
        return lhs.triggerAt < rhs.triggerAt;
    }
    friend bool operator>(const TimerHandle& lhs, const TimerHandle& rhs){
        return lhs.triggerAt > rhs.triggerAt;
    }
};
};
#endif