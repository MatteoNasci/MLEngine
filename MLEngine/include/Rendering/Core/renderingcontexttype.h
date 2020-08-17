#ifndef RENDERINGCONTEXTTYPE_H
#define RENDERINGCONTEXTTYPE_H

namespace mle{
enum class RenderingContextType{
    None = 0,
    Opengl,
    Vulkan,
    DirectX11,
    DirectX12,
};
};

#endif