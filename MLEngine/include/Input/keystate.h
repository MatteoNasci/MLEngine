#ifndef KEYSTATE_H
#define KEYSTATE_H
namespace mle{
enum class KeyState{
    //The key or mouse button was released.
    Release = 0,
    //The key or mouse button was pressed.
    Press = 1,
    //The key was held down until it repeated.
    Repeat = 2,
};
};
#endif