#ifndef MONITORMODE_H
#define MONITORMODE_H

#include <mlengine_global.h>
namespace mle{
struct MLENGINE_SHARED_EXPORT MonitorMode{
        int width, height, redBits, greenBits, blueBits, refreshRate;
};
};
#endif