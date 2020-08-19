#ifndef RENDERINGMANAGER_H
#define RENDERINGMANAGER_H

#include <mlengine_global.h>

#include <Rendering/Core/windowhintsdata.h>
#include <Rendering/Core/windowsharedata.h>
#include <Hardware/Monitor/monitorhandle.h>
#include <Rendering/Core/generalvalues.h>
#include <Rendering/Core/imagedata.h>
#include <Rendering/Core/windowprofileapi.h>
#include <Rendering/Core/procaddress.h>
#include <Rendering/Core/renderinginitdata.h>
#include <Engine/Core/engineerror.h>
#include <Rendering/Core/windowattribute.h>
#include <Rendering/Core/renderingcontexttype.h>

#include <Rendering/Core/contextinitdata.h>

#include <vector>
#include <string>
#include <mutex>
#include <functional>
#include <array>
#include <list>
//TODO: fare solo engine singleton, se ho bisogno di altri singleton evito e li faccio friend di engine con ctor privato e niente copy o move

//TODO: use glad for opengl extensions. https://www.glfw.org/docs/latest/context_guide.html#context_glext
struct GLFWwindow;
namespace mle{//TODO: choose between directx 12 and vulkan (vulkan is cross platform, may be more interesting)
class MLENGINE_SHARED_EXPORT RenderingManager{ //TODO: test all
//TODO: associate callbacks to glfw
public:
    static GeneralValues constexpr dontCareValue(){
        return GeneralValues::DontCare;
    }

    EngineError getWindowAttribute(const WindowShareData& window, const WindowAttribute attribute, int& out_value) const;

    bool isInitialized() const;
    bool isContextInitialized() const;
    bool isRunningLoop() const;
    void stopLoop();
    void enableLoop();
    static EngineError getAndClearError();
    EngineError init(const RenderingInitData& data);
    EngineError release();
    EngineError singleLoop();
    size_t getWindowsCount() const;
    EngineError addWindow(const std::string& title, const int width, const int height, const MonitorHandle& monitor_data, const WindowShareData& share, const WindowHintsData& hints, ContextInitData& context_data);
    EngineError enableFullscreenMode(const WindowShareData& window, const MonitorHandle& monitor, const bool enable, const int posX, const int posY, const int width, const int height, const int refresh_rate) const;
    
    EngineError getWindowCurrentMonitor(const WindowShareData& window, MonitorHandle& out_monitor) const;
    EngineError iconifyWindow(const WindowShareData& window) const;
    EngineError restoreWindow(const WindowShareData& window) const;
    EngineError setWindowTitle(const WindowShareData& window, const std::string& new_title) const;
    EngineError setWindowPosition(const WindowShareData& window, const int x, const int y) const;
    EngineError getWindowPosition(const WindowShareData& window, int& out_x, int& out_y) const;
    EngineError setWindowSize(const WindowShareData& window, const int width, const int height) const;
    EngineError getWindowSize(const WindowShareData& window, int& out_width, int& out_height) const;
    EngineError getWindowFrameSize(const WindowShareData& window, int& out_left, int& out_top, int& out_right, int& out_bottom) const;
    EngineError getWindowFrabebufferSize(const WindowShareData& window, int& out_pixels_width, int& out_pixels_height) const;
    EngineError setWindowShouldClose(const WindowShareData& window, const bool should_close) const;
    EngineError showWindow(const WindowShareData& window) const;
    EngineError hideWindow(const WindowShareData& window) const;
    EngineError setWindowUserPointer(const WindowShareData& window, void* pointer) const;
    EngineError getWindowShouldClose(const WindowShareData& window, bool& out_result) const;
    EngineError getWindowContentScale(const WindowShareData& window, float& out_scaleX, float& out_scaleY) const;
    EngineError setWindowAspectRation(const WindowShareData& window, const int width, const int height) const;
    EngineError setWindowSizeLimits(const WindowShareData& window, const int min_width, const int min_height, const int max_width, const int max_height) const;
    EngineError getWindowUserPointer(const WindowShareData& window, void** pointer) const;
    EngineError setWindowIcon(const WindowShareData& window, const ImageData& images) const;
    EngineError getWindowIsIconified(const WindowShareData& window, bool& out_iconified) const;
    EngineError setWindowMaximize(const WindowShareData& window) const;
    EngineError getWindowIsMaximized(const WindowShareData& window, bool& out_maximized) const;
    EngineError getWindowIsVisible(const WindowShareData& window, bool& out_visible) const;
    EngineError setWindowFocused(const WindowShareData& window) const;
    EngineError setWindowDecorated(const WindowShareData& window, const bool decorated) const;
    EngineError getWindowIsDecorated(const WindowShareData& window, bool& out_decorated) const;
    EngineError setWindowAttentionRequest(const WindowShareData& window) const;
    EngineError getWindowIsResizable(const WindowShareData& window, bool& out_resizable) const;
    EngineError setWindowResizable(const WindowShareData& window, const bool resizable) const;
    EngineError setWindowFloating(const WindowShareData& window, const bool floating) const;
    EngineError getWindowIsFloating(const WindowShareData& window, bool& out_focused) const;
    EngineError getWindowIsAutoIconified(const WindowShareData& window, bool& out_auto_iconified) const;
    EngineError setWindowAutoIconified(const WindowShareData& window, const bool auto_iconified) const;
    EngineError getWindowIsFocusOnShow(const WindowShareData& window, bool& out_focus_on_show) const;
    EngineError setWindowFocusOnShow(const WindowShareData& window, const bool focus_on_show) const;
    EngineError getWindowIsFramebufferTransparent(const WindowShareData& window) const;
    EngineError setWindowOpacity(const WindowShareData& window, const float opacity) const;
    EngineError getWindowOpacity(const WindowShareData& window, float& out_opacity) const;
    EngineError setSwapInterval(const int interval) const;

    EngineError getWindowClientApi(const WindowShareData& window, ClientApi& out_result) const;
    EngineError getWindowContextCreationApi(const WindowShareData& window, ContextCreationApi& out_result) const;
    EngineError getWindowContextVersion(const WindowShareData& window, int& out_major, int& out_minor, int& out_revision) const;
    EngineError getWindowForwardCompat(const WindowShareData& window, bool& out_result) const;
    EngineError getWindowDebugContext(const WindowShareData& window, bool& out_result) const;
    EngineError getWindowProfile(const WindowShareData& window, WindowProfileApi& out_result) const;
    EngineError getWindowContextReleaseBehavior(const WindowShareData& window, ContextReleaseBehavior& out_result) const;
    EngineError getWindowContextNoError(const WindowShareData& window, bool& out_result) const;
    EngineError getWindowContextRobustness(const WindowShareData& window, ContextRobustness& out_result) const;


    EngineError getIsExtensionSupported(const std::string& extension_name, bool& out_supported) const;
    EngineError postEmptyEvent() const;
    EngineError getWindowIsHovered(const WindowShareData& window, bool& out_hovered) const;
    std::string getVersionString() const;
    void getRuntimeVersion(int& out_major, int& out_minor, int& out_revision) const;
    void getCompileVersion(int& out_major, int& out_minor, int& out_revision) const;
    EngineError getTimerFrequency(unsigned long long& out_Hz) const;
    EngineError getTimervalue(unsigned long long& out_value) const;
    EngineError getTime(double& out_time) const;
    EngineError setTime(const double time) const;
    EngineError getProcAddress(const std::string& proc_name, ProcAddress& out_address) const;

    void setFastestPollAction();
    void setWaitForNextEventPollAction();
    void setWaitForNextEventWithLimitPollAction();
    void setEventTimeout(const double timeout);
    double getEventTimeout() const;
    
    //Vk stuff
    EngineError isVulkanSupported(bool& out_supported) const;
    
    RenderingContextType getRenderingContextType() const;

    WindowShareData getCurrentWindow() const;
    friend class Engine;
private: RenderingManager();
    RenderingManager(const RenderingManager& rhs) = delete;
    RenderingManager(RenderingManager&& rhs) = delete;
    RenderingManager& operator=(const RenderingManager& rhs) = delete;
    RenderingManager& operator=(RenderingManager&& rhs) = delete;

    static void waitForNextEventOrTimeout();

    //Callbacks
    static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
    static void errorCallback(int error_code, const char* description);
    static void windowPosCallback(GLFWwindow* window, int xpos, int ypos);
    static void windowSizeCallback(GLFWwindow* window, int width, int height);
    static void windowCloseCallback(GLFWwindow* window);
    static void windowRefreshCallback(GLFWwindow* window);
    static void windowFocusCallback(GLFWwindow* window, int focused);
    static void windowIconifyCallback(GLFWwindow* window, int iconified);
    static void windowMaximizeCallback(GLFWwindow* window, int maximized);
    static void windowContentScaleCallback(GLFWwindow* window, float xscale, float yscale);
private:
    std::list<GLFWwindow*> m_windows;
    //TODO: use mutexes for non threadsafe ops if needed
    bool m_initialized;
    bool m_contextInitialized;
    RenderingContextType m_renderingContextType;
    bool m_run;
    std::function<void(void)> m_poolAction;
    double m_poolTimeout;
};
};
#endif