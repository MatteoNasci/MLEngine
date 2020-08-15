#include <renderingmanager.h>

#include <GLFW/glfw3.h>

#include <engine.h>

#include <stack>
/*
These methods cannot be used in glfw callbacks
glfwDestroyWindow
glfwDestroyCursor
glfwPollEvents
glfwWaitEvents
glfwWaitEventsTimeout
glfwTerminate
*/

using namespace mle;
RenderingManager::InitData::InitData() :
    joystick_hat_buttons(GLFW_TRUE),
    macOS_cocoa_chdir_resources(GLFW_TRUE),
    macOS_cocoa_menubar(GLFW_TRUE)
{

}
RenderingManager::RenderingManager() : m_windows(), m_initialized(false), m_contextInitialized(false), m_run(false), m_poolAction(), m_poolTimeout(0){
    setFastestPollAction();
}
RenderingManager::~RenderingManager(){
    release();
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
void RenderingManager::joystickCallback(int jid, int event)
{
    const std::string joystick_name(glfwGetJoystickName(jid));
    void* user_pointer = glfwGetJoystickUserPointer(jid);
    if (event == GLFW_CONNECTED)
    {
        // The joystick was connected
        Engine::instance().console().log("The joystick '" + joystick_name + "' is now connected!", Console::getHighestPriorityClassification());
    }
    else if(event == GLFW_DISCONNECTED)
    {
        // The joystick was disconnected
        // In this case only glfwGetJoystickName and glfwGetJoystickUserPointer will be available for the joystick and only before the end of the callback
        Engine::instance().console().log("The joystick '" + joystick_name + "' is now disconnected!", Console::getHighestPriorityClassification());
    }
}
RenderingManager::Error RenderingManager::checkError(std::string& out_decription) const{
    char* description;
    const int internal_code = glfwGetError(&description);
    out_decription = std::string(description);
    return RenderingManager::fromInternalToError(internal_code);
}
RenderingManager::Error RenderingManager::fromInternalToError(const int internal_error){

}
void RenderingManager::setFastestPollAction(){
    m_poolAction = glfwPollEvents;
}
void RenderingManager::setWaitForNextEventPollAction(){
    m_poolAction = glfwWaitEvents;
}
void RenderingManager::setWaitForNextEventWithLimitPollAction(){
    m_poolAction = [](){glfwWaitEventsTimeout(RenderingManager::instance().getEventTimeout());};
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
RenderingManager::Error RenderingManager::postEmptyEvent() const{
    glfwPostEmptyEvent();
}
RenderingManager::Error RenderingManager::getWindowClientApi(const WindowShareData& window, int& out_result) const{
    out_result = glfwGetWindowAttrib(window.window, GLFW_CLIENT_API);
    if(!out_result){
        return checkError();
    }
    return RenderingManager::Error::Ok;
}
RenderingManager::Error RenderingManager::getWindowContextCreationApi(const WindowShareData& window, int& out_result) const{
    out_result = glfwGetWindowAttrib(window.window, GLFW_CONTEXT_CREATION_API);
    if(!out_result){
        return checkError();
    }
    return RenderingManager::Error::Ok;
}
RenderingManager::Error RenderingManager::getWindowContextVersion(const WindowShareData& window, int& out_major, int& out_minor, int& out_revision) const{
    out_major = glfwGetWindowAttrib(window.window, GLFW_CONTEXT_VERSION_MAJOR);
    out_minor = glfwGetWindowAttrib(window.window, GLFW_CONTEXT_VERSION_MINOR);
    out_revision = glfwGetWindowAttrib(window.window, GLFW_CONTEXT_REVISION);
    if(!out_major || !out_minor || !out_revision){
        return checkError();
    }
    return RenderingManager::Error::Ok;
}
RenderingManager::Error RenderingManager::getWindowForwardCompat(const WindowShareData& window, bool& out_result) const{
    out_result = glfwGetWindowAttrib(window.window, GLFW_OPENGL_FORWARD_COMPAT);
    return RenderingManager::Error::Ok;
}
RenderingManager::Error RenderingManager::getWindowDebugContext(const WindowShareData& window, bool& out_result) const{
    out_result = glfwGetWindowAttrib(window.window, GLFW_OPENGL_DEBUG_CONTEXT);
    return RenderingManager::Error::Ok;
}
RenderingManager::Error RenderingManager::getWindowProfile(const WindowShareData& window, int& out_result) const{
    out_result = glfwGetWindowAttrib(window.window, GLFW_OPENGL_PROFILE);
    if(!out_result){
        return checkError();
    }
    return RenderingManager::Error::Ok;
}
RenderingManager::Error RenderingManager::getWindowContextReleaseBehavior(const WindowShareData& window, int& out_result) const{
    out_result = glfwGetWindowAttrib(window.window, GLFW_CONTEXT_RELEASE_BEHAVIOR);
    if(!out_result){
        return checkError();
    }
    return RenderingManager::Error::Ok;
}
RenderingManager::Error RenderingManager::getWindowContextNoError(const WindowShareData& window, bool& out_result) const{
    out_result = glfwGetWindowAttrib(window.window, GLFW_CONTEXT_NO_ERROR);
    return RenderingManager::Error::Ok;
}
RenderingManager::Error RenderingManager::getWindowContextRobustness(const WindowShareData& window, int& out_result) const{
    out_result = glfwGetWindowAttrib(window.window, GLFW_CONTEXT_ROBUSTNESS);
    if(!out_result){
        return checkError();
    }
    return RenderingManager::Error::Ok;
}
void RenderingManager::release(){
    if(m_initialized){
        m_initialized = !m_initialized;
        glfwTerminate();
    }            
}
bool RenderingManager::enableFullscreenMode(const WindowShareData& window, const MonitorData::Handle& monitor, const bool enable, const int posX, const int posY, const int width, const int height, const int refresh_rate) const{
    if(!m_initialized){
        return false;
    }

    glfwSetWindowMonitor(window.window, (enable ? monitor.monitor : nullptr), posX, posY, width, height, refresh_rate);
    return true;
}
bool RenderingManager::init(const InitData& data){
    if(m_initialized){
        return true;
    }

    glfwInitHint(GLFW_JOYSTICK_HAT_BUTTONS, data.joystick_hat_buttons);
    glfwInitHint(GLFW_COCOA_CHDIR_RESOURCES, data.macOS_cocoa_chdir_resources);
    glfwInitHint(GLFW_COCOA_MENUBAR, data.macOS_cocoa_menubar);
    glfwSetJoystickCallback(&RenderingManager::joystickCallback);
    glfwSetErrorCallback(&RenderingManager::errorCallback);

    // Initialize the library
    if (glfwInit() == GLFW_FALSE){
        return false;
    }

    m_initialized = true;

    enableLoop();

    return true;
}
bool RenderingManager::addWindow(const std::string& title, const int width, const int height, const MonitorData::Handle& monitor_data, const WindowShareData& share_data, const WindowHintsData& hints_data){
    if(!m_initialized){
        return false;
    }

    //Set window hints
    glfwDefaultWindowHints();
    glfwWindowHint(GLFW_RESIZABLE, hints_data.resizable);
    glfwWindowHint(GLFW_VISIBLE, hints_data.visible);
    glfwWindowHint(GLFW_DECORATED, hints_data.decorated);
    glfwWindowHint(GLFW_FOCUSED, hints_data.focused);
    glfwWindowHint(GLFW_AUTO_ICONIFY, hints_data.auto_iconify);
    glfwWindowHint(GLFW_FLOATING, hints_data.floating);
    glfwWindowHint(GLFW_MAXIMIZED, hints_data.maximized);
    glfwWindowHint(GLFW_CENTER_CURSOR, hints_data.center_cursor);
    glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, hints_data.transparent_framebuffer);
    glfwWindowHint(GLFW_FOCUS_ON_SHOW, hints_data.focus_on_show);
    glfwWindowHint(GLFW_SCALE_TO_MONITOR, hints_data.scale_to_monitor);
    glfwWindowHint(GLFW_RED_BITS, hints_data.red_bits);
    glfwWindowHint(GLFW_GREEN_BITS, hints_data.green_bits);
    glfwWindowHint(GLFW_BLUE_BITS, hints_data.blue_bits);
    glfwWindowHint(GLFW_ALPHA_BITS, hints_data.alpha_bits);
    glfwWindowHint(GLFW_DEPTH_BITS, hints_data.depth_bits);
    glfwWindowHint(GLFW_STENCIL_BITS, hints_data.stencil_bits);
    glfwWindowHint(GLFW_SAMPLES, hints_data.samples);
    glfwWindowHint(GLFW_REFRESH_RATE, hints_data.refresh_rate);
    glfwWindowHint(GLFW_STEREO, hints_data.stereo);
    glfwWindowHint(GLFW_SRGB_CAPABLE, hints_data.srgb_capable);
    glfwWindowHint(GLFW_DOUBLEBUFFER, hints_data.double_buffer);
    glfwWindowHint(GLFW_CLIENT_API, hints_data.client_api);
    glfwWindowHint(GLFW_CONTEXT_CREATION_API, hints_data.context_creation_api);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, hints_data.context_version_major);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, hints_data.context_version_minor);
    glfwWindowHint(GLFW_CONTEXT_ROBUSTNESS, hints_data.context_robustness);
    glfwWindowHint(GLFW_CONTEXT_RELEASE_BEHAVIOR, hints_data.context_release_behavior);
    glfwWindowHint(GLFW_CONTEXT_NO_ERROR, hints_data.context_no_error);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, hints_data.opengl_forward_compat);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, hints_data.opengl_debug_context);
    glfwWindowHint(GLFW_OPENGL_PROFILE, hints_data.opengl_profile);
    glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, hints_data.macOS_cocoa_retina_framebuffer);
    glfwWindowHintString(GLFW_COCOA_FRAME_NAME, hints_data.macOS_cocoa_frame_name.c_str());
    glfwWindowHint(GLFW_COCOA_GRAPHICS_SWITCHING, hints_data.macOS_cocoa_graphics_switching);
    glfwWindowHintString(GLFW_X11_CLASS_NAME, hints_data.X11_class_name.c_str());
    glfwWindowHintString(GLFW_X11_INSTANCE_NAME, hints_data.X11_instance_name.c_str());
    
    ///Create a window
    GLFWwindow* window = glfwCreateWindow(width, height, title.c_str(), monitor_data.monitor, share_data.window);
    if (!window){
        return false;
    }       
    m_windows.push_back(window);

    glfwSetKeyCallback(window, &RenderingManager::keyCallback);
    glfwSetCharCallback(window, &RenderingManager::characterCallback);
    glfwSetCharModsCallback(window, &RenderingManager::charModsCallback);
    glfwSetCursorPosCallback(window, &RenderingManager::cursorPositionCallback);
    glfwSetCursorEnterCallback(window, &RenderingManager::cursorEnterCallback);
    glfwSetMouseButtonCallback(window, &RenderingManager::mouseButtonCallback);
    glfwSetScrollCallback(window, &RenderingManager::scrollCallback);
    glfwSetDropCallback(window, &RenderingManager::dropCallback);
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
        glfwMakeContextCurrent(window); 

        if(!m_contextInitialized){
            m_contextInitialized = !m_contextInitialized;
            //TODO: load opengl
        }
    }
    return true;
}
int RenderingManager::singleLoop(){
    if(m_run){
        std::stack<GLFWwindow*> windows_to_remove;
        for(size_t i = 0; i < m_windows.size(); ++i){
            GLFWwindow* current_window = m_windows[i];

            //Make the window's context current
            glfwMakeContextCurrent(current_window);

            //Render here
            glClear(GL_COLOR_BUFFER_BIT);
            //Swap buffers
            glfwSwapBuffers(current_window);
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
        return 0;
    }else{
        for(auto it = m_windows.cbegin(); it != m_windows.cend(); ++it){
            GLFWwindow* current_window = *it;

            glfwSetWindowShouldClose(current_window, GLFW_TRUE);
            glfwDestroyWindow(current_window);
        }

        m_windows.clear();
    }
    return 1;
}
RenderingManager& RenderingManager::instance(){
    static RenderingManager singleton;
    return singleton;
}