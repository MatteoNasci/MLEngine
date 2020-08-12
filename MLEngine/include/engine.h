#ifndef ENGINE_H
#define ENGINE_H

#include <mlengine_global.h>
#include <console.h>
#include <timermanager.h>

namespace mle{
class MLENGINE_SHARED_EXPORT Engine{
public:
    bool init();

    Console& console();
    TimerManager& timerManager();

    static Engine& instance();
private: Engine();
private:
    TimerManager m_mainTimeManager;
    Console m_mainConsole;
};
};
#endif