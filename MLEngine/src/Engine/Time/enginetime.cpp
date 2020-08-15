#include <Engine/Time/enginetime.h>

#define __STDC_WANT_LIB_EXT1__ 1
#include <chrono>
#include <ctime>
#include <cstring>

using namespace mle;
void EngineTime::nowString(char* output, const size_t max_size){
    EngineTime::timeString(EngineTime::extractTime(EngineTime::now<std::chrono::system_clock>()), output, max_size);
}
void EngineTime::timeString(const time_t& time, char* output, const size_t max_size){
    if(!max_size || output == nullptr){
        return;
    }
    output[0] = '\0';

    static const size_t min_size = 26;
    if(max_size >= min_size){ 
        if(ctime_s(output, max_size, &time)){
            static const char error_output[] = "No valid time found!";
            static const size_t error_output_size = (std::strlen(error_output) + 1);
            if(max_size >= error_output_size){
#ifdef __STDC_LIB_EXT1__
                std::strcpy_s(output, error_output_size, error_output);
#else
                std::strcpy(output, error_output);
#endif
            }
        }
    }
}
time_t EngineTime::extractTime(const std::chrono::system_clock::time_point& timePoint){
    return std::chrono::system_clock::to_time_t(timePoint);
}
