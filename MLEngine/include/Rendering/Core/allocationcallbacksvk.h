#ifndef ALLOCATIONCALLBACKSVK_H
#define ALLOCATIONCALLBACKSVK_H

/*typedef struct VkAllocationCallbacks {
    void *            pUserData;
    PFN_vkAllocationFunction     pfnAllocation;
    PFN_vkReallocationFunction   pfnReallocation;
    PFN_vkFreeFunction      pfnFree;
    PFN_vkInternalAllocationNotification   pfnInternalAllocation;
    PFN_vkInternalFreeNotification   pfnInternalFree;
} VkAllocationCallbacks;*/

#include <mlengine_global.h>
#include <Rendering/Core/systemallocationscopevk.h>
#include <Rendering/Core/internalallocationtypevk.h>
namespace mle{
struct MLENGINE_SHARED_EXPORT AllocationCallbacksVk{
    void* userData;
    void (*allocationFunc)(void* user_data, size_t size, size_t alignment, SystemAllocationScopeVk scope);
    void (*reallocationFunc)(void* user_data, void* original, size_t size, size_t alignment, SystemAllocationScopeVk scope);
    void (*freeFunc)(void* user_data, void* memory);
    void (*internalAllocationNotification)(void* user_data, size_t size, InternalAllocationTypeVk type, SystemAllocationScopeVk scope);
    void (*internalFreeNotification)(void* user_data, size_t size, InternalAllocationTypeVk type, SystemAllocationScopeVk);
};
};
#endif