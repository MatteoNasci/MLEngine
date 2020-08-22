#ifndef CONTEXTINITDATA_H
#define CONTEXTINITDATA_H

#include <mlengine_global.h>
#include <Rendering/Core/windowsharedata.h>

#include <vector>
namespace mle{
struct MLENGINE_SHARED_EXPORT ContextInitData{
    WindowShareData window;
    std::string appName;
    std::vector<const char*> vulkan_layers;
    std::vector<const char*> vulkan_logical_device_extensions;
    bool vulkan_use_layers;
    uint32_t appVersionMajor, appVersionMinor, appRevision;
};
};

#endif