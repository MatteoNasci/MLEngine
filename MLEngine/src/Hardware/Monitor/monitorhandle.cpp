#include <Hardware/Monitor/monitorhandle.h>

using namespace mle;
MonitorHandle::MonitorHandle() : MonitorHandle(nullptr){

}
MonitorHandle::MonitorHandle(GLFWmonitor* in_monitor) : monitor(in_monitor){

}