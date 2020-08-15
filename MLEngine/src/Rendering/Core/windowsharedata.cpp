#include <Rendering/Core/windowsharedata.h>

using namespace mle;
WindowShareData::WindowShareData() : WindowShareData(nullptr){

}
WindowShareData::WindowShareData(GLFWwindow* in_window) : window(in_window){

}