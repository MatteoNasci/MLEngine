#ifndef CONTEXTINITDATA_H
#define CONTEXTINITDATA_H

#include <mlengine_global.h>

#include <vector>
namespace mle{
struct MLENGINE_SHARED_EXPORT ContextInitData{
std::vector<const char*> vulkan_layers;
bool vulkan_use_layers;
};
};

#endif