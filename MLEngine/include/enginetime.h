#ifndef TIME_H
#define TIME_H

#include <mlengine_global.h>

#include <ctime>
#include <chrono>

namespace mle{
class MLENGINE_SHARED_EXPORT EngineTime{
public:
    static void now(char* output, const size_t max_size);
    static time_t extractTime(const std::chrono::system_clock::time_point& timePoint);
    static std::chrono::system_clock::time_point nowPoint();
};
}
#endif