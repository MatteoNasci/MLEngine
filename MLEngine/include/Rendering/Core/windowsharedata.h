#ifndef WINDOWSHAREDATA_H
#define WINDOWSHAREDATA_H

#include <mlengine_global.h>

struct GLFWwindow;
namespace mle{
class RenderingManager;
struct MLENGINE_SHARED_EXPORT WindowShareData{
    WindowShareData();
    friend class RenderingManager;
    friend class VulkanHandler;
private:
    WindowShareData(GLFWwindow* in_window);
    GLFWwindow* window;
};
};
#endif