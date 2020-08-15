#ifndef GAMMARAMP_H
#define GAMMARAMP_H

#include <mlengine_global.h>

#include <vector>
namespace mle{
struct MLENGINE_SHARED_EXPORT GammaRamp{
        std::vector<unsigned short> reds;
        std::vector<unsigned short> greens;
        std::vector<unsigned short> blues;
};
};
#endif