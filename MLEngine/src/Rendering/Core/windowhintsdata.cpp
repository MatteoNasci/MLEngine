#include <Rendering/Core/windowhintsdata.h>

#include <Rendering/Core/generalvalues.h>
using namespace mle;
WindowHintsData::WindowHintsData() : 
    try_use_vulkan(true),
    resizable(true),
    visible(true),
    decorated(true),
    focused(true),
    auto_iconify(true),
    floating(false),
    maximized(false),
    center_cursor(true),
    transparent_framebuffer(false),
    focus_on_show(true),
    scale_to_monitor(false),
    red_bits(8),
    green_bits(8),
    blue_bits(8),
    alpha_bits(8),
    depth_bits(24),
    stencil_bits(8),
    samples(0),
    refresh_rate(static_cast<int>(GeneralValues::DontCare)),
    stereo(false),
    srgb_capable(false),
    double_buffer(true),
    client_api(ClientApi::Opengl),
    context_creation_api(ContextCreationApi::Native),
    context_version_major(1),
    context_version_minor(0),
    context_robustness(ContextRobustness::None),
    context_release_behavior(ContextReleaseBehavior::Any),
    context_no_error(false),
    opengl_forward_compat(false),
    opengl_debug_context(false),
    opengl_profile(WindowProfileApi::Any),
    macOS_cocoa_retina_framebuffer(true),
    macOS_cocoa_frame_name(""),
    macOS_cocoa_graphics_switching(false),
    X11_class_name(""),
    X11_instance_name("")
{

}