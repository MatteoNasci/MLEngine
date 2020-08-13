#include <mlengineconfig.h>
#include <engine.h>
#include <timermanager.h>

#include <exception>
#include <iostream>
#include <string>
int main(int argc, char** argv){
    const std::string exe_path(argv[0]);

    try{
        mle::Engine::instance().console().log("Init glfw: " + mle::Engine::instance().init(), mle::LogClassification::Info);
    }
    catch(std::exception e){
        mle::Engine::instance().console().log("Exception: " + e.what());
    }
    mle::Engine::instance().console().log(exe_path + " Version " + std::to_string(MLEngine_VERSION_MAJOR) + "."
          + std::to_string(MLEngine_VERSION_MINOR), mle::LogClassification::Info);
    mle::Engine::instance().console().log("Usage: " + exe_path + " number", mle::LogClassification::Info);

    while(true){
        mle::Engine::instance().timerManager().advanceTime(0.01);
    }
    return 0;
}