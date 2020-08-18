#ifndef FRAMETIME_H
#define FRAMETIME_H

#include <mlengine_global.h>
namespace mle{
class MLENGINE_SHARED_EXPORT FrameTime{
public:
    FrameTime();
    void newFrame(const double frameTime);
    double getLastFrameTime() const;

private:
    double m_lastFrame;
};
};

#endif