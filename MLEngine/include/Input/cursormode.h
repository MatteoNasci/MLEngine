#ifndef CURSORMODE_H
#define CURSORMODE_H
namespace mle{
enum class CursorMode{
    Normal = 0x00034001, 
    Hidden = 0x00034002,
    Disabled = 0x00034003,
};
};
#endif