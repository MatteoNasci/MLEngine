#include <Engine/Core/engine.h>

#include <Rendering/Core/renderingmanager.h>
#include <Engine/Time/enginetime.h>
using namespace mle;

Engine::Engine() : m_mainTimeManager(), m_mainConsole("MLEngineMainLogs.txt", timerManager()), m_monitorData(), m_renderer(){

}

EngineError Engine::init(const RenderingInitData& data){
    auto result = renderingManager().init(data);
    if(result == EngineError::Ok){
        monitorData().setMonitorCallback();
        result = monitorData().updateMonitorList();
    }
    return result;
}
void Engine::stop(){
    renderingManager().stopLoop();
}
EngineError Engine::loop(){
    frameTime().newFrame(0.0);
    EngineError stopped = EngineError::Ok;
    for(;stopped == EngineError::Ok;){
        timerManager().advanceTime(frameTime().getLastFrameTime());

        double startFrameTime;
        renderingManager().getTime(startFrameTime);

        stopped = renderingManager().singleLoop();           
                
        double endFrameTime;
        renderingManager().getTime(endFrameTime);
        frameTime().newFrame(endFrameTime - startFrameTime);
    }
    renderingManager().release();
    return stopped;
}
