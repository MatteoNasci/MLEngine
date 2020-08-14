#include <mlengineconfig.h>
#include <engine.h>
#include <timermanager.h>
#include <renderingmanager.h>
#include <monitordata.h>

#include <exception>
#include <iostream>
#include <string>

using namespace mle;
int main(int argc, char** argv){
    const std::string exe_path(argv[0]);

    try{
        mle::Engine::instance().console().log("Init glfw: " + mle::Engine::instance().init(), mle::Console::getHighestPriorityClassification());
    }
    catch(std::exception e){
        mle::Engine::instance().console().log("Exception: " + std::string(e.what()), mle::Console::getHighestPriorityClassification());
        return 1;
    }
    mle::Engine::instance().console().log(exe_path + " Version " + std::to_string(MLEngine_VERSION_MAJOR) + "."
          + std::to_string(MLEngine_VERSION_MINOR), mle::Console::getHighestPriorityClassification());
    mle::Engine::instance().console().log("Usage: " + exe_path + " number", mle::Console::getHighestPriorityClassification());
    
    mle::WindowHintsData window_hints = mle::WindowHintsData();
    /*if(!mle::RenderingManager::instance().addWindow("Ciao", 2560, 1440, 0, mle::WindowShareData(), window_hints)){
        mle::Engine::instance().console().log("Failed to create a main window!", mle::Console::getHighestPriorityClassification());
    }*/

    std::vector<std::string> commands;
    Engine::instance().console().getCommandList(commands);
    mle::Engine::instance().console().log("Commands found: " + std::to_string(commands.size()), LogClassification::Info);
    for(size_t i = 0; i < commands.size(); ++i){
        mle::Engine::instance().console().log("\t" + commands[i], LogClassification::Info);
    }
    if(!Engine::instance().console().log("/log info", LogClassification::Info)){
        Engine::instance().console().log("sddggdsgsdgsdgsddgsg", LogClassification::Info);
    }

    mle::MonitorData::instance().logMonitorsInformation(Engine::instance().console(), LogClassification::Info);

    const int result = 0/*mle::RenderingManager::instance().loop()*/;
    while(Engine::instance().console().isLoggingtoFile()){
        Engine::instance().timerManager().advanceTime(10);
    }

    return result;
}