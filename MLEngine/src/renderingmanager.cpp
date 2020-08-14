#include <renderingmanager.h>

#include <GLFW/glfw3.h>

using namespace mle;
RenderingManager::RenderingManager() : m_windows(), m_initialized(false), m_run(false){
    
}
RenderingManager::~RenderingManager(){
    release();
}

bool RenderingManager::isInitialized() const{
    return m_initialized;
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
void RenderingManager::release(){
    if(m_initialized){
        m_initialized = !m_initialized;
        glfwTerminate();
    }            
}
bool RenderingManager::init(){
    if(m_initialized){
        return true;
    }

    // Initialize the library
    if (glfwInit() == GLFW_FALSE){
        return false;
    }

    m_initialized = true;

    return true;
}
bool RenderingManager::addWindow(const std::string& title, const int width, const int height, const MonitorData::Handle& monitor_data, const WindowShareData& share_data, const WindowHintsData& hints_data){
    //Set window hints
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

    if(m_windows.size() == 1){
        //Make the window's context current
        glfwMakeContextCurrent(window); 
    }
    return true;
}
int RenderingManager::loop(){
    m_run = true;
    while(m_run){
        for(auto it = m_windows.cbegin(); it != m_windows.cend();){
            GLFWwindow* current_window = *it;

            //Make the window's context current
            glfwMakeContextCurrent(current_window);

            //Render here
            glClear(GL_COLOR_BUFFER_BIT);
            //Swap buffers
            glfwSwapBuffers(current_window);
            //Poll and process events
            glfwPollEvents();

            auto window_to_check_it = it;
            ++it;
            if(glfwWindowShouldClose(current_window)){
                m_windows.erase(window_to_check_it);
            }
        }
    }

    for(auto it = m_windows.cbegin(); it != m_windows.cend(); ++it){
        GLFWwindow* current_window = *it;

        glfwSetWindowShouldClose(current_window, GLFW_TRUE);
        glfwDestroyWindow(current_window);
    }

    m_windows.clear();

    return 0;
}
RenderingManager& RenderingManager::instance(){
    static RenderingManager singleton;
    return singleton;
}