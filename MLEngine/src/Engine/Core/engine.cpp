#include <Engine/Core/engine.h>

#include <Rendering/Core/renderingmanager.h>
#include <Engine/Time/enginetime.h>
using namespace mle;

Engine::Engine() : m_mainTimeManager(), m_mainConsole("MLEngineMainLogs.txt", timerManager()), m_monitorData(), m_renderer(){

}
Console& Engine::console(){
    return m_mainConsole;
}
TimerManager& Engine::timerManager(){
    return m_mainTimeManager;
}
MonitorData& Engine::monitorData(){
    return m_monitorData;
}
RenderingManager& Engine::renderingManager(){
    return m_renderer;
}
Input& Engine::input(){
    return m_input;
}
const Console& Engine::console() const{
    return m_mainConsole;
}
const TimerManager& Engine::timerManager() const{
    return m_mainTimeManager;
}
const MonitorData& Engine::monitorData() const{
    return m_monitorData;
}
const RenderingManager& Engine::renderingManager() const{
    return m_renderer;
}
const Input& Engine::input() const{
    return m_input;
}
EngineError Engine::init(const RenderingInitData& data){
    auto result = renderingManager().init(data);
    if(result == EngineError::Ok){
        result = monitorData().updateMonitorList();
    }
    return result;
}
void Engine::stop(){
    renderingManager().stopLoop();
}
EngineError Engine::loop(){
    double lastFrameTime = 0.0;
    EngineError stopped = EngineError::Ok;
    for(;stopped == EngineError::Ok;){
        timerManager().advanceTime(lastFrameTime);

        double startFrameTime;
        renderingManager().getTime(startFrameTime);

        stopped = renderingManager().singleLoop();           
                
        double endFrameTime;
        renderingManager().getTime(endFrameTime);
        lastFrameTime = endFrameTime - startFrameTime;
    }
    renderingManager().release();
    return stopped;
}
Engine& Engine::instance(){
    static Engine engine;
    return engine;
}