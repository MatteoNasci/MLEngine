#ifndef CLIENTAPI_H
#define CLIENTAPI_H
namespace mle{
enum class ClientApi : int{
    None = 0,
    Opengl = 0x00030001,
    OpenglEs = 0x00030002,
};
};
#endif