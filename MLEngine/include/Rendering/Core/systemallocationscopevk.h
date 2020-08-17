#ifndef SYSTEMALLOCATIONSCOPEVK_H
#define SYSTEMALLOCATIONSCOPEVK_H

namespace mle{
enum class SystemAllocationScopeVk{
    SystemAllocationScopeCommand = 0,
    SystemAllocationScopeObject = 1,
    SystemAllocationScopeCache = 2,
    SystemAllocationScopeDevice = 3,
    SystemAllocationScopeInstance = 4
};
};

#endif