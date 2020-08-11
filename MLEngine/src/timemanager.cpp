#include <timemanager.h>

#include <limits>

using namespace mle;
TimeManager::TimeManager() : m_time(0.0), m_timers(){

}
TimeManager::~TimeManager(){

}
void TimeManager::advanceTime(const double timeToAdd){
    m_time += timeToAdd;
    checkTimers();
}
double TimeManager::timePassed() const{
    return m_time;
}
double TimeManager::getRemainingTime(const double timeout) const{
    return timeout - timePassed();
}
void TimeManager::addTimer(const double duration, std::function<void(void)> onTimeout){
    m_timers.emplace(getExpectedTimeout(duration), onTimeout);
}
double TimeManager::getExpectedTimeout(const double duration) const{
    return duration + timePassed();
}
size_t TimeManager::getCurrentTimersCount() const{
    return m_timers.size();
}
double TimeManager::getRemainingTimeForNextTimer() const{
    if(m_timers.empty()){
        return std::numeric_limits<double>::max();
    }
    return getRemainingTime(m_timers.cbegin()->triggerAt);
}
void TimeManager::checkTimers(){
    while(getRemainingTimeForNextTimer() <= 0.0){
        const auto& timer_it = m_timers.cbegin();
        const auto& timer = *timer_it;

        if(timer.onTimeout){
            timer.onTimeout();
        }

        m_timers.erase(timer_it);
    }
}