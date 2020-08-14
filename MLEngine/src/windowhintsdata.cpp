#include <windowhintsdata.h>

#include <GLFW/glfw3.h>
using namespace mle;
WindowHintsData::WindowHintsData() : 
    resizable(GLFW_TRUE),
    visible(GLFW_TRUE),
    decorated(GLFW_TRUE),
    focused(GLFW_TRUE),
    auto_iconify(GLFW_TRUE),
    floating(GLFW_FALSE),
    maximized(GLFW_FALSE),
    center_cursor(GLFW_TRUE),
    transparent_framebuffer(GLFW_FALSE),
    focus_on_show(GLFW_TRUE),
    scale_to_monitor(GLFW_FALSE),
    red_bits(8),
    green_bits(8),
    blue_bits(8),
    alpha_bits(8),
    depth_bits(24),
    stencil_bits(8),
    samples(0),
    refresh_rate(GLFW_DONT_CARE),
    stereo(GLFW_FALSE),
    srgb_capable(GLFW_FALSE),
    double_buffer(GLFW_TRUE),
    client_api(GLFW_OPENGL_API),
    context_creation_api(GLFW_NATIVE_CONTEXT_API),
    context_version_major(1),
    context_version_minor(0),
    context_robustness(GLFW_NO_ROBUSTNESS),
    context_release_behavior(GLFW_ANY_RELEASE_BEHAVIOR),
    context_no_error(GLFW_FALSE),
    opengl_forward_compat(GLFW_FALSE),
    opengl_debug_context(GLFW_FALSE),
    opengl_profile(GLFW_OPENGL_ANY_PROFILE),
    macOS_cocoa_retina_framebuffer(GLFW_TRUE),
    macOS_cocoa_frame_name(""),
    macOS_cocoa_graphics_switching(GLFW_FALSE),
    X11_class_name(""),
    X11_instance_name("")
{

}