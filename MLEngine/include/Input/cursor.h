#ifndef CURSOR_H
#define CURSOR_H

#include <mlengine_global.h>

struct GLFWcursor;
namespace mle{
class RenderingManager;
struct MLENGINE_SHARED_EXPORT Cursor{
    Cursor();
    friend class RenderingManager;
private:
    Cursor(GLFWcursor* cursor);
    GLFWcursor* cursor;
};
};
#endif