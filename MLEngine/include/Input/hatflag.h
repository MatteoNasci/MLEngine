#ifndef HATFLAG_H
#define HATFLAG_H
namespace mle{
enum class HatFlag{
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
};
#endif