#include <Input/input.h>

#include <GLFW/glfw3.h>

#include <Hardware/connectionevent.h>
#include <Engine/Core/engine.h>
#include <Engine/Debug/console.h>

using namespace mle;
Input::Input(){
    
}
void Input::joystickCallback(int jid, int event)
{
    const std::string joystick_name(glfwGetJoystickName(jid));
    void* user_pointer = glfwGetJoystickUserPointer(jid);
    if (event == static_cast<int>(ConnectionEvent::Connected))
    {
        // The joystick was connected
        Engine::instance().console().log("The joystick '" + joystick_name + "' is now connected!", Console::getHighestPriorityClassification());
    }
    else if(event == static_cast<int>(ConnectionEvent::Disconnected))
    {
        // The joystick was disconnected
        // In this case only glfwGetJoystickName and glfwGetJoystickUserPointer will be available for the joystick and only before the end of the callback
        Engine::instance().console().log("The joystick '" + joystick_name + "' is now disconnected!", Console::getHighestPriorityClassification());
    }
}
void Input::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods){
    //TODO: use KeyData for this internally
}
void Input::characterCallback(GLFWwindow* window, unsigned int codepoint){
    
}
void Input::cursorPositionCallback(GLFWwindow* window, double xpos, double ypos){
    
}
void Input::cursorEnterCallback(GLFWwindow* window, int entered){
    
}
void Input::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods){
    //TODO: use MouseKeyData for this internally
} 
void Input::scrollCallback(GLFWwindow* window, double xoffset, double yoffset){
    
}
void Input::charModsCallback(GLFWwindow* window, unsigned int codepoint, int mods){
    
}
void Input::dropCallback(GLFWwindow* window, int count, const char** paths){

}