#ifndef KEYDATA_H
#define KEYDATA_H

#include <mlengine_global.h>

#include <Input/keystate.h>
#include <Input/keymodflag.h>

namespace mle{
struct MLENGINE_SHARED_EXPORT KeyData{
    Key key;
    KeyState action; 
    KeyModFlag mods;
    int scancode; 
};
};

#endif