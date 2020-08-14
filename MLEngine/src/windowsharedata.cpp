#include <windowsharedata.h>

#include <GLFW/glfw3.h>
using namespace mle;
WindowShareData::WindowShareData() : WindowShareData(nullptr){

}
WindowShareData::WindowShareData(GLFWwindow* in_window) : window(in_window){

}