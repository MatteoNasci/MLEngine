#ifndef CONTEXTCREATIONAPI_H
#define CONTEXTCREATIONAPI_H
namespace mle{
enum class ContextCreationApi : int{
    Native = 0x00036001,
    Egl = 0x00036002,
    Omesa = 0x00036003,
};
};
#endif