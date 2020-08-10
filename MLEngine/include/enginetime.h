#ifndef TIME_H
#define TIME_H

#include <mlengine_global.h>

namespace mle{
class MLENGINE_SHARED_EXPORT EngineTime{
public:
    static void now(char* output, const size_t max_size);
};
}
#endif