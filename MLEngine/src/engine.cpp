#include <engine.h>

#include <renderingmanager.h>
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
    return RenderingManager::instance().loop();
}
Engine& Engine::instance(){
    static Engine engine;
    return engine;
}