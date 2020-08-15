#ifndef MOUSEKEYDATA_H
#define MOUSEKEYDATA_H

#include <mlengine_global.h>

#include <Input/keystate.h>
#include <Input/mousebutton.h>
#include <Input/keymodflag.h>
namespace mle{
struct MLENGINE_SHARED_EXPORT MouseKeyData{
    MouseButton key;
    KeyState action; 
    KeyModFlag mods;
};
};

#endif