#ifndef CONTEXTRELEASEBEHAVIOR_H
#define CONTEXTRELEASEBEHAVIOR_H
namespace mle{
enum class ContextReleaseBehavior : int{
    Any = 0,
    Flush = 0x00035001,
    None = 0x00035002,
};
};
#endif