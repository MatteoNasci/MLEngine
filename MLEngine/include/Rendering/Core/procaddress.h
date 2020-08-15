#ifndef PROCADDRESS_H
#define PROCADDRESS_H

#include <mlengine_global.h>
#include <Rendering/Core/windowsharedata.h>

#include <functional>

namespace mle{
struct MLENGINE_SHARED_EXPORT ProcAddress{
    std::function<void(void)> address;
    WindowShareData context;
};
};
#endif