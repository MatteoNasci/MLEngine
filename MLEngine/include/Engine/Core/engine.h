#ifndef ENGINE_H
#define ENGINE_H

#include <mlengine_global.h>
#include <Engine/Debug/console.h>
#include <Engine/Time/timermanager.h>
#include <Rendering/Core/renderinginitdata.h>
#include <Hardware/Monitor/monitordata.h>
#include <Rendering/Core/renderingmanager.h>
#include <Input/input.h>
#include <Engine/Time/frametime.h>

namespace mle{
class MLENGINE_SHARED_EXPORT Engine{ //TODO: test all
public://TODO: Engine should only store classes and have init-loop-stop. Everything else has to go into something else
    EngineError init(const RenderingInitData& data);
    void stop();
    EngineError loop();

    inline Console& console(){
        return m_mainConsole;
    }
    inline TimerManager& timerManager(){
        return m_mainTimeManager;
    }
    inline MonitorData& monitorData(){
        return m_monitorData;
    }
    inline RenderingManager& renderingManager(){
        return m_renderer;
    }
    inline Input& input(){
        return m_input;
    }
    inline FrameTime& frameTime(){
        return m_frameTime;
    }
    inline const Console& console() const{
        return m_mainConsole;
    }
    inline const TimerManager& timerManager() const{
        return m_mainTimeManager;
    }
    inline const MonitorData& monitorData() const{
        return m_monitorData;
    }
    inline const RenderingManager& renderingManager() const{
        return m_renderer;
    }
    inline const Input& input() const{
        return m_input;
    }
    inline const FrameTime& frameTime() const{
        return m_frameTime;
    }
    inline static Engine& instance(){
        static Engine engine;
        return engine;
    }

private: Engine();
private:
    TimerManager m_mainTimeManager;
    Console m_mainConsole;
    MonitorData m_monitorData;
    RenderingManager m_renderer;
    Input m_input;
    FrameTime m_frameTime;
};
};
#endif