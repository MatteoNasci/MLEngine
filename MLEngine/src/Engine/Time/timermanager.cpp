#include <Engine/Time/timermanager.h>

#include <limits>

using namespace mle;
TimerManager::TimerManager() : m_time(0.0), m_timers(){

}
TimerManager::~TimerManager(){

}
void TimerManager::advanceTime(const double timeToAdd){
    m_time += timeToAdd;
    checkTimers();
}
double TimerManager::timePassed() const{
    return m_time;
}
double TimerManager::getRemainingTime(const double timeout) const{
    return timeout - timePassed();
}
void TimerManager::addTimer(const double duration, std::function<void(void)> onTimeout){
    m_timers.emplace(getExpectedTimeout(duration), onTimeout);
}
double TimerManager::getExpectedTimeout(const double duration) const{
    return duration + timePassed();
}
size_t TimerManager::getCurrentTimersCount() const{
    return m_timers.size();
}
double TimerManager::getRemainingTimeForNextTimer() const{
    if(m_timers.empty()){
        return std::numeric_limits<double>::max();
    }
    return getRemainingTime(m_timers.cbegin()->triggerAt);
}
bool TimerManager::isNextTimerOver() const{
    if(m_timers.empty()){
        return false;
    }
    return m_timers.cbegin()->triggerAt <= timePassed();
}
void TimerManager::checkTimers(){
    while(isNextTimerOver()){
        const auto& timer_it = m_timers.cbegin();
        const auto& timer = *timer_it;

        if(timer.onTimeout){
            timer.onTimeout();
        }

        m_timers.erase(timer_it);
    }
}