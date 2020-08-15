#ifndef STANDARDCURSORSHAPE_H
#define STANDARDCURSORSHAPE_H
namespace mle{
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
};
#endif