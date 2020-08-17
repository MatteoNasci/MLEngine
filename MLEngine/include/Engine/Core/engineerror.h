#ifndef ENGINEERROR_H
#define ENGINEERROR_H

namespace mle{
enum class EngineError{ //TODO: test these values so that if values of glfw ever change the tests will fail
    //No error has occurred.
    Ok = 0,
    //This occurs if a GLFW function was called that must not be called unless the library is initialized. Initialize GLFW before calling any function that requires initialization.
    NotInitialized = 0x00010001,
    //This occurs if a GLFW function was called that needs and operates on the current OpenGL or OpenGL ES context but no context is current on the calling thread. One such function is glfwSwapInterval. Ensure a context is current before calling functions that require a current context.
    NoCurrentContext = 0x00010002,
    //One of the arguments to the function was an invalid enum value, for example requesting GLFW_RED_BITS with glfwGetWindowAttrib. Fix the offending call.
    InvalidEnum = 0x00010003,
    //One of the arguments to the function was an invalid value, for example requesting a non-existent OpenGL or OpenGL ES version like 2.7. Requesting a valid but unavailable OpenGL or OpenGL ES version will instead result in a GLFW_VERSION_UNAVAILABLE error. Fix the offending call.
    InvalidValue = 0x00010004,
    //A memory allocation failed. A bug in GLFW or the underlying operating system. Report the bug to our issue tracker. https://github.com/glfw/glfw/issues
    OutOfMemory = 0x00010005,
    //GLFW could not find support for the requested API on the system. The installed graphics driver does not support the requested API, or does not support it via the chosen context creation backend. Examples: Some pre-installed Windows graphics drivers do not support OpenGL. AMD only supports OpenGL ES via EGL, while Nvidia and Intel only support it via a WGL or GLX extension. macOS does not provide OpenGL ES at all. The Mesa EGL, OpenGL and OpenGL ES libraries do not interface with the Nvidia binary driver. Older graphics drivers do not support Vulkan.
    ApiUnavailable = 0x00010006,
    //The requested OpenGL or OpenGL ES version (including any requested context or framebuffer hints) is not available on this machine. The machine does not support your requirements. If your application is sufficiently flexible, downgrade your requirements and try again. Otherwise, inform the user that their machine does not match your requirements. Future invalid OpenGL and OpenGL ES versions, for example OpenGL 4.8 if 5.0 comes out before the 4.x series gets that far, also fail with this error and not GLFW_INVALID_VALUE, because GLFW cannot know what future versions will exist.
    VersionUnavailable = 0x00010007,
    //A platform-specific error occurred that does not match any of the more specific categories. A bug or configuration error in GLFW, the underlying operating system or its drivers, or a lack of required resources. Report the issue to our issue tracker. https://github.com/glfw/glfw/issues
    PlatformError = 0x00010008,
    //If emitted during window creation, the requested pixel format is not supported. If emitted when querying the clipboard, the contents of the clipboard could not be converted to the requested format. If emitted during window creation, one or more hard constraints did not match any of the available pixel formats. If your application is sufficiently flexible, downgrade your requirements and try again. Otherwise, inform the user that their machine does not match your requirements. If emitted when querying the clipboard, ignore the error or report it to the user, as appropriate.
    FormatUnavailable = 0x00010009,
    //A window that does not have an OpenGL or OpenGL ES context was passed to a function that requires it to have one. Fix the offending call.
    NoWindowContext = 0x0001000A,

    VK_NotReady = 1,
    VK_Timeout = 2,
    VK_EventSet = 3,
    VK_EventReset = 4,
    VK_Incomplete = 5,
    VK_ErrorOutOfHostMemory = -1,
    VK_ErrorOutOfDeviceMemory = -2,
    VK_ErrorInitializationFailed = -3,
    VK_ErrorDeviceLost = -4,
    VK_ErrorMemoryMapFailed = -5,
    VK_ErrorLayerNotPresent = -6,
    VK_ErrorExtensionNotPresent = -7,
    VK_ErrorFeatureNotPresent = -8,
    VK_ErrorIncompatibleDriver = -9,
    VK_ErrorTooManyObjects = -10,
    VK_ErrorFormatNotSupported = -11,
    VK_ErrorFragmentedPool = -12,
    VK_ErrorOutOfPoolMemory= -1000069000,
    VK_ErrorInvalidExternalHandle = -1000072003,
    VK_ErrorSurfaceLostKHR = -1000000000,
    VK_ErrorNativeWindowInUseKHR = -1000000001,
    VK_SuboptimalKHR = 1000001003,
    VK_ErrorOutOfDateKHR = -1000001004,
    VK_ErrorValidationFailedExt = -1000011001,

    //The rendering loop has been stopped by setting the relative flag to false.
    Stopped,
};
};
#endif