#ifndef INPUTCONFIG_H
#define INPUTCONFIG_H

#include <mlengine_global.h>

#include <Input/cursormode.h>

namespace mle{
struct MLENGINE_SHARED_EXPORT InputConfig{
    CursorMode cursorMode;
    bool enableStickyKeys;
    bool enableStickyMouseButtons;
    bool enableLockKeyMod;
    bool useRawMouseMotion;
};
};

#endif