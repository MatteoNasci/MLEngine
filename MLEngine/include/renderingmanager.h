#ifndef RENDERINGMANAGER_H
#define RENDERINGMANAGER_H

#include <mlengine_global.h>

#include <windowhintsdata.h>
#include <windowsharedata.h>
#include <monitordata.h>
#include <imagedata.h>

#include <vector>
#include <string>
#include <mutex>
#include <functional>
//TODO: fare solo engine singleton, se ho bisogno di altri singleton evito e li faccio friend di engine con ctor privato e niente copy o move

//TODO: use glad for opengl extensions. https://www.glfw.org/docs/latest/context_guide.html#context_glext
struct GLFWwindow;
namespace mle{//TODO: choose between directx 12 and vulkan (vulkan is cross platform, may be more interesting)
class MLENGINE_SHARED_EXPORT RenderingManager{ //TODO: test all
public: //TODO: associate callbacks to glfw
    struct MLENGINE_SHARED_EXPORT InitData{
        InitData();
        //specifies whether to also expose joystick hats as buttons, for compatibility with earlier versions of GLFW that did not have glfwGetJoystickHats. Set this with glfwInitHint.
        bool joystick_hat_buttons;

        //macOS specific:

        //specifies whether to set the current directory to the application to the Contents/Resources subdirectory of the application's bundle, if present. Set this with glfwInitHint.
        bool macOS_cocoa_chdir_resources;
        //specifies whether to create a basic menu bar, either from a nib or manually, when the first window is created, which is when AppKit is initialized. Set this with glfwInitHint.
        bool macOS_cocoa_menubar;
    };
    enum class Error{ //TODO: test these values so that if values of glfw ever change the tests will fail
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
    };
    enum class KeyState{
        //The key or mouse button was released.
        Release = 0,
        //The key or mouse button was pressed.
        Press = 1,
        //The key was held down until it repeated.
        Repeat = 2,
    };
    enum class GamepadAx{
        LeftX = 0,
        LeftY = 1,
        RightX = 2,
        RightY = 3,
        LeftTrigger = 4,
        RightTrigger = 5,
    };
    enum class GamepadButton{
        A = 0,
        B = 1,
        X = 2,
        Y = 3,
        LeftBumper = 4,
        RightBumper = 5,
        Back = 6,
        Start = 7,
        Guide = 8,
        LeftThumb = 9,
        RightThumb = 10,
        DpadUp = 11,
        DpadRight = 12,
        DpadDown = 13,
        DpadLeft = 14,
    };
    enum class JoystickHatBitmask{
        Centered = 0,
        Up = 1 << 0,
        Right = 1 << 1,
        Down = 1 << 2,
        Left = 1 << 3,
        RightUp = Right | Up,
        RightDown = Right | Down,
        LeftUp = Left | Up,
        LeftDown = Left | Down,
    };
    enum class Key{
        Unknown = -1,
        Space = 32,
        Apostrophe = 39,
        Comma = 44, /*,*/
        Minus = 45,
        Period = 46, /*.*/
        Slash = 47, /*/*/
        Key0 = 48,
        Key1,
        Key2,
        Key3,
        Key4,
        Key5,
        Key6,
        Key7,
        Key8,
        Key9 = 57,
        Semicolon = 59, /*;*/
        Equal = 61,
        A = 65,
        B,
        C,
        D,
        E,
        F,
        G,
        H,
        I,
        J,
        K,
        L,
        M,
        N,
        O,
        P,
        Q,
        R,
        S,
        T,
        U,
        V,
        W,
        X,
        Y,
        Z = 90,
        LeftBracket = 91, /*[*/
        Backslash = 92, /*\*/
        RightBracket = 93, /*]*/
        GraveAccent = 96, /*`*/
        World1 = 161, /* non-US #1*/
        World2 = 162, /* non-US #2*/
        Escape = 256,
        Enter,
        Tab,
        Backspace,
        Insert,
        Delete,
        Right,
        Left,
        Down,
        Up,
        PageUp,
        PageDown,
        Home,
        End = 269,
        CapsLock = 280,
        ScrollLock = 281,
        NumLock = 282,
        PrintScreen = 283,
        Pause = 284,
        F1 = 290,
        F2,
        F3,
        F4,
        F5,
        F6,
        F7,
        F8,
        F9,
        F10,
        F11,
        F12,
        F13,
        F14,
        F15,
        F16,
        F17,
        F18,
        F19,
        F20,
        F21,
        F22,
        F23,
        F24,
        F25 = 314,
        KP0 = 320,
        KP1,
        KP2,
        KP3,
        KP4,
        KP5,
        KP6,
        KP7,
        KP8,
        KP9 = 329,
        KPDecimal,
        KPDivide,
        KPMultiply,
        KPSubtract,
        KPAdd,
        KPEnter,
        KPEqual = 336,
        LeftShift = 340,
        LeftControl,
        LeftAlt,
        LeftSuper,
        RightShift,
        RightControl,
        RightAlt,
        RightSuper,
        Menu = 348,
    };
    enum class KeyModFlag{
        //No mod keys were held down.
        None = 0,
        //If this bit is set one or more Shift keys were held down.
        Shift = 0x0001,
        //If this bit is set one or more Control keys were held down.
        Control = 0x0002,
        //If this bit is set one or more Alt keys were held down.
        Alt = 0x0004,
        //If this bit is set one or more Super keys were held down.
        Super = 0x0008,
        //If this bit is set the Caps Lock key is enabled and the GLFW_LOCK_KEY_MODS input mode is set.
        CapsLock = 0x0010,
        //If this bit is set the Num Lock key is enabled and the GLFW_LOCK_KEY_MODS input mode is set.
        NumsLock = 0x0020,
    };
    enum class MouseButton{
        Left = 0,
        Right = 1,
        Middle = 2,
        Fourth = 3,
        Fifth = 4,
        Sixth = 5,
        Seventh = 6,
        Eighth = 7,
    };
    enum class StandardCursorShape{
        //The regular arrow cursor.
        Arrow = 0x00036001,
        //The text input I-beam cursor shape.
        IBeam = 0x00036002,
        //The crosshair shape.
        Crosshair = 0x00036003,
        //The hand shape.
        Hand = 0x00036004,
        //The horizontal resize arrow shape.
        HResize = 0x00036005,
        //The vertical resize arrow shape.
        VResize = 0x00036006,
    };
    enum class WindowAttribute{
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
    struct MLENGINE_SHARED_EXPORT KeyData{
        Key key;
        KeyState action; 
        KeyModFlag mods;
        int scancode; 
    };
    struct MLENGINE_SHARED_EXPORT MouseKeyData{
        MouseButton key;
        KeyState action; 
        KeyModFlag mods;
    };
public:
    bool isInitialized() const;
    bool isContextInitialized() const;
    bool isRunningLoop() const;
    void stopLoop();
    void enableLoop();
    Error init(const InitData& data);
    Error release();
    Error singleLoop();
    size_t getWindowsCount() const;
    Error addWindow(const std::string& title, const int width, const int height, const MonitorData::Handle& monitor_data, const WindowShareData& share, const WindowHintsData& hints);
    Error enableFullscreenMode(const WindowShareData& window, const MonitorData::Handle& monitor, const bool enable, const int posX, const int posY, const int width, const int height, const int refresh_rate) const;
    
    Error getWindowCurrentMonitor(const WindowShareData& window, MonitorData::Handle& out_monitor) const;
    Error iconifyWindow(const WindowShareData& window) const;
    Error restoreWindow(const WindowShareData& window) const;
    Error setWindowTitle(const WindowShareData& window, const std::string& new_title) const;
    Error setWindowPosition(const WindowShareData& window, const int x, const int y) const;
    Error getWindowPosition(const WindowShareData& window, int& out_x, int& out_y) const;
    Error setWindowSize(const WindowShareData& window, const int width, const int height) const;
    Error getWindowSize(const WindowShareData& window, int& out_width, int& out_height) const;
    Error getWindowFrameSize(const WindowShareData& window, int& out_left, int& out_top, int& out_right, int& out_bottom) const;
    Error getWindowFrabebufferSize(const WindowShareData& window, int& out_pixels_width, int& out_pixels_height) const;
    Error setWindowShouldClose(const WindowShareData& window, const bool should_close) const;
    Error showWindow(const WindowShareData& window) const;
    Error hideWindow(const WindowShareData& window) const;
    Error setWindowUserPointer(const WindowShareData& window, void* pointer) const;
    Error getWindowShouldClose(const WindowShareData& window, bool& out_result) const;
    Error getWindowContentScale(const WindowShareData& window, float& out_scaleX, float& out_scaleY) const;
    Error setWindowAspectRation(const WindowShareData& window, const int width, const int height) const;
    Error setWindowSizeLimits(const WindowShareData& window, const int min_width, const int min_height, const int max_width, const int max_height) const;
    Error getWindowSizeLimits(const WindowShareData& window, int& out_min_width, int& out_min_height, int& out_max_width, int& out_max_height) const;
    Error getWindowUserPointer(const WindowShareData& window, void** pointer) const;
    Error setWindowIcon(const WindowShareData& window, const ImageData& images) const;
    Error getWindowIsIconified(const WindowShareData& window, bool& out_iconified) const;
    Error enableWindowMaximize(const WindowShareData& window, const bool maximize) const;
    Error getWindowIsMaximized(const WindowShareData& window, bool& out_maximized) const;
    Error getWindowIsVisible(const WindowShareData& window, bool& out_visible) const;
    Error setWindowFocused(const WindowShareData& window) const;
    Error setWindowDecorated(const WindowShareData& window, const bool decorated) const;
    Error getWindowIsDecorated(const WindowShareData& window, bool& out_decorated) const;
    Error setWindowAttentionRequest(const WindowShareData& window) const;
    Error getWindowIsResizable(const WindowShareData& window, bool& out_resizable) const;
    Error setWindowResizable(const WindowShareData& window, const bool resizable) const;
    Error getWindowIsResizable(const WindowShareData& window, bool& out_floating) const;
    Error setWindowFloating(const WindowShareData& window, const bool floating) const;
    Error getWindowIsFloating(const WindowShareData& window, bool& out_focused) const;
    Error getWindowIsAutoIconified(const WindowShareData& window, bool& out_auto_iconified) const;
    Error setWindowAutoIconified(const WindowShareData& window, const bool auto_iconified) const;
    Error getWindowIsFocusOnShow(const WindowShareData& window, bool& out_focus_on_show) const;
    Error setWindowFocusOnShow(const WindowShareData& window, const bool focus_on_show) const;
    Error getWindowIsFramebufferTransparent(const WindowShareData& window) const;
    Error setWindowOpacity(const WindowShareData& window, const float opacity) const;
    Error getWindowOpacity(const WindowShareData& window, float& out_opacity) const;
    Error setSwapInterval(const int interval) const;

    Error getWindowClientApi(const WindowShareData& window, int& out_result) const;
    Error getWindowContextCreationApi(const WindowShareData& window, int& out_result) const;
    Error getWindowContextVersion(const WindowShareData& window, int& out_major, int& out_minor, int& out_revision) const;
    Error getWindowForwardCompat(const WindowShareData& window, bool& out_result) const;
    Error getWindowDebugContext(const WindowShareData& window, bool& out_result) const;
    Error getWindowProfile(const WindowShareData& window, int& out_result) const;
    Error getWindowContextReleaseBehavior(const WindowShareData& window, int& out_result) const;
    Error getWindowContextNoError(const WindowShareData& window, bool& out_result) const;
    Error getWindowContextRobustness(const WindowShareData& window, int& out_result) const;


    Error getIsExtensionSupported(const std::string& extension_name, bool& out_supported) const;
    Error postEmptyEvent() const;
    Error getWindowIsHovered(const WindowShareData& window, bool& out_hovered) const;
    std::string getVersionString() const;
    void getRuntimeVersion(int& out_major, int& out_minor, int& out_revision) const;
    void getCompileVersion(int& out_major, int& out_minor, int& out_revision) const;
    Error getTimerFrequency(unsigned long long& out_Hz) const;
    Error getTimervalue(unsigned long long& out_value) const;
    Error getTime(double& out_time) const;
    Error setTime(const double time) const;
    Error getProcAddress(const std::string& proc_name, ProcAddress& out_address) const;

    Error setWindowInputMode(const WindowShareData& window, const InputInit& init_data) const;
    Error getWindowInputMode(const WindowShareData& window, InputInit& out_data) const;
    Error getKey(const WindowShareData& window, const int key, KeyState& out_keystate) const;
    Error getKeyName(const KeyData& key, std::string& out_name) const;
    Error getMouseKey(const WindowShareData& window, const int mouse_key, KeyState& out_keystate) const;
    Error getKeyScancode(const int key, int& out_scancode) const;

    Error getCursorPosition(const WindowShareData& window, double& x, double& y) const;
    Error setCursorPosition(const WindowShareData& window, const double x, const double y) const;
    Error isRawMouseMotionSupported(bool& out_supported) const;

    Error createCursor(Cursor& out_cursor) const;
    Error createStandardCursor(Cursor& out_cursor) const;
    void destroyCursor(Cursor& to_destroy) const;
    Error setWindowCursor(const WindowShareData& window, const Cursor& cursor) const;

    Error isJoystickPresent(const int joystick_index, bool& out_present) const; //TODO: i can iterate over all joysticks, see if i can use it for something
    Error isJoystickPresentAndGamepad(const int joystick_index, bool& out_presentAndGamepad) const;
    Error getGamepadName(const int joystick_index, std::string& out_name) const;
    Error getGamepadState(const int joystick_index, GamepadState& out_state) const;
    Error getJoystickAxes(const int joystick_index, std::vector<float>& out_axes) const;
    Error getJoystickButtons(const int joystick_index, std::vector<KeyState>& out_buttonsStates) const;
    Error getJoystickHats(const int joystick_index, std::vector<HatState>& out_hatsStates) const;
    Error getJoystickName(const int joystick_index, std::string& out_name) const;
    Error getJoystickUserPointer(const int joystick_index, void** out_pointer) const;
    Error setJoystickUserPointer(const int joystick_index, void* pointer) const;
    Error getJoystickGUID(const int joystick_index, std::string& out_guid) const;

    Error updateGamepadSettingsFromData(const std::string& data, bool& out_success) const;

    Error getClipboardString(std::string& out_string) const;
    Error setClipboardString(const std::string& to_set) const;

    void setFastestPollAction();
    void setWaitForNextEventPollAction();
    void setWaitForNextEventWithLimitPollAction();
    void setEventTimeout(const double timeout);
    double getEventTimeout() const;

    static GamepadAx constexpr lastAx(){
        return GamepadAx::RightTrigger;
    }
    static GamepadButton constexpr lastButton(){
        return GamepadButton::DpadLeft;
    }
    static GamepadButton constexpr gamepadButtonCross(){
        return GamepadButton::A;
    }
    static GamepadButton constexpr gamepadButtonCircle(){
        return GamepadButton::B;
    }
    static GamepadButton constexpr gamepadButtonSquare(){
        return GamepadButton::X;
    }
    static GamepadButton constexpr gamepadButtonTriangle(){
        return GamepadButton::Y;
    }
    static int constexpr joystickFirstIndex(){
        return 0;
    }
    static int constexpr joystickLastIndex(){
        return 15;
    }
    static Key constexpr lastKey(){
        return Key::Menu;
    }
    static MouseButton constexpr lastMouseButton(){
        return MouseButton::Eighth;
    }
    
    /* Vk stuff
    Error isVulkanSupported(bool& out_supported) const;
    Error getRequiredInstanceExtensions(std::vector<std::string>& out_extensions) const;
    Error getInstanceProcAddress(const std::string& proc_name, const VkInstance& instance, VkProcAddress& out_address) const;
    Error getPhysicalDevicePresentationSupport(const VkInstance& instance, const VkPhysicalDevice& device, const uint32_t queue_family, bool& out_supported) const;
    Error createWindowSurface(const VkInstance& instance, const WindowShareData& window, const VkAllocationCallbacks& allocator, const VkSurfaceKHR& surface) const;
    */
    
    WindowShareData getCurrentWindow() const;
    static RenderingManager& instance();

    ~RenderingManager();
private: RenderingManager();

    Error checkError() const;
    Error checkError(std::string& out_decription) const;
    static Error fromInternalToError(const int internal_error);

    //Callbacks
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);//TODO: use KeyData for this internally
    static void characterCallback(GLFWwindow* window, unsigned int codepoint);
    static void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos);
    static void cursorEnterCallback(GLFWwindow* window, int entered);
    static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods); //TODO: use MouseKeyData for this internally
    static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
    static void joystickCallback(int jid, int event);
    static void dropCallback(GLFWwindow* window, int count, const char** paths);
    static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
    static void errorCallback(int error_code, const char* description);
    static void charModsCallback(GLFWwindow* window, unsigned int codepoint, int mods);
    static void windowPosCallback(GLFWwindow* window, int xpos, int ypos);
    static void windowSizeCallback(GLFWwindow* window, int width, int height);
    static void windowCloseCallback(GLFWwindow* window);
    static void windowRefreshCallback(GLFWwindow* window);
    static void windowFocusCallback(GLFWwindow* window, int focused);
    static void windowIconifyCallback(GLFWwindow* window, int iconified);
    static void windowMaximizeCallback(GLFWwindow* window, int maximized);
    static void windowContentScaleCallback(GLFWwindow* window, float xscale, float yscale);
private:
    std::vector<GLFWwindow*> m_windows;
    //TODO: use mutexes for non threadsafe ops if needed
    bool m_initialized;
    bool m_contextInitialized;
    bool m_run;
    std::function<void(void)> m_poolAction;
    double m_poolTimeout;
};
};
#endif