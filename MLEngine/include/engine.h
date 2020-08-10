#ifndef ENGINE_H
#define ENGINE_H

#include <mlengine_global.h>

namespace mle{
class MLENGINE_SHARED_EXPORT Engine{
public:
    bool init();

    static Engine& instance();
private: Engine();
};
};
#endif