#ifndef WINDOWATTRIBUTE_H
#define WINDOWATTRIBUTE_H
namespace mle{
enum class WindowAttribute : int{
    //Input focus window hint or window attribute.
    Focused = 0x00020001,
    //Window iconification window attribute.
    Iconified,
    //Window resize-ability window hint and window attribute.
    Resizable,
    //Window visibility window hint and window attribute.
    Visible,
    //Window decoration window hint and window attribute.
    Decorated,
    //Window auto-iconification window hint and window attribute.
    AutoIconify,
    //Window decoration window hint and window attribute.
    Floating,
    //Window maximization window hint and window attribute.
    Maximized,
    //Cursor centering window hint.
    CenterCursor,
    //Window framebuffer transparency window hint and window attribute.
    TransparentFramebuffer = 0x0002000A,
    //Mouse cursor hover window attribute.
    Hovered,
    //Input focus window hint or window attribute.
    FocusOnShow = 0x0002000C,
    //Framebuffer bit depth hint.
    RedBits = 0x00021001,
    //Framebuffer bit depth hint.
    GreenBits,
    //Framebuffer bit depth hint.
    BlueBits,
    //Framebuffer bit depth hint.
    AlphaBits,
    //Framebuffer bit depth hint.
    DepthBits,
    //Framebuffer bit depth hint.
    StencilBits,
    //Framebuffer bit depth hint. [DEPRECATED]
    AccumRedBits,
    //Framebuffer bit depth hint. [DEPRECATED]
    AccumGreenBits,
    //Framebuffer bit depth hint. [DEPRECATED]
    AccumBlueBits,
    //Framebuffer bit depth hint. [DEPRECATED]
    AccumAlphaBits = 0x0002100A,
    //Framebuffer auxiliary buffer hint. [DEPRECATED]
    AuxBuffers,
    //OpenGL stereoscopic rendering hint.
    Stereo,
    //Framebuffer MSAA samples hint.
    Samples,
    //Framebuffer sRGB hint.
    SrgbCapable,
    //Monitor refresh rate hint.
    RefreshRate,
    //Framebuffer double buffering hint.
    DoubleBuffer = 0x00021010,
    //Context client API hint and attribute.
    ClientApi = 0x00022001,
    //Context client API major version hint and attribute.
    ContextVersionMajor,
    //Context client API minor version hint and attribute.
    ContextVersionMinor,
    //Context client API revision number attribute.
    ContextRevision,
    //Context client API revision number hint and attribute.
    ContextRobustness,
    //OpenGL forward-compatibility hint and attribute.
    OpenGLForwardCompat,
    //OpenGL debug context hint and attribute.
    OpenGLDebugContext,
    //OpenGL profile hint and attribute.
    OpenGLProfile,
    //Context flush-on-release hint and attribute.
    ContextReleaseBehavior,
    //Context error suppression hint and attribute.
    ContextNoError = 0x0002200A,
    //Context creation API hint and attribute.
    ContextCreationApi,
    ScaleToMonitor = 0x0002200C,
    CocoaRetinaFramebuffer = 0x00023001,
    CocoaFrameName,
    CocoaGraphicsSwitching = 0x00023003,
    X11ClassName = 0x00024001,
    X11InstanceName = 0x00024002,
};
};
#endif