#include <mlengineconfig.h>
#include <test.h>
#include <engine.h>

#include <exception>
#include <iostream>
int main(int argc, char** argv){
    if (argc < 2) {
    // report version
    try{
        std::cout << "Init glfw: " << mle::Engine::instance().init() << std::endl;
    }
    catch(std::exception e){
        std::cout << "Exception: " << e.what() << std::endl;
    }
        std::cout << argv[0] << " Version " << MLEngine_VERSION_MAJOR << "."
              << MLEngine_VERSION_MINOR << std::endl;
        std::cout << "Usage: " << argv[0] << " number" << std::endl;
        std::cout << "1 + 2 = " << mle::Test().add(1, 2) << std::endl;
    }
    while(true){

    }
    return 0;
}