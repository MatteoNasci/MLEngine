#include <Engine/Time/frametime.h>

using namespace mle;

FrameTime::FrameTime() : m_lastFrame(0.0){

}

void FrameTime::newFrame(const double frameTime){
    m_lastFrame = frameTime;
}
double FrameTime::getLastFrameTime() const{
    return m_lastFrame;
}