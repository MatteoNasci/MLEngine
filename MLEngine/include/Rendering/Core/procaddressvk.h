#ifndef PROCADDRESSVK_H
#define PROCADDRESSVK_H

#include <mlengine_global.h>
#include <Rendering/Core/windowsharedata.h>

#include <functional>

namespace mle{
struct MLENGINE_SHARED_EXPORT ProcAddressVk{
    std::function<void(void)> address;
};
};
#endif