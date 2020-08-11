#ifndef ENGINE_H
#define ENGINE_H

#include <mlengine_global.h>
#include <console.h>
#include <timemanager.h>

namespace mle{
class MLENGINE_SHARED_EXPORT Engine{
public:
    bool init();

    Console& console();
    TimeManager& timeManager();

    static Engine& instance();
private: Engine();
private:
    TimeManager m_mainTimeManager;
    Console m_mainConsole;
};
};
#endif