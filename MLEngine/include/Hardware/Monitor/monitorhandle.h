#ifndef MONITORHANDLE_H
#define MONITORHANDLE_H

#include <mlengine_global.h>

struct GLFWmonitor;
namespace mle{
class RenderingManager;
class MonitorData;
struct MLENGINE_SHARED_EXPORT MonitorHandle{
    MonitorHandle();  
        
    friend class MonitorData;
    friend class RenderingManager;
private:
    MonitorHandle(GLFWmonitor* in_monitor);
    GLFWmonitor* monitor;
};
};
#endif