#ifndef KEYMODEFLAG_H
#define KEYMODEFLAG_H
namespace mle{
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
};
#endif