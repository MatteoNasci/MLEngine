#ifndef INPUT_H
#define INPUT_H

#include <mlengine_global.h>

#include <Input/gamepadax.h>
#include <Input/gamepadbutton.h>
#include <Input/key.h>
#include <Input/mousebutton.h>
#include <Input/inputconfig.h>
#include <Input/keystate.h>
#include <Input/cursor.h>
#include <Input/standardcursorshape.h>
#include <Input/keydata.h>
#include <Input/hatflag.h>
#include <Rendering/Core/windowsharedata.h>
#include <Rendering/Core/imagedata.h>
#include <Engine/Core/engineerror.h>

#include <vector>
#include <string>
#include <array>

struct GLFWwindow;
namespace mle{
class MLENGINE_SHARED_EXPORT Input{
public:
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

    EngineError setWindowInputMode(const WindowShareData& window, const InputConfig& init_data) const;
    EngineError getWindowInputMode(const WindowShareData& window, InputConfig& out_data) const;
    EngineError getKey(const WindowShareData& window, const int key, KeyState& out_keystate) const;
    EngineError getKeyName(const KeyData& key, std::string& out_name) const;
    EngineError getMouseKey(const WindowShareData& window, const int mouse_key, KeyState& out_keystate) const;
    EngineError getKeyScancode(const int key, int& out_scancode) const;
    EngineError getCursorPosition(const WindowShareData& window, double& x, double& y) const;
    EngineError setCursorPosition(const WindowShareData& window, const double x, const double y) const;
    EngineError isRawMouseMotionSupported(bool& out_supported) const;
    EngineError createCursor(const ImageData& image, const int cursorHotXPosition, const int cursorHotYPosition, Cursor& out_cursor) const;
    EngineError createStandardCursor(const StandardCursorShape shape, Cursor& out_cursor) const;
    void destroyCursor(Cursor& to_destroy) const;
    EngineError setWindowCursor(const WindowShareData& window, const Cursor& cursor) const;
    EngineError isJoystickPresent(const int joystick_index, bool& out_present) const; //TODO: i can iterate over all joysticks, see if i can use it for something
    EngineError isJoystickPresentAndGamepad(const int joystick_index, bool& out_presentAndGamepad) const;
    EngineError getGamepadName(const int joystick_index, std::string& out_name) const;
    EngineError getGamepadState(const int joystick_index, std::array<KeyState, 15>& out_state, std::array<float, 6>& out_axis) const;
    EngineError getJoystickAxes(const int joystick_index, std::vector<float>& out_axes) const;
    EngineError getJoystickButtons(const int joystick_index, std::vector<KeyState>& out_buttonsStates) const;
    EngineError getJoystickHats(const int joystick_index, std::vector<HatFlag>& out_hatsStates) const;
    EngineError getJoystickName(const int joystick_index, std::string& out_name) const;
    EngineError getJoystickUserPointer(const int joystick_index, void** out_pointer) const;
    EngineError setJoystickUserPointer(const int joystick_index, void* pointer) const;
    EngineError getJoystickGUID(const int joystick_index, std::string& out_guid) const;
    EngineError updateGamepadSettingsFromData(const std::string& data, bool& out_success) const;
    EngineError getClipboardString(std::string& out_string) const;
    EngineError setClipboardString(const std::string& to_set) const;

    friend class Engine;
    friend class RenderingManager;
private:
    Input();
    Input(const Input& rhs) = delete;
    Input(Input&& rhs) = delete;
    Input& operator=(const Input& rhs) = delete;
    Input& operator=(Input&& input) = delete;

    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);//TODO: use KeyData for this internally
    static void characterCallback(GLFWwindow* window, unsigned int codepoint);
    static void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos);
    static void cursorEnterCallback(GLFWwindow* window, int entered);
    static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods); //TODO: use MouseKeyData for this internally
    static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
    static void joystickCallback(int jid, int event);
    static void charModsCallback(GLFWwindow* window, unsigned int codepoint, int mods);
    static void dropCallback(GLFWwindow* window, int count, const char** paths);
};
};

#endif