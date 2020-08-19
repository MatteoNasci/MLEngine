#include <Rendering/Core/renderingmanager.h>

#include <GLFW/glfw3.h>

#include <Engine/Core/engine.h>
#include <Engine/Core/engineerrorhelper.h>
#include <Hardware/connectionevent.h>
#include <Input/input.h>//TODO: remove this and stuff like this include (Less dependance)
#include <Rendering/Core/vulkanhandler.h>

#include <stack>
#include <sstream>
#include <unordered_set>
#include <cstring>

using namespace mle;

/*
These methods cannot be used in glfw callbacks
glfwDestroyWindow
glfwDestroyCursor
glfwPollEvents
glfwWaitEvents
glfwWaitEventsTimeout
glfwTerminate
*/

RenderingManager::RenderingManager() : m_windows(), m_initialized(false), m_contextInitialized(false), m_run(false), m_poolAction(), m_poolTimeout(0.0){ 
    glfwSetErrorCallback(&RenderingManager::errorCallback);
    setFastestPollAction();
}
std::string RenderingManager::getVersionString() const{
    return std::string(glfwGetVersionString());
}
void RenderingManager::getRuntimeVersion(int& out_major, int& out_minor, int& out_revision) const{
    glfwGetVersion(&out_major, &out_minor, &out_revision);
}
void RenderingManager::getCompileVersion(int& out_major, int& out_minor, int& out_revision) const{
    out_major = GLFW_VERSION_MAJOR;
    out_minor = GLFW_VERSION_MINOR;
    out_revision = GLFW_VERSION_REVISION;
}
void RenderingManager::waitForNextEventOrTimeout(){
    glfwWaitEventsTimeout(Engine::instance().renderingManager().getEventTimeout());
}
void RenderingManager::setFastestPollAction(){
    m_poolAction = glfwPollEvents;
}
void RenderingManager::setWaitForNextEventPollAction(){
    m_poolAction = glfwWaitEvents;
}
void RenderingManager::setWaitForNextEventWithLimitPollAction(){
    m_poolAction = &RenderingManager::waitForNextEventOrTimeout;
}
void RenderingManager::setEventTimeout(const double timeout){
    m_poolTimeout = timeout;
}
double RenderingManager::getEventTimeout() const{
    return m_poolTimeout;
}
bool RenderingManager::isInitialized() const{
    return m_initialized;
}
bool RenderingManager::isContextInitialized() const{
    return m_contextInitialized;
}
bool RenderingManager::isRunningLoop() const{
    return m_run;
}
void RenderingManager::stopLoop(){
    m_run = false;
}
WindowShareData RenderingManager::getCurrentWindow() const{
    return WindowShareData(glfwGetCurrentContext());
}
void RenderingManager::enableLoop(){
    m_run = true;
}
size_t RenderingManager::getWindowsCount() const{
    return m_windows.size();
}
EngineError RenderingManager::postEmptyEvent() const{
    glfwPostEmptyEvent();
    return getAndClearError();
}
EngineError RenderingManager::getWindowAttribute(const WindowShareData& window, const WindowAttribute attribute, int& out_value) const{
    out_value = glfwGetWindowAttrib(window.window, static_cast<int>(attribute));
    return getAndClearError();
}
EngineError RenderingManager::getWindowClientApi(const WindowShareData& window, ClientApi& out_result) const{
    int res;
    auto e = getWindowAttribute(window.window, WindowAttribute::ClientApi, res);
    out_result = static_cast<ClientApi>(res);
    return e;
}
EngineError RenderingManager::getWindowContextCreationApi(const WindowShareData& window, ContextCreationApi& out_result) const{
    int res;
    auto e = getWindowAttribute(window.window, WindowAttribute::ContextCreationApi, res);
    out_result = static_cast<ContextCreationApi>(res);
    return e;
}
EngineError RenderingManager::getWindowContextVersion(const WindowShareData& window, int& out_major, int& out_minor, int& out_revision) const{
    EngineError e = getWindowAttribute(window.window, WindowAttribute::ContextVersionMajor, out_major);
    if(e != EngineError::Ok){
        return e;
    }

    e = getWindowAttribute(window.window, WindowAttribute::ContextVersionMinor, out_minor);
    if(e != EngineError::Ok){
        return e;
    }

    return getWindowAttribute(window.window, WindowAttribute::ContextRevision, out_revision);
}
EngineError RenderingManager::getWindowForwardCompat(const WindowShareData& window, bool& out_result) const{
    int res;
    auto e = getWindowAttribute(window.window, WindowAttribute::OpenGLForwardCompat, res);
    out_result = res;
    return e;
}
EngineError RenderingManager::getWindowDebugContext(const WindowShareData& window, bool& out_result) const{
    int res;
    auto e = getWindowAttribute(window.window, WindowAttribute::OpenGLDebugContext, res);
    out_result = res;
    return e;
}
EngineError RenderingManager::getWindowProfile(const WindowShareData& window, WindowProfileApi& out_result) const{
    int res;
    auto e = getWindowAttribute(window.window, WindowAttribute::OpenGLProfile, res);
    out_result = static_cast<WindowProfileApi>(res);
    return e;
}
EngineError RenderingManager::getWindowContextReleaseBehavior(const WindowShareData& window, ContextReleaseBehavior& out_result) const{
    int res;
    auto e = getWindowAttribute(window.window, WindowAttribute::ContextReleaseBehavior, res);
    out_result = static_cast<ContextReleaseBehavior>(res);
    return e;
}
EngineError RenderingManager::getWindowContextNoError(const WindowShareData& window, bool& out_result) const{
    int res;
    auto e = getWindowAttribute(window.window, WindowAttribute::ContextNoError, res);
    out_result = res;
    return e;
}
EngineError RenderingManager::getWindowContextRobustness(const WindowShareData& window, ContextRobustness& out_result) const{
    int res;
    auto e = getWindowAttribute(window.window, WindowAttribute::ContextRobustness, res);
    out_result = static_cast<ContextRobustness>(res);
    return e;
}
EngineError RenderingManager::release(){
    if(m_initialized){
        m_initialized = !m_initialized;
        m_renderingContextType = RenderingContextType::None;
        if(m_contextInitialized){
            m_contextInitialized = !m_contextInitialized;
            switch (getRenderingContextType())
            {
            case RenderingContextType::Vulkan:
                VulkanHandler::release();
            default:
                break;
            }
        }
        
        glfwTerminate();
    }
    return EngineError::Ok;            
}
RenderingContextType RenderingManager::getRenderingContextType() const{
    return m_renderingContextType;
}
EngineError RenderingManager::enableFullscreenMode(const WindowShareData& window, const MonitorHandle& monitor, const bool enable, const int posX, const int posY, const int width, const int height, const int refresh_rate) const{
    glfwSetWindowMonitor(window.window, (enable ? monitor.monitor : nullptr), posX, posY, width, height, refresh_rate);
    return getAndClearError();
}
EngineError RenderingManager::getAndClearError(){
    const char** s = nullptr;
    return EngineErrorHelper::convertFromInternalError(glfwGetError(s));
}
EngineError RenderingManager::init(const RenderingInitData& data){
    if(m_initialized){
        return EngineError::Ok;
    }

    glfwInitHint(static_cast<int>(GeneralValues::JoystickHatButtons), data.joystick_hat_buttons);
    glfwInitHint(static_cast<int>(GeneralValues::CocoaChdirResources), data.macOS_cocoa_chdir_resources);
    glfwInitHint(static_cast<int>(GeneralValues::CocoaMenubar), data.macOS_cocoa_menubar);

    // Initialize the library
    if (!glfwInit()){
        return getAndClearError();;
    }
    
    glfwSetJoystickCallback(&Input::joystickCallback);

    m_initialized = true;

    enableLoop();

    return EngineError::Ok;
}

EngineError RenderingManager::addWindow(const std::string& title, const int width, const int height, const MonitorHandle& monitor_data, const WindowShareData& share_data, const WindowHintsData& hints_data, ContextInitData& context_data){
    //Set window hints
    glfwDefaultWindowHints();
    glfwWindowHint(static_cast<int>(WindowAttribute::Resizable), hints_data.resizable);
    glfwWindowHint(static_cast<int>(WindowAttribute::Visible), hints_data.visible);
    glfwWindowHint(static_cast<int>(WindowAttribute::Decorated), hints_data.decorated);
    glfwWindowHint(static_cast<int>(WindowAttribute::Focused), hints_data.focused);
    glfwWindowHint(static_cast<int>(WindowAttribute::AutoIconify), hints_data.auto_iconify);
    glfwWindowHint(static_cast<int>(WindowAttribute::Floating), hints_data.floating);
    glfwWindowHint(static_cast<int>(WindowAttribute::Maximized), hints_data.maximized);
    glfwWindowHint(static_cast<int>(WindowAttribute::CenterCursor), hints_data.center_cursor);
    glfwWindowHint(static_cast<int>(WindowAttribute::TransparentFramebuffer), hints_data.transparent_framebuffer);
    glfwWindowHint(static_cast<int>(WindowAttribute::FocusOnShow), hints_data.focus_on_show);
    glfwWindowHint(static_cast<int>(WindowAttribute::ScaleToMonitor), hints_data.scale_to_monitor);
    glfwWindowHint(static_cast<int>(WindowAttribute::RedBits), hints_data.red_bits);
    glfwWindowHint(static_cast<int>(WindowAttribute::GreenBits), hints_data.green_bits);
    glfwWindowHint(static_cast<int>(WindowAttribute::BlueBits), hints_data.blue_bits);
    glfwWindowHint(static_cast<int>(WindowAttribute::AlphaBits), hints_data.alpha_bits);
    glfwWindowHint(static_cast<int>(WindowAttribute::DepthBits), hints_data.depth_bits);
    glfwWindowHint(static_cast<int>(WindowAttribute::StencilBits), hints_data.stencil_bits);
    glfwWindowHint(static_cast<int>(WindowAttribute::Samples), hints_data.samples);
    glfwWindowHint(static_cast<int>(WindowAttribute::RefreshRate), hints_data.refresh_rate);
    glfwWindowHint(static_cast<int>(WindowAttribute::Stereo), hints_data.stereo);
    glfwWindowHint(static_cast<int>(WindowAttribute::SrgbCapable), hints_data.srgb_capable);
    glfwWindowHint(static_cast<int>(WindowAttribute::DoubleBuffer), hints_data.double_buffer);
    glfwWindowHint(static_cast<int>(WindowAttribute::ClientApi), static_cast<int>(hints_data.client_api));
    glfwWindowHint(static_cast<int>(WindowAttribute::ContextCreationApi), static_cast<int>(hints_data.context_creation_api));
    glfwWindowHint(static_cast<int>(WindowAttribute::ContextVersionMajor), hints_data.context_version_major);
    glfwWindowHint(static_cast<int>(WindowAttribute::ContextVersionMinor), hints_data.context_version_minor);
    glfwWindowHint(static_cast<int>(WindowAttribute::ContextRobustness), static_cast<int>(hints_data.context_robustness));
    glfwWindowHint(static_cast<int>(WindowAttribute::ContextReleaseBehavior), static_cast<int>(hints_data.context_release_behavior));
    glfwWindowHint(static_cast<int>(WindowAttribute::ContextNoError), hints_data.context_no_error);
    glfwWindowHint(static_cast<int>(WindowAttribute::OpenGLForwardCompat), hints_data.opengl_forward_compat);
    glfwWindowHint(static_cast<int>(WindowAttribute::OpenGLDebugContext), hints_data.opengl_debug_context);
    glfwWindowHint(static_cast<int>(WindowAttribute::OpenGLProfile), static_cast<int>(hints_data.opengl_profile));
    glfwWindowHint(static_cast<int>(WindowAttribute::CocoaRetinaFramebuffer), hints_data.macOS_cocoa_retina_framebuffer);
    glfwWindowHintString(static_cast<int>(WindowAttribute::CocoaFrameName), hints_data.macOS_cocoa_frame_name.c_str());
    glfwWindowHint(static_cast<int>(WindowAttribute::CocoaGraphicsSwitching), hints_data.macOS_cocoa_graphics_switching);
    glfwWindowHintString(static_cast<int>(WindowAttribute::X11ClassName), hints_data.X11_class_name.c_str());
    glfwWindowHintString(static_cast<int>(WindowAttribute::X11InstanceName), hints_data.X11_instance_name.c_str());
    
    ///Create a window
    GLFWwindow* window = glfwCreateWindow(width, height, title.c_str(), monitor_data.monitor, share_data.window);
    if (!window){
        return getAndClearError();;
    }       
    m_windows.push_back(window);

    context_data.window = WindowShareData(window);

    glfwSetKeyCallback(window, &Input::keyCallback);
    glfwSetCharCallback(window, &Input::characterCallback);
    glfwSetCharModsCallback(window, &Input::charModsCallback);
    glfwSetCursorPosCallback(window, &Input::cursorPositionCallback);
    glfwSetCursorEnterCallback(window, &Input::cursorEnterCallback);
    glfwSetMouseButtonCallback(window, &Input::mouseButtonCallback);
    glfwSetScrollCallback(window, &Input::scrollCallback);
    glfwSetDropCallback(window, &Input::dropCallback);
    glfwSetFramebufferSizeCallback(window, &RenderingManager::framebufferSizeCallback);
    glfwSetWindowPosCallback(window, &RenderingManager::windowPosCallback);
    glfwSetWindowSizeCallback(window, &RenderingManager::windowSizeCallback);
    glfwSetWindowCloseCallback(window, &RenderingManager::windowCloseCallback);
    glfwSetWindowRefreshCallback(window, &RenderingManager::windowRefreshCallback);
    glfwSetWindowFocusCallback(window, &RenderingManager::windowFocusCallback);
    glfwSetWindowIconifyCallback(window, &RenderingManager::windowIconifyCallback);
    glfwSetWindowMaximizeCallback(window, &RenderingManager::windowMaximizeCallback);
    glfwSetWindowContentScaleCallback(window, &RenderingManager::windowContentScaleCallback);

    if(m_windows.size() == 1){
        //Make the window's context current

        if(!m_contextInitialized){
            Engine::instance().console().log("Initializing rendering context...", Console::getHighestPriorityClassification());
            
            bool vulkan_supported;
            isVulkanSupported(vulkan_supported);
            if(hints_data.try_use_vulkan && hints_data.client_api == ClientApi::None && vulkan_supported){
                VulkanHandler::initialize(context_data);
                m_renderingContextType = RenderingContextType::Vulkan;
            }
            else{
                
                glfwMakeContextCurrent(window); 
                //TODO: Load opengl
            }



            
            m_contextInitialized = !m_contextInitialized;
        }
    }
    return EngineError::Ok;
}
EngineError RenderingManager::singleLoop(){
    
    VulkanHandler::advanceLoggerTime(Engine::instance().frameTime().getLastFrameTime());
    if(m_run){
        std::stack<GLFWwindow*> windows_to_remove;
        for(size_t i = 0; i < m_windows.size(); ++i){
            GLFWwindow* current_window = m_windows[i];

            //Make the window's context current
            //glfwMakeContextCurrent(current_window);

            //Render here
            glClear(GL_COLOR_BUFFER_BIT);

            VulkanHandler::drawFrame();

            //Swap buffers
            //glfwSwapBuffers(current_window);  //TODO: do simila thing of poolevents, use enum m_RenderingContextType to determine what to do here
            //Poll and process events
            m_poolAction();

            if(glfwWindowShouldClose(current_window)){
                windows_to_remove.push(current_window);
            }
        }

        for(auto it = m_windows.crbegin(); it != m_windows.crend() && windows_to_remove.size() != 0; ++it){
            GLFWwindow* to_remove = windows_to_remove.top();
            if(to_remove == *it){
                windows_to_remove.pop();
                glfwDestroyWindow(to_remove);
            }
        }
        return EngineError::Ok;
    }else{
        for(auto it = m_windows.cbegin(); it != m_windows.cend(); ++it){
            GLFWwindow* current_window = *it;

            glfwSetWindowShouldClose(current_window, true);
            glfwDestroyWindow(current_window);
        }

        m_windows.clear();
    }
    VulkanHandler::waitForDeviceIdle();
    return EngineError::Stopped;
}
EngineError RenderingManager::getWindowCurrentMonitor(const WindowShareData& window, MonitorHandle& out_monitor) const{
    out_monitor = glfwGetWindowMonitor(window.window);
    return getAndClearError();
}
EngineError RenderingManager::iconifyWindow(const WindowShareData& window) const{
    glfwIconifyWindow(window.window);
    return getAndClearError();
}
EngineError RenderingManager::restoreWindow(const WindowShareData& window) const{
    glfwRestoreWindow(window.window);
    return getAndClearError();
}
EngineError RenderingManager::setWindowTitle(const WindowShareData& window, const std::string& new_title) const{
    glfwSetWindowTitle(window.window, new_title.c_str());
    return getAndClearError();
}
EngineError RenderingManager::setWindowPosition(const WindowShareData& window, const int x, const int y) const{
    glfwSetWindowPos(window.window, x, y);
    return getAndClearError();
}
EngineError RenderingManager::getWindowPosition(const WindowShareData& window, int& out_x, int& out_y) const{
    glfwGetWindowPos(window.window, &out_x, &out_y);
    return getAndClearError();
}
EngineError RenderingManager::setWindowSize(const WindowShareData& window, const int width, const int height) const{
    glfwSetWindowSize(window.window, width, height);
    return getAndClearError();
}
EngineError RenderingManager::getWindowSize(const WindowShareData& window, int& out_width, int& out_height) const{
    glfwGetWindowSize(window.window, &out_width, &out_height);
    return getAndClearError();
}
EngineError RenderingManager::getWindowFrameSize(const WindowShareData& window, int& out_left, int& out_top, int& out_right, int& out_bottom) const{
    glfwGetWindowFrameSize(window.window, &out_left, &out_top, &out_right, &out_bottom);
    return getAndClearError();
}
EngineError RenderingManager::getWindowFrabebufferSize(const WindowShareData& window, int& out_pixels_width, int& out_pixels_height) const{
    glfwGetFramebufferSize(window.window, &out_pixels_width, &out_pixels_height);
    return getAndClearError();
}
EngineError RenderingManager::setWindowShouldClose(const WindowShareData& window, const bool should_close) const{
    glfwSetWindowShouldClose(window.window, should_close);
    return getAndClearError();
}
EngineError RenderingManager::showWindow(const WindowShareData& window) const{
    glfwShowWindow(window.window);
    return getAndClearError();
}
EngineError RenderingManager::hideWindow(const WindowShareData& window) const{
    glfwHideWindow(window.window);
    return getAndClearError();
}
EngineError RenderingManager::setWindowUserPointer(const WindowShareData& window, void* pointer) const{
    glfwSetWindowUserPointer(window.window, pointer);
    return getAndClearError();
}
EngineError RenderingManager::getWindowShouldClose(const WindowShareData& window, bool& out_result) const{
    out_result = glfwWindowShouldClose(window.window);
    return getAndClearError();
}
EngineError RenderingManager::getWindowContentScale(const WindowShareData& window, float& out_scaleX, float& out_scaleY) const{
    glfwGetWindowContentScale(window.window, &out_scaleX, &out_scaleY);
    return getAndClearError();
}
EngineError RenderingManager::setWindowAspectRation(const WindowShareData& window, const int width, const int height) const{
    glfwSetWindowAspectRatio(window.window, width, height);
    return getAndClearError();
}
EngineError RenderingManager::setWindowSizeLimits(const WindowShareData& window, const int min_width, const int min_height, const int max_width, const int max_height) const{
    glfwSetWindowSizeLimits(window.window, min_width, min_height, max_width, max_height);
    return getAndClearError();
}
EngineError RenderingManager::getWindowUserPointer(const WindowShareData& window, void** pointer) const{
    *pointer = glfwGetWindowUserPointer(window.window);
    return getAndClearError();
}
EngineError RenderingManager::setWindowIcon(const WindowShareData& window, const ImageData& images) const{
    return getAndClearError();
}
EngineError RenderingManager::getWindowIsIconified(const WindowShareData& window, bool& out_iconified) const{
    int res;
    auto e = getWindowAttribute(window, WindowAttribute::Iconified, res);
    out_iconified = res;
    return e;
}
EngineError RenderingManager::setWindowMaximize(const WindowShareData& window) const{
    glfwMaximizeWindow(window.window);
    return getAndClearError();
}
EngineError RenderingManager::getWindowIsMaximized(const WindowShareData& window, bool& out_maximized) const{
    return getAndClearError();
}
EngineError RenderingManager::getWindowIsVisible(const WindowShareData& window, bool& out_visible) const{
    return getAndClearError();
}
EngineError RenderingManager::setWindowFocused(const WindowShareData& window) const{
    return getAndClearError();
}
EngineError RenderingManager::setWindowDecorated(const WindowShareData& window, const bool decorated) const{
    return getAndClearError();
}
EngineError RenderingManager::getWindowIsDecorated(const WindowShareData& window, bool& out_decorated) const{
    return getAndClearError();
}
EngineError RenderingManager::setWindowAttentionRequest(const WindowShareData& window) const{
   return getAndClearError();
}
EngineError RenderingManager::getWindowIsResizable(const WindowShareData& window, bool& out_resizable) const{
    return getAndClearError();
}
EngineError RenderingManager::setWindowResizable(const WindowShareData& window, const bool resizable) const{
    return getAndClearError();
}
EngineError RenderingManager::setWindowFloating(const WindowShareData& window, const bool floating) const{
    return getAndClearError();
}
EngineError RenderingManager::getWindowIsFloating(const WindowShareData& window, bool& out_focused) const{
    return getAndClearError();
}
EngineError RenderingManager::getWindowIsAutoIconified(const WindowShareData& window, bool& out_auto_iconified) const{
    return getAndClearError();
}
EngineError RenderingManager::setWindowAutoIconified(const WindowShareData& window, const bool auto_iconified) const{
    return getAndClearError();
}
EngineError RenderingManager::getWindowIsFocusOnShow(const WindowShareData& window, bool& out_focus_on_show) const{
    return getAndClearError();
}
EngineError RenderingManager::setWindowFocusOnShow(const WindowShareData& window, const bool focus_on_show) const{
    return getAndClearError();
}
EngineError RenderingManager::getWindowIsFramebufferTransparent(const WindowShareData& window) const{
   return getAndClearError();
}
EngineError RenderingManager::setWindowOpacity(const WindowShareData& window, const float opacity) const{
    return getAndClearError();
}
EngineError RenderingManager::getWindowOpacity(const WindowShareData& window, float& out_opacity) const{
    return getAndClearError();
}
EngineError RenderingManager::setSwapInterval(const int interval) const{
    return getAndClearError();
}

void RenderingManager::framebufferSizeCallback(GLFWwindow* window, int width, int height){
    VulkanHandler::notifyFramebufferResized();
}
void RenderingManager::errorCallback(int code, const char* desc){
    const EngineError engine_code = EngineErrorHelper::convertFromInternalError(code);

    if(engine_code != EngineError::Ok){
        std::stringstream stream;
        std::string t(desc);
        stream << "An error occurred in the rendering manager(Glfw)! Error description: " << code << ": " << std::move(t);
        Engine::instance().console().log(stream.str(), Console::getHighestPriorityClassification());
    }
}
void RenderingManager::windowPosCallback(GLFWwindow* window, int xpos, int ypos){

}
void RenderingManager::windowSizeCallback(GLFWwindow* window, int width, int height){

}
void RenderingManager::windowCloseCallback(GLFWwindow* window){

}
void RenderingManager::windowRefreshCallback(GLFWwindow* window){

}
void RenderingManager::windowFocusCallback(GLFWwindow* window, int focused){

}
void RenderingManager::windowIconifyCallback(GLFWwindow* window, int iconified){

}
void RenderingManager::windowMaximizeCallback(GLFWwindow* window, int maximized){

}
void RenderingManager::windowContentScaleCallback(GLFWwindow* window, float xscale, float yscale){

}

EngineError RenderingManager::getIsExtensionSupported(const std::string& extension_name, bool& out_supported) const{
    return getAndClearError();
}
EngineError RenderingManager::getWindowIsHovered(const WindowShareData& window, bool& out_hovered) const{
    return getAndClearError();
}
EngineError RenderingManager::getTimerFrequency(unsigned long long& out_Hz) const{
    return getAndClearError();
}
EngineError RenderingManager::getTimervalue(unsigned long long& out_value) const{
    return getAndClearError();
}
EngineError RenderingManager::getTime(double& out_time) const{
    out_time = glfwGetTime();
    return getAndClearError();
}
EngineError RenderingManager::setTime(const double time) const{
    return getAndClearError();
}
EngineError RenderingManager::getProcAddress(const std::string& proc_name, ProcAddress& out_address) const{
    return getAndClearError();
}

EngineError RenderingManager::isVulkanSupported(bool& out_supported) const{
    out_supported = glfwVulkanSupported();
    return getAndClearError();
}
