#ifndef WINDOWHINTSDATA_H
#define WINDOWHINTSDATA_H

#include <mlengine_global.h>

#include <Rendering/Core/clientapi.h>
#include <Rendering/Core/contextcreationapi.h>
#include <Rendering/Core/contextreleasebehavior.h>
#include <Rendering/Core/contextrobustness.h>
#include <Rendering/Core/windowprofileapi.h>

#include <string>

namespace mle{
struct MLENGINE_SHARED_EXPORT WindowHintsData{
    WindowHintsData();

    //specifies whether the windowed mode window will be resizable by the user. The window will still be resizable using the glfwSetWindowSize function. Possible values are GLFW_TRUE and GLFW_FALSE. This hint is ignored for full screen and undecorated windows.
    bool resizable;
    //specifies whether the windowed mode window will be initially visible. Possible values are GLFW_TRUE and GLFW_FALSE. This hint is ignored for full screen windows.
    bool visible;
    //specifies whether the windowed mode window will have window decorations such as a border, a close widget, etc. An undecorated window will not be resizable by the user but will still allow the user to generate close events on some platforms. Possible values are GLFW_TRUE and GLFW_FALSE. This hint is ignored for full screen windows.
    bool decorated;
    //specifies whether the windowed mode window will be given input focus when created. Possible values are GLFW_TRUE and GLFW_FALSE. This hint is ignored for full screen and initially hidden windows.
    bool focused;
    //specifies whether the full screen window will automatically iconify and restore the previous video mode on input focus loss. Possible values are GLFW_TRUE and GLFW_FALSE. This hint is ignored for windowed mode windows.
    bool auto_iconify;
    //specifies whether the windowed mode window will be floating above other regular windows, also called topmost or always-on-top. This is intended primarily for debugging purposes and cannot be used to implement proper full screen windows. Possible values are GLFW_TRUE and GLFW_FALSE. This hint is ignored for full screen windows.
    bool floating;
    //specifies whether the windowed mode window will be maximized when created. Possible values are GLFW_TRUE and GLFW_FALSE. This hint is ignored for full screen windows.
    bool maximized;
    //specifies whether the cursor should be centered over newly created full screen windows. Possible values are GLFW_TRUE and GLFW_FALSE. This hint is ignored for windowed mode windows.
    bool center_cursor;
    //specifies whether the window framebuffer will be transparent. If enabled and supported by the system, the window framebuffer alpha channel will be used to combine the framebuffer with the background. This does not affect window decorations. Possible values are GLFW_TRUE and GLFW_FALSE.
    //Windows: GLFW sets a color key for the window to work around repainting issues with a transparent framebuffer. The chosen color value is RGB 255,0,255 (magenta). This will make pixels with that exact color fully transparent regardless of their alpha values. If this is a problem, make these pixels any other color before buffer swap.
    bool transparent_framebuffer;
    //specifies whether the window will be given input focus when glfwShowWindow is called. Possible values are GLFW_TRUE and GLFW_FALSE.
    bool focus_on_show;
    //specified whether the window content area should be resized based on the monitor content scale of any monitor it is placed on. This includes the initial placement when the window is created. Possible values are GLFW_TRUE and GLFW_FALSE.
    //This hint only has an effect on platforms where screen coordinates and pixels always map 1:1 such as Windows and X11. On platforms like macOS the resolution of the framebuffer is changed independently of the window size.
    bool scale_to_monitor;

    //specify the desired bit depths of the various components of the default framebuffer. A value of GLFW_DONT_CARE means the application has no preference.
    int red_bits; //0 to INT_MAX or GLFW_DONT_CARE
    int green_bits; //0 to INT_MAX or GLFW_DONT_CARE
    int blue_bits; //0 to INT_MAX or GLFW_DONT_CARE
    int alpha_bits; //0 to INT_MAX or GLFW_DONT_CARE
    int depth_bits; //0 to INT_MAX or GLFW_DONT_CARE
    int stencil_bits; //0 to INT_MAX or GLFW_DONT_CARE

    //specifies whether to use OpenGL stereoscopic rendering. Possible values are GLFW_TRUE and GLFW_FALSE. This is a hard constraint.
    bool stereo;
    //specifies the desired number of samples to use for multisampling. Zero disables multisampling. A value of GLFW_DONT_CARE means the application has no preference.
    int samples; //0 to INT_MAX or GLFW_DONT_CARE
    //specifies whether the framebuffer should be sRGB capable. Possible values are GLFW_TRUE and GLFW_FALSE.
    //OpenGL: If enabled and supported by the system, the GL_FRAMEBUFFER_SRGB enable will control sRGB rendering. By default, sRGB rendering will be disabled.
    //OpenGL ES: If enabled and supported by the system, the context will always have sRGB rendering enabled.
    bool srgb_capable;
    //specifies whether the framebuffer should be double buffered. You nearly always want to use double buffering. This is a hard constraint. Possible values are GLFW_TRUE and GLFW_FALSE.
    bool double_buffer;

    //specifies the desired refresh rate for full screen windows. A value of GLFW_DONT_CARE means the highest available refresh rate will be used. This hint is ignored for windowed mode windows.
    int refresh_rate; //0 to INT_MAX or GLFW_DONT_CARE

    //specifies which client API to create the context for. Possible values are GLFW_OPENGL_API, GLFW_OPENGL_ES_API and GLFW_NO_API. This is a hard constraint.
    ClientApi client_api; //GLFW_OPENGL_API, GLFW_OPENGL_ES_API or GLFW_NO_API
    //specifies which context creation API to use to create the context. Possible values are GLFW_NATIVE_CONTEXT_API, GLFW_EGL_CONTEXT_API and GLFW_OSMESA_CONTEXT_API. This is a hard constraint. If no client API is requested, this hint is ignored.
    //macOS: The EGL API is not available on this platform and requests to use it will fail.
    //Wayland: The EGL API is the native context creation API, so this hint will have no effect.
    //OSMesa: As its name implies, an OpenGL context created with OSMesa does not update the window contents when its buffers are swapped. Use OpenGL functions or the OSMesa native access functions glfwGetOSMesaColorBuffer and glfwGetOSMesaDepthBuffer to retrieve the framebuffer contents.
    //Note: An OpenGL extension loader library that assumes it knows which context creation API is used on a given platform may fail if you change this hint. This can be resolved by having it load via glfwGetProcAddress, which always uses the selected API.
    //Bug: On some Linux systems, creating contexts via both the native and EGL APIs in a single process will cause the application to segfault. Stick to one API or the other on Linux for now.
    ContextCreationApi context_creation_api; //GLFW_NATIVE_CONTEXT_API, GLFW_EGL_CONTEXT_API or GLFW_OSMESA_CONTEXT_API
    //specify the client API version that the created context must be compatible with. The exact behavior of these hints depend on the requested client API.
    //Note: Do not confuse these hints with GLFW_VERSION_MAJOR and GLFW_VERSION_MINOR, which provide the API version of the GLFW header.
    //OpenGL: These hints are not hard constraints, but creation will fail if the OpenGL version of the created context is less than the one requested. It is therefore perfectly safe to use the default of version 1.0 for legacy code and you will still get backwards-compatible contexts of version 3.0 and above when available. While there is no way to ask the driver for a context of the highest supported version, GLFW will attempt to provide this when you ask for a version 1.0 context, which is the default for these hints
    //OpenGL ES: These hints are not hard constraints, but creation will fail if the OpenGL ES version of the created context is less than the one requested. Additionally, OpenGL ES 1.x cannot be returned if 2.0 or later was requested, and vice versa. This is because OpenGL ES 3.x is backward compatible with 2.0, but OpenGL ES 2.0 is not backward compatible with 1.x.
    //macOS: The OS only supports forward-compatible core profile contexts for OpenGL versions 3.2 and later. Before creating an OpenGL context of version 3.2 or later you must set the GLFW_OPENGL_FORWARD_COMPAT and GLFW_OPENGL_PROFILE hints accordingly. OpenGL 3.0 and 3.1 contexts are not supported at all on macOS.
    int context_version_major; //Any valid major version number of the chosen client API
    int context_version_minor; //Any valid minor version number of the chosen client API

    //specifies whether the OpenGL context should be forward-compatible, i.e. one where all functionality deprecated in the requested version of OpenGL is removed. This must only be used if the requested OpenGL version is 3.0 or above. If OpenGL ES is requested, this hint is ignored.
    bool opengl_forward_compat;
    //specifies whether to create a debug OpenGL context, which may have additional error and performance issue reporting functionality. Possible values are GLFW_TRUE and GLFW_FALSE. If OpenGL ES is requested, this hint is ignored.
    bool opengl_debug_context;
    //specifies which OpenGL profile to create the context for. Possible values are one of GLFW_OPENGL_CORE_PROFILE or GLFW_OPENGL_COMPAT_PROFILE, or GLFW_OPENGL_ANY_PROFILE to not request a specific profile. If requesting an OpenGL version below 3.2, GLFW_OPENGL_ANY_PROFILE must be used. If OpenGL ES is requested, this hint is ignored.
    WindowProfileApi opengl_profile; //GLFW_OPENGL_ANY_PROFILE, GLFW_OPENGL_COMPAT_PROFILE or GLFW_OPENGL_CORE_PROFILE
    //specifies the robustness strategy to be used by the context. This can be one of GLFW_NO_RESET_NOTIFICATION or GLFW_LOSE_CONTEXT_ON_RESET, or GLFW_NO_ROBUSTNESS to not request a robustness strategy.
    ContextRobustness context_robustness; //GLFW_NO_ROBUSTNESS, GLFW_NO_RESET_NOTIFICATION or GLFW_LOSE_CONTEXT_ON_RESET
    //specifies the release behavior to be used by the context. Possible values are one of GLFW_ANY_RELEASE_BEHAVIOR, GLFW_RELEASE_BEHAVIOR_FLUSH or GLFW_RELEASE_BEHAVIOR_NONE. If the behavior is GLFW_ANY_RELEASE_BEHAVIOR, the default behavior of the context creation API will be used. If the behavior is GLFW_RELEASE_BEHAVIOR_FLUSH, the pipeline will be flushed whenever the context is released from being the current one. If the behavior is GLFW_RELEASE_BEHAVIOR_NONE, the pipeline will not be flushed on release.
    ContextReleaseBehavior context_release_behavior; //GLFW_ANY_RELEASE_BEHAVIOR, GLFW_RELEASE_BEHAVIOR_FLUSH or GLFW_RELEASE_BEHAVIOR_NONE
    //specifies whether errors should be generated by the context. Possible values are GLFW_TRUE and GLFW_FALSE. If enabled, situations that would have generated errors instead cause undefined behavior.
    bool context_no_error;

    //macOS specific hints

    //specifies whether to use full resolution framebuffers on Retina displays. Possible values are GLFW_TRUE and GLFW_FALSE. This is ignored on other platforms.
    bool macOS_cocoa_retina_framebuffer;
    //specifies the UTF-8 encoded name to use for autosaving the window frame, or if empty disables frame autosaving for the window. This is ignored on other platforms. This is set with glfwWindowHintString.
    std::string macOS_cocoa_frame_name; //A UTF-8 encoded frame autosave name
    //specifies whether to in Automatic Graphics Switching, i.e. to allow the system to choose the integrated GPU for the OpenGL context and move it between GPUs if necessary or whether to force it to always run on the discrete GPU. This only affects systems with both integrated and discrete GPUs. Possible values are GLFW_TRUE and GLFW_FALSE. This is ignored on other platforms. Simpler programs and tools may want to enable this to save power, while games and other applications performing advanced rendering will want to leave it disabled. A bundled application that wishes to participate in Automatic Graphics Switching should also declare this in its Info.plist by setting the NSSupportsAutomaticGraphicsSwitching key to true.
    bool macOS_cocoa_graphics_switching;

    //X11 specific hints

    //specifies the desired ASCII encoded class and instance parts of the ICCCM WM_CLASS window property. These are set with glfwWindowHintString.
    std::string X11_class_name; //An ASCII encoded WM_CLASS class name
    std::string X11_instance_name; //An ASCII encoded WM_CLASS instance name
};
};
#endif