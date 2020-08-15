#ifndef IMAGEDATA_H
#define IMAGEDATA_H

#include <mlengine_global.h>
struct GLFWimage;
namespace mle{
struct MLENGINE_SHARED_EXPORT ImageData{
    ImageData();
    friend class RenderingManager;
private:
    ImageData(GLFWimage* in_images, const size_t in_count);
    GLFWimage* images;
    size_t count;
};
};

#endif