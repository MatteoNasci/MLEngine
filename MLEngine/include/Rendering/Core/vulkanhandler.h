#ifndef VULKANHANDLER_H
#define VULKANHANDLER_H

#include <glm/glm.hpp>

#include <mlengine_global.h>

#include <Engine/Core/engineerror.h>
#include <Rendering/Core/contextinitdata.h>

#include <string>
#include <vector>
#include <set>

namespace mle{
class MLENGINE_SHARED_EXPORT VulkanHandler{
public:
    static EngineError initialize(const ContextInitData& data);
    static EngineError release();
    static EngineError addExtension(const std::string& ext);
    static bool isExtensionLoaded(const std::string& ext);
    static void advanceLoggerTime(const double timeToAdd);
    static void notifyFramebufferResized();

    static void waitForDeviceIdle();

    static EngineError drawFrame();

private: VulkanHandler() = delete;
    static EngineError checkPreInit(const ContextInitData& context_data, std::vector<const char*>& out_extensions);
    static EngineError createInstance(const ContextInitData& context_data, const std::vector<const char*>& extensions);
    static EngineError setupDebugMessengers(const ContextInitData& context_data);
    static EngineError setupPhysicalDevices(const ContextInitData& context_data);
    static EngineError setupLogicalDevices(const ContextInitData& context_data);
    static EngineError createSurface(const ContextInitData& context_data);
    static EngineError createSwapChain();
    static EngineError createImageViews();
    static EngineError createRenderPass();
    static EngineError createGraphicPipeline();
    static EngineError createFrameBuffers();
    static EngineError createCommandPool();
    static EngineError createCommandBuffers();
    static EngineError createSyncObjects();
    static EngineError createVertexBuffer();

    static EngineError recreateSwapChain();
    static EngineError cleanupSwapChain();
};
};

#endif