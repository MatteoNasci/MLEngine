#include <mlengineconfig.h>
#include <Engine/Core/engine.h>
#include <Engine/Time/timermanager.h>
#include <Rendering/Core/renderingmanager.h>
#include <Hardware/Monitor/monitordata.h>
#include <Rendering/Core/contextcreationapi.h>

#include <exception>
#include <iostream>
#include <string>

using namespace mle;
int main(int argc, char** argv){  //TODO: check hoe much std::function impacts performance against simple func pointers (for timer and event system)
    const std::string exe_path(argv[0]);

    try{
        mle::Engine::instance().console().log("Init glfw: " + int(mle::Engine::instance().init(RenderingInitData())), mle::Console::getHighestPriorityClassification());
    }
    catch(std::exception e){
        mle::Engine::instance().console().log("Exception: " + std::string(e.what()), mle::Console::getHighestPriorityClassification());
        return 1;
    }

        
    mle::Engine::instance().console().log(exe_path + " Version " + std::to_string(MLEngine_VERSION_MAJOR) + "."
          + std::to_string(MLEngine_VERSION_MINOR), mle::Console::getHighestPriorityClassification());
    mle::Engine::instance().console().log("Usage: " + exe_path + " number", mle::Console::getHighestPriorityClassification());
    
    mle::WindowHintsData window_hints = mle::WindowHintsData();
    window_hints.resizable = false;
    window_hints.client_api = ClientApi::None;
    window_hints.try_use_vulkan = true;
    if(Engine::instance().renderingManager().addWindow("Ciao", 800, 500, MonitorHandle(), mle::WindowShareData(), window_hints) != EngineError::Ok){
        mle::Engine::instance().console().log("Failed to create a main window!", mle::Console::getHighestPriorityClassification());
    }

    std::vector<std::string> commands;
    Engine::instance().console().getCommandList(commands);
    mle::Engine::instance().console().log("Commands found: " + std::to_string(commands.size()), Console::getHighestPriorityClassification());
    for(size_t i = 0; i < commands.size(); ++i){
        mle::Engine::instance().console().log("\t" + commands[i], Console::getHighestPriorityClassification());
    }
    if(!Engine::instance().console().log("/log info-kokojjjij", Console::getHighestPriorityClassification())){
        Engine::instance().console().log("sddggdsgsdgsdgsddgsg", Console::getHighestPriorityClassification());
    }

    Engine::instance().monitorData().logMonitorsInformation(Engine::instance().console(), Console::getHighestPriorityClassification());

    const auto result = Engine::instance().loop();
    while(Engine::instance().console().isLoggingToFile()){
        Engine::instance().timerManager().advanceTime(10);
    }

    return int(result);
}