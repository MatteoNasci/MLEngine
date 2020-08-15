#ifndef CONNECTIONEVENT_H
#define CONNECTIONEVENT_H
namespace mle{
enum class ConnectionEvent{
    Connected = 0x00040001,
    Disconnected = 0x00040002,
};
};
#endif