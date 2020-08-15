#ifndef ENGINE_H
#define ENGINE_H

#include <mlengine_global.h>
#include <Engine/Debug/console.h>
#include <Engine/Time/timermanager.h>
#include <Rendering/Core/renderinginitdata.h>
#include <Hardware/Monitor/monitordata.h>
#include <Rendering/Core/renderingmanager.h>
#include <Input/input.h>

namespace mle{
class MLENGINE_SHARED_EXPORT Engine{ //TODO: test all
public:
    EngineError init(const RenderingInitData& data);
    void stop();
    EngineError loop();

    Console& console();
    TimerManager& timerManager();
    MonitorData& monitorData();
    RenderingManager& renderingManager();
    Input& input();
    const Console& console() const;
    const TimerManager& timerManager() const;
    const MonitorData& monitorData() const;
    const RenderingManager& renderingManager() const;
    const Input& input() const;

    static Engine& instance();
private: Engine();
private:
    TimerManager m_mainTimeManager;
    Console m_mainConsole;
    MonitorData m_monitorData;
    RenderingManager m_renderer;
    Input m_input;
};
};
#endif