#ifndef TIME_H
#define TIME_H

#include <mlengine_global.h>

#include <ctime>
#include <chrono>

namespace mle{
class MLENGINE_SHARED_EXPORT EngineTime{
public:
    static void timeString(const time_t& time, char* output, const size_t max_size);
    static void nowString(char* output, const size_t max_size);
    static time_t extractTime(const std::chrono::system_clock::time_point& timePoint);

    template<typename Time>
    static auto now(){
        return Time::now();
    }
    template<typename TimeUnit, typename Time>
    static double getInterval(const Time& start, const Time& end){
        return std::chrono::duration<double, TimeUnit>(end - start).count();
    }
    template<typename TimeUnit, typename Time>
    static double getTimeCount(const Time& time){
        return std::chrono::duration<double, TimeUnit>(time.time_since_epoch()).count();
    }
    template<typename Time, typename OffsetUnit>
    static Time getTimeFromOffset(const OffsetUnit& sinceStart){        
        return (Time() + sinceStart);
    }
};
}
#endif