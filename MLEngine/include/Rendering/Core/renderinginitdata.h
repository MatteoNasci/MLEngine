#ifndef RENDERINGINITDATA_H
#define RENDERINGINITDATA_H

#include <mlengine_global.h>

namespace mle{
struct MLENGINE_SHARED_EXPORT RenderingInitData{
    RenderingInitData();
    //specifies whether to also expose joystick hats as buttons, for compatibility with earlier versions of GLFW that did not have glfwGetJoystickHats. Set this with glfwInitHint.
    bool joystick_hat_buttons;

    //macOS specific:

    //specifies whether to set the current directory to the application to the Contents/Resources subdirectory of the application's bundle, if present. Set this with glfwInitHint.
    bool macOS_cocoa_chdir_resources;
    //specifies whether to create a basic menu bar, either from a nib or manually, when the first window is created, which is when AppKit is initialized. Set this with glfwInitHint.
    bool macOS_cocoa_menubar;
};
};
#endif