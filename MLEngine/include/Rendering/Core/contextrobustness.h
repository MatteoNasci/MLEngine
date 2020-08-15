#ifndef CONTEXTROBUSTNESS_H
#define CONTEXTROBUSTNESS_H
namespace mle{
enum class ContextRobustness : int{
    None = 0,
    NoResetNotification = 0x00031001,
    LoseContextOnReset = 0x00031002,
};
};
#endif