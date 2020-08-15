#include <Engine/Time/timerhandle.h>

using namespace mle;

TimerHandle::TimerHandle() : TimerHandle(0.0, std::function<void(void)>()) {}
TimerHandle::TimerHandle(const double in_triggerAt, std::function<void(void)> in_onTimeout) : 
    triggerAt(in_triggerAt),
    onTimeout(in_onTimeout)
{

}
TimerHandle::TimerHandle(TimerHandle&& rhs) : 
    triggerAt(rhs.triggerAt),
    onTimeout(rhs.onTimeout)
{

}
TimerHandle::TimerHandle(const TimerHandle& rhs) :
    triggerAt(rhs.triggerAt),
    onTimeout(rhs.onTimeout)
{

}
TimerHandle& TimerHandle::operator=(const TimerHandle& rhs){
    triggerAt = rhs.triggerAt;
    onTimeout = rhs.onTimeout;
    return *this;
}
TimerHandle& TimerHandle::operator=(TimerHandle&& rhs){
    triggerAt = rhs.triggerAt;
    onTimeout = rhs.onTimeout;
    return *this;
}
