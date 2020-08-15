#include <engine.h>

#include <renderingmanager.h>
#include <enginetime.h>

#include <chrono>
using namespace mle;

Engine::Engine() : m_mainTimeManager(), m_mainConsole("Console.txt", timerManager()){

}
Console& Engine::console(){
    return m_mainConsole;
}
TimerManager& Engine::timerManager(){
    return m_mainTimeManager;
}
bool Engine::init(){
    return RenderingManager::instance().init();
}
int Engine::loop(){
    double lastFrameTime = 0.0;
    int result = 0;
    for(;!result;){
        timerManager().advanceTime(lastFrameTime);
        auto startFrameTime = EngineTime::now<std::chrono::steady_clock>();
        result = RenderingManager::instance().singleLoop();

        auto endFrameTime = EngineTime::now<std::chrono::steady_clock>();
        lastFrameTime = EngineTime::getInterval<std::milli>(startFrameTime, endFrameTime);

        if(!result){
            result = !RenderingManager::instance().getWindowsCount() ? -1 : 0;
        }
    }
    return result;
}
Engine& Engine::instance(){
    static Engine engine;
    return engine;
}