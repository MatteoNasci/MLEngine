#include <mlengineconfig.h>
#include <engine.h>

#include <exception>
#include <iostream>
int main(int argc, char** argv){
    if (argc < 2) {
        try{
            std::cout << "Init glfw: " << mle::Engine::instance().init() << std::endl;
        }
        catch(std::exception e){
            std::cout << "Exception: " << e.what() << std::endl;
        }
        mle::Engine::instance().console().log("Miao", mle::LogClassification::Critical);
        mle::Engine::instance().console().log("Miao2", mle::LogClassification::Critical);
        mle::Engine::instance().console().log("Miao3", mle::LogClassification::Critical);
        std::cout << argv[0] << " Version " << MLEngine_VERSION_MAJOR << "."
              << MLEngine_VERSION_MINOR << std::endl;
        std::cout << "Usage: " << argv[0] << " number" << std::endl;
    }
    while(true){
        mle::Engine::instance().timeManager().advanceTime(0.01);
    }
    return 0;
}