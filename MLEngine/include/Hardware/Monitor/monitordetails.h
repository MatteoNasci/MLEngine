#ifndef MONITORDETAILS_H
#define MONITORDETAILS_H

#include <mlengine_global.h>

#include <Hardware/Monitor/gammaramp.h>
#include <Hardware/Monitor/monitormode.h>

#include <string>
#include <vector>
namespace mle{
struct MLENGINE_SHARED_EXPORT MonitorDetails{
    GammaRamp gammaRamp;
    MonitorMode mode;
    std::vector<MonitorMode> modes;
    std::string name;
    void* userData;
    int positionX, positionY;
    int workareaPositionX, workareaPositionY, workareaWidth, workareaHeight;
    int widthMM, heightMM;
    float scaleX, scaleY;
    size_t index;
};
};
#endif