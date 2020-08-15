#ifndef WINDOWPROFILEAPI_H
#define WINDOWPROFILEAPI_H
namespace mle{
enum class WindowProfileApi : int{
    Any = 0,
    Core = 0x00032001,
    Compat = 0x00032002,
};
};
#endif