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

    while(Engine::instance().console().isLoggingToFile()){
        Engine::instance().timerManager().advanceTime(10);
    }
    
    try
    {  
        mle::WindowHintsData window_hints = mle::WindowHintsData();
        window_hints.resizable = false;
        window_hints.client_api = ClientApi::None;
        window_hints.try_use_vulkan = true;
        ContextInitData context_data;
        context_data.vulkan_use_layers = true;
        context_data.vulkan_layers.push_back("VK_LAYER_KHRONOS_validation");
        context_data.vulkan_logical_device_extensions.push_back("VK_KHR_swapchain");
        if(Engine::instance().renderingManager().addWindow("Ciao", 800, 500, MonitorHandle(), mle::WindowShareData(), window_hints, context_data) != EngineError::Ok){
            mle::Engine::instance().console().log("Failed to create a main window!", mle::Console::getHighestPriorityClassification());
        }
    }
    catch(std::exception e){
        mle::Engine::instance().console().log("Exception: " + std::string(e.what()), mle::Console::getHighestPriorityClassification());
        return 1;
    }

    while(Engine::instance().console().isLoggingToFile()){
        Engine::instance().timerManager().advanceTime(10);
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
    while(Engine::instance().console().isLoggingToFile()){
        Engine::instance().timerManager().advanceTime(10);
    }

    Engine::instance().monitorData().logMonitorsInformation(Engine::instance().console(), Console::getHighestPriorityClassification());

    while(Engine::instance().console().isLoggingToFile()){
        Engine::instance().timerManager().advanceTime(10);
    }

    Engine::instance().timerManager().addTimer(500000, [](){Engine::instance().stop();});
    const auto result = Engine::instance().loop();
    while(Engine::instance().console().isLoggingToFile()){
        Engine::instance().timerManager().advanceTime(10);
    }

    return int(result);
}