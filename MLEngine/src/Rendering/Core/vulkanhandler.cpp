#include <Rendering/Core/vulkanhandler.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <Engine/Debug/console.h>
#include <Engine/Time/timermanager.h>
#include <FileSystem/fileutils.h>

#include <unordered_map>
#include <cstring>
#include <set>
#include <limits>
#include <optional>
#include <algorithm>
#include <array>



using namespace mle;

struct UniformBufferObject {
    alignas(16) glm::vec2 random_value;
    alignas(16) glm::mat4 model; //alignas required in this case in order to have a correct data alignment, to avoid preblems just use it if it's data to send to shaders
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 proj;
};
struct VertexData{
    glm::vec2 pos;
    glm::vec3 color;
    static VkVertexInputBindingDescription getBindingDescription() {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(VertexData);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescription;
    }
    static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions(){
        std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};
        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(VertexData, pos);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(VertexData, color);

        return attributeDescriptions;
    }
};
struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};
struct LogicalDevice{
    VkDevice device;
    VkQueue graphicsQueue;
    VkQueue presentQueue;
};
struct QueueFamilyIndices{
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isCompleted(){
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};
struct PhysicalDeviceScored{
    PhysicalDeviceScored() : device(VK_NULL_HANDLE), score(std::numeric_limits<double>::min()) {

    }
    PhysicalDeviceScored(VkPhysicalDevice in_device, const double in_score, QueueFamilyIndices in_indices, VkPhysicalDeviceProperties in_properties, VkPhysicalDeviceFeatures in_features) : 
        device(in_device), 
        score(in_score),
        indices(in_indices),
        properties(in_properties),
        features(in_features)
    {

    }
    
    QueueFamilyIndices indices;
    VkPhysicalDevice device;
    VkPhysicalDeviceProperties properties;
    VkPhysicalDeviceFeatures features;
    double score;

    friend bool operator<(const PhysicalDeviceScored& lhs, const PhysicalDeviceScored& rhs){
        return lhs.score < rhs.score;
    }
    friend bool operator>(const PhysicalDeviceScored& lhs, const PhysicalDeviceScored& rhs){
        return lhs.score > rhs.score;
    }
};
struct VulkanData{
    VulkanData(){}
    std::vector<VkDebugUtilsMessengerEXT> debugMessengers;
    std::multiset<PhysicalDeviceScored, std::greater<PhysicalDeviceScored>> physical_devices; //This is cleaned up by vkDestroyInstance
    std::optional<LogicalDevice> logical_device;
    std::optional<VkSurfaceKHR> surface;
    std::optional<VkSwapchainKHR> swap_chain;
    std::vector<VkImage> swapChainImages;
    std::vector<VkImageView> swapChainImageViews;
    std::optional<VkPipelineLayout> pipelineLayout;//TODO: check carefully all optional fields initializzation to avoid initialized but not valid fields
    std::optional<VkRenderPass> renderPass;
    std::optional<VkPipeline> graphicsPipeline;
    std::vector<VkFramebuffer> swapChainFramebuffers;
    std::optional<VkCommandPool> commandPool;
    std::vector<VkCommandBuffer> commandBuffers;
    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;
    std::vector<VkFence> imagesInFlight;
    std::optional<VkBuffer> vertexBuffer;
    std::optional<VkDeviceMemory> vertexBufferMemory;
    std::optional<VkBuffer> indexBuffer;
    std::optional<VkDeviceMemory> indexBufferMemory;
    std::optional<VkDescriptorSetLayout> descriptorSetLayout;
    std::vector<VkBuffer> uniformBuffers;
    std::vector<VkDeviceMemory> uniformBuffersMemory;
    std::optional<VkDescriptorPool> descriptorPool;
    std::vector<VkDescriptorSet> descriptorSets;
    std::optional<VkImage> textureImage;
    std::optional<VkDeviceMemory> textureImageMemory;
    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;
    std::optional<ContextInitData> context_data;
    bool framebufferResized = false;

    std::unordered_map<std::string, void(*)(void)> extensions_available;

    std::optional<VkInstance> instance;

    PhysicalDeviceScored getMainPhysicalDevice(){
        return *(physical_devices.cbegin());
    }
};
struct Logger{
    Logger() : timerManager(), console(Console("VulkanLayerLogs.txt", timerManager)){
        
    }
    TimerManager timerManager;
    Console console;
};
/*
TODO:
- Allocate memory from Vulkan only once and handle allocations within that space yourself (see https://developer.nvidia.co...
- Don't implement a draw() call on your objects that directly draws them, but rather schedule drawing so that your engine controls the actual draw order. You can then sort objects by material to minimize the number of graphics pipeline switches. That switching is the main expense and not so much the pipeline objects themselves.
*/
static std::set<std::string> s_extensionsToAdd;
static std::vector<VkExtensionProperties> s_availableExtensions;
static Logger s_logger;
static VulkanData s_state;
static size_t currentFrame = 0;

static const std::vector<VertexData> vertices = {
    {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
    {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
    {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
    {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
};
static const std::vector<uint16_t> indices = {
    0, 1, 2, 2, 3, 0
};


static const size_t MaxFramesInFlight = 2;

static VKAPI_ATTR VkBool32 VKAPI_CALL vulkanDebugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData);
static EngineError checkVulkanExtensionsValidity(const std::vector<const char*>& requested_layers, std::vector<VkExtensionProperties>& out_extensions);
static EngineError checkVulkanLayersValidity(const std::vector<const char*>& requested_layers);
static EngineError saveAvailableExtensions(const std::set<std::string>& extensions);
static EngineError getAndClearGlfwError();
static void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
static double isPhysicalDeviceSuitable(const ContextInitData& context_data, const VkPhysicalDevice& device, const VkSurfaceKHR& surface, QueueFamilyIndices& out_queueFamily, VkPhysicalDeviceProperties& out_p, VkPhysicalDeviceFeatures& out_f);
static const EngineError findQueueFamilies(const VkPhysicalDevice& device, const VkSurfaceKHR& surface, QueueFamilyIndices& out_indices);
static const bool isDeviceExtensionSupported(const ContextInitData& context_data, const VkPhysicalDevice& device);
static EngineError querySwapChainSupport(const VkPhysicalDevice& device, SwapChainSupportDetails& out_details);
static VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
static VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
static VkExtent2D chooseSwapExtent(GLFWwindow* window, const VkSurfaceCapabilitiesKHR& capabilities);
static EngineError createShaderModule(const std::vector<char>& code, VkShaderModule& out_module);
static void waitForLogging();
static uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
static EngineError createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
static EngineError copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
static EngineError updateUniformBuffer(uint32_t currentImage);
static void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
static VkCommandBuffer beginSingleTimeCommands();
static void endSingleTimeCommands(VkCommandBuffer commandBuffer);
static void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
static void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);


static void waitForLogging(){
    while(s_logger.console.isLoggingToFile()){
        VulkanHandler::advanceLoggerTime(10);
    }
}
static void waitForLoggingWithText(const std::string& text){
    s_logger.console.log(text, Console::getHighestPriorityClassification());
    while(s_logger.console.isLoggingToFile()){
        VulkanHandler::advanceLoggerTime(10);
    }
}


void VulkanHandler::advanceLoggerTime(const double timeToAdd){
    s_logger.timerManager.advanceTime(timeToAdd);
}
EngineError VulkanHandler::addExtension(const std::string& ext){
    EngineError result = EngineError::Ok;
    if(!s_extensionsToAdd.count(ext)){
        s_extensionsToAdd.insert(ext);

        if(s_state.instance.has_value()){
            result = saveAvailableExtensions(s_extensionsToAdd);
            s_extensionsToAdd.clear();
        }
    }

    return result;
}
bool VulkanHandler::isExtensionLoaded(const std::string& ext){
    return s_state.extensions_available.count(ext);
}
EngineError VulkanHandler::createSwapChainObjects(){
    createSwapChain();
    createImageViews();
    createRenderPass();
    createFrameBuffers();
    createUniformBuffers();
    createDescriptorPool();
    createDescriptorSets();
    createCommandBuffers(); //TODO: use this function in initialize to avoid duplicated code (check if anything breaks after)

    return EngineError::Ok;
}
EngineError VulkanHandler::initialize(const ContextInitData& context_data){
    std::vector<const char*> extensions;
    auto preInitError = checkPreInit(context_data, extensions);
    if(preInitError != EngineError::Ok){
        return preInitError;
    }

    auto createInstanceError = createInstance(context_data, extensions);
    if(createInstanceError != EngineError::Ok){
        return createInstanceError;
    }

    auto savingAdressedError = saveAvailableExtensions(s_extensionsToAdd);
    s_extensionsToAdd.clear();

    auto settingDebugMessengersError = setupDebugMessengers(context_data);

    auto createSurfaceError = createSurface(context_data);

    auto setupPhysicalDevicesError = setupPhysicalDevices(context_data);
    if(setupPhysicalDevicesError != EngineError::Ok){
        return setupPhysicalDevicesError;
    }

    auto setupLogicalDevicesError = setupLogicalDevices(context_data);

    s_state.context_data = context_data;

    auto createSwapChainError = createSwapChain();

    auto createImagesError = createImageViews();

    auto createRenderPassError = createRenderPass();

    auto createDescriptorSetLayoutError = createDescriptorSetLayout();

    auto createGraphicPipelineError = createGraphicPipeline();

    auto createFrameBuffersError = createFrameBuffers();

    auto createCommandPoolError = createCommandPool();

    auto createTextureImageError = createTextureImage();

    auto createVertexBufferError = createVertexBuffer();

    auto createIndexBufferError = createIndexBuffer();

    auto createUniformBufferError = createUniformBuffers();

    auto createDescriptorPoolError = createDescriptorPool();

    auto createDescriptorSetsError = createDescriptorSets();

    auto createCommandBuffersError = createCommandBuffers();

    auto createSemaphoresError = createSyncObjects();

    return EngineError::Ok;
}
EngineError VulkanHandler::release(){
    s_logger.console.log("Releasing Vulkan resources...", Console::getHighestPriorityClassification());

    if(s_state.logical_device.has_value()){ 
        for(size_t i = 0; i < s_state.inFlightFences.size(); ++i){
            vkDestroyFence(s_state.logical_device.value().device, s_state.inFlightFences[i], nullptr);
        }
        s_state.inFlightFences.clear();

        for(size_t i = 0; i < s_state.renderFinishedSemaphores.size(); ++i){
            vkDestroySemaphore(s_state.logical_device.value().device, s_state.renderFinishedSemaphores[i], nullptr);
        }
        s_state.renderFinishedSemaphores.clear();

        for(size_t i = 0; i < s_state.imageAvailableSemaphores.size(); ++i){
            vkDestroySemaphore(s_state.logical_device.value().device, s_state.imageAvailableSemaphores[i], nullptr);
        }
        s_state.imageAvailableSemaphores.clear();

        if(s_state.commandPool.has_value()){
            vkDestroyCommandPool(s_state.logical_device.value().device, s_state.commandPool.value(), nullptr);
        }
        s_state.commandPool.reset();

        if(s_state.graphicsPipeline.has_value()){
            vkDestroyPipeline(s_state.logical_device.value().device, s_state.graphicsPipeline.value(), nullptr);
        }
        s_state.graphicsPipeline.reset();

        if(s_state.pipelineLayout.has_value()){
            vkDestroyPipelineLayout(s_state.logical_device.value().device, s_state.pipelineLayout.value(), nullptr);
        }
        s_state.pipelineLayout.reset();

        cleanupSwapChain();

        if(s_state.textureImage.has_value()){
            vkDestroyImage(s_state.logical_device.value().device, s_state.textureImage.value(), nullptr);
        }
        s_state.textureImage.reset();

        if(s_state.textureImageMemory.has_value()){
            vkFreeMemory(s_state.logical_device.value().device, s_state.textureImageMemory.value(), nullptr);
        }
        s_state.textureImageMemory.reset();

        if(s_state.descriptorSetLayout.has_value()){
            vkDestroyDescriptorSetLayout(s_state.logical_device.value().device, s_state.descriptorSetLayout.value(), nullptr);
        }
        s_state.descriptorSetLayout.reset();

        if(s_state.vertexBuffer.has_value()){
            vkDestroyBuffer(s_state.logical_device.value().device, s_state.vertexBuffer.value(), nullptr);
        }
        s_state.vertexBuffer.reset();

        if(s_state.vertexBufferMemory.has_value()){
            vkFreeMemory(s_state.logical_device.value().device, s_state.vertexBufferMemory.value(), nullptr);
        }
        s_state.vertexBufferMemory.reset();

        if(s_state.indexBuffer.has_value()){
            vkDestroyBuffer(s_state.logical_device.value().device, s_state.indexBuffer.value(), nullptr);
        }
        s_state.indexBuffer.reset();

        if(s_state.indexBufferMemory.has_value()){
            vkFreeMemory(s_state.logical_device.value().device, s_state.indexBufferMemory.value(), nullptr);
        }
        s_state.indexBufferMemory.reset();

        vkDestroyDevice(s_state.logical_device.value().device, nullptr);
    }
    s_state.logical_device.reset();

    if(s_state.instance.has_value()){
        if(s_state.surface.has_value()){
            vkDestroySurfaceKHR(s_state.instance.value(), s_state.surface.value(), nullptr);
        }
        s_state.surface.reset();

        if(s_state.extensions_available.count("vkDestroyDebugUtilsMessengerEXT")){
            auto destroyer = (PFN_vkDestroyDebugUtilsMessengerEXT)s_state.extensions_available["vkDestroyDebugUtilsMessengerEXT"];
            for(size_t i = 0; i < s_state.debugMessengers.size(); ++i){
                const auto& element = s_state.debugMessengers[i];
                destroyer(s_state.instance.value(), element, nullptr);
            }
        }else{
            s_logger.console.log("Vulkan could not find the required extension to use to delete created debug messengers!", Console::getHighestPriorityClassification());
        }
        s_state.debugMessengers.clear();

        vkDestroyInstance(s_state.instance.value(), nullptr);
    }
    s_state.instance.reset();

    s_state.context_data.reset();

    waitForLogging();

    return EngineError::Ok;
}
EngineError VulkanHandler::cleanupSwapChain(){
    if(s_state.logical_device.has_value()){
        for(size_t i = 0; i < s_state.swapChainFramebuffers.size(); ++i){
            vkDestroyFramebuffer(s_state.logical_device.value().device, s_state.swapChainFramebuffers[i], nullptr);
        }
        s_state.swapChainFramebuffers.clear();

        vkFreeCommandBuffers(s_state.logical_device.value().device, s_state.commandPool.value(), static_cast<uint32_t>(s_state.commandBuffers.size()), s_state.commandBuffers.data());

        if(s_state.renderPass.has_value()){
            vkDestroyRenderPass(s_state.logical_device.value().device, s_state.renderPass.value(), nullptr);
        }
        s_state.renderPass.reset();

        for(size_t i = 0; i < s_state.swapChainImageViews.size(); ++i){
            vkDestroyImageView(s_state.logical_device.value().device, s_state.swapChainImageViews[i], nullptr);
        }
        s_state.swapChainImageViews.clear();

        if(s_state.swap_chain.has_value()){
            vkDestroySwapchainKHR(s_state.logical_device.value().device, s_state.swap_chain.value(), nullptr);
        }
        s_state.swap_chain.reset();

        for (size_t i = 0; i < s_state.uniformBuffers.size(); i++) {
            vkDestroyBuffer(s_state.logical_device.value().device, s_state.uniformBuffers[i], nullptr);
        }
        s_state.uniformBuffers.clear();

        for (size_t i = 0; i < s_state.uniformBuffersMemory.size(); i++) {
            vkFreeMemory(s_state.logical_device.value().device, s_state.uniformBuffersMemory[i], nullptr);
        }
        s_state.uniformBuffersMemory.clear();

        if(s_state.descriptorPool.has_value()){
            vkDestroyDescriptorPool(s_state.logical_device.value().device, s_state.descriptorPool.value(), nullptr);
        }
        s_state.descriptorPool.reset();
    }

    return EngineError::Ok;
}





static void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory) {
    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = width;
    imageInfo.extent.height = height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = format;
    imageInfo.tiling = tiling;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = usage;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateImage(s_state.logical_device.value().device, &imageInfo, nullptr, &image) != VK_SUCCESS) {
        throw std::runtime_error("failed to create image!");
    }

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(s_state.logical_device.value().device, image, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(s_state.logical_device.value().device, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate image memory!");
    }

    vkBindImageMemory(s_state.logical_device.value().device, image, imageMemory, 0);
}
EngineError VulkanHandler::createTextureImage() {
    int texWidth, texHeight, texChannels;
    stbi_uc* pixels = stbi_load("D:/Personal/MLEngine/MLEngine/textures/statue.jpg", &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    VkDeviceSize imageSize = texWidth * texHeight * 4;

    if (!pixels) {
        throw std::runtime_error("failed to load texture image!");
    }

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

    void* data;
    vkMapMemory(s_state.logical_device.value().device, stagingBufferMemory, 0, imageSize, 0, &data);
    memcpy(data, pixels, static_cast<size_t>(imageSize));
    vkUnmapMemory(s_state.logical_device.value().device, stagingBufferMemory);

    stbi_image_free(pixels);

    VkImage textureImage;
    VkDeviceMemory textureImageMemory;
    createImage(texWidth, texHeight, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, textureImage, textureImageMemory);
    s_state.textureImage = textureImage;
    s_state.textureImageMemory = textureImageMemory;

    transitionImageLayout(s_state.textureImage.value(), VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    copyBufferToImage(stagingBuffer, textureImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));
    transitionImageLayout(s_state.textureImage.value(), VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    vkDestroyBuffer(s_state.logical_device.value().device, stagingBuffer, nullptr);
    vkFreeMemory(s_state.logical_device.value().device, stagingBufferMemory, nullptr);

    return EngineError::Ok;
}
static VkCommandBuffer beginSingleTimeCommands() {
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = s_state.commandPool.value();
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(s_state.logical_device.value().device, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    return commandBuffer;
}
static void endSingleTimeCommands(VkCommandBuffer commandBuffer) {
    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(s_state.logical_device.value().graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(s_state.logical_device.value().graphicsQueue);

    vkFreeCommandBuffers(s_state.logical_device.value().device, s_state.commandPool.value(), 1, &commandBuffer);
}
EngineError VulkanHandler::createDescriptorSets(){
    std::vector<VkDescriptorSetLayout> layouts(s_state.swapChainImages.size(), s_state.descriptorSetLayout.value());
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = s_state.descriptorPool.value();
    allocInfo.descriptorSetCount = static_cast<uint32_t>(s_state.swapChainImages.size());
    allocInfo.pSetLayouts = layouts.data();
    
    s_state.descriptorSets.resize(s_state.swapChainImages.size());
    if (vkAllocateDescriptorSets(s_state.logical_device.value().device, &allocInfo, s_state.descriptorSets.data()) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate descriptor sets!");
    }

    for (size_t i = 0; i < s_state.swapChainImages.size(); i++) {
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = s_state.uniformBuffers[i];
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(UniformBufferObject);

        VkWriteDescriptorSet descriptorWrite{};
        descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite.dstSet = s_state.descriptorSets[i];
        descriptorWrite.dstBinding = 0;
        descriptorWrite.dstArrayElement = 0;

        descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrite.descriptorCount = 1;

        descriptorWrite.pBufferInfo = &bufferInfo;
        descriptorWrite.pImageInfo = nullptr; // Optional
        descriptorWrite.pTexelBufferView = nullptr; // Optional

        vkUpdateDescriptorSets(s_state.logical_device.value().device, 1, &descriptorWrite, 0, nullptr);
    }
 

    return EngineError::Ok;
}
EngineError VulkanHandler::createDescriptorPool(){
    VkDescriptorPoolSize poolSize{};
    poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSize.descriptorCount = static_cast<uint32_t>(s_state.swapChainImages.size());

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = 1;
    poolInfo.pPoolSizes = &poolSize;
    poolInfo.maxSets = static_cast<uint32_t>(s_state.swapChainImages.size());

    VkDescriptorPool descriptorPool;
    if (vkCreateDescriptorPool(s_state.logical_device.value().device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor pool!");
    }
    s_state.descriptorPool = descriptorPool;

    return EngineError::Ok;
}
static EngineError updateUniformBuffer(uint32_t currentImage){
    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    UniformBufferObject ubo{};
    ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));

    ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));

    ubo.proj = glm::perspective(glm::radians(45.0f), s_state.swapChainExtent.width /(float)s_state.swapChainExtent.height, 0.1f, 10.0f);

    ubo.proj[1][1] *= -1;

    void* data;
    vkMapMemory(s_state.logical_device.value().device, s_state.uniformBuffersMemory[currentImage], 0, sizeof(ubo), 0, &data);
    memcpy(data, &ubo, sizeof(ubo));
    vkUnmapMemory(s_state.logical_device.value().device, s_state.uniformBuffersMemory[currentImage]);

    return EngineError::Ok;
}
EngineError VulkanHandler::createUniformBuffers(){
    VkDeviceSize bufferSize = sizeof(UniformBufferObject);

    s_state.uniformBuffers.resize(s_state.swapChainImages.size());
    s_state.uniformBuffersMemory.resize(s_state.swapChainImages.size());

    for (size_t i = 0; i < s_state.swapChainImages.size(); i++) {
        createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, s_state.uniformBuffers[i], s_state.uniformBuffersMemory[i]);
    }


    return EngineError::Ok;
}
EngineError VulkanHandler::createDescriptorSetLayout(){
    VkDescriptorSetLayoutBinding uboLayoutBinding{};
    uboLayoutBinding.binding = 0;
    uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboLayoutBinding.descriptorCount = 1;

    uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    uboLayoutBinding.pImmutableSamplers = nullptr; // Optional

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = 1;
    layoutInfo.pBindings = &uboLayoutBinding;

    VkDescriptorSetLayout descriptorSetLayout;
    if (vkCreateDescriptorSetLayout(s_state.logical_device.value().device, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor set layout!");
    }
    s_state.descriptorSetLayout = descriptorSetLayout;

    return EngineError::Ok;
}
EngineError VulkanHandler::createIndexBuffer(){
    VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

    void* data;
    vkMapMemory(s_state.logical_device.value().device, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, indices.data(), (size_t) bufferSize);
    vkUnmapMemory(s_state.logical_device.value().device, stagingBufferMemory);

    VkBuffer indexBuffer;
    VkDeviceMemory indexBufferMemory;
    createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer, indexBufferMemory);

    copyBuffer(stagingBuffer, indexBuffer, bufferSize);

    s_state.indexBuffer = indexBuffer;
    s_state.indexBufferMemory = indexBufferMemory;

    vkDestroyBuffer(s_state.logical_device.value().device, stagingBuffer, nullptr);
    vkFreeMemory(s_state.logical_device.value().device, stagingBufferMemory, nullptr);

    return EngineError::Ok;
}
static EngineError createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory) {
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(s_state.logical_device.value().device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to create buffer!");
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(s_state.logical_device.value().device, buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(s_state.logical_device.value().device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate buffer memory!");
    }

    vkBindBufferMemory(s_state.logical_device.value().device, buffer, bufferMemory, 0);

    return EngineError::Ok;
}
EngineError VulkanHandler::createVertexBuffer(){
    VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

    void* data;
    vkMapMemory(s_state.logical_device.value().device, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, vertices.data(), (size_t) bufferSize);
    vkUnmapMemory(s_state.logical_device.value().device, stagingBufferMemory);

    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;
    createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer, vertexBufferMemory);

    copyBuffer(stagingBuffer, vertexBuffer, bufferSize);

    s_state.vertexBuffer = vertexBuffer;
    s_state.vertexBufferMemory = vertexBufferMemory;

    vkDestroyBuffer(s_state.logical_device.value().device, stagingBuffer, nullptr);
    vkFreeMemory(s_state.logical_device.value().device, stagingBufferMemory, nullptr);

    return EngineError::Ok;
}
void VulkanHandler::notifyFramebufferResized(){
    s_state.framebufferResized = true;
}
EngineError VulkanHandler::recreateSwapChain(){
    int width = 0, height = 0;
    glfwGetFramebufferSize(s_state.context_data.value().window.window, &width, &height);
    while (width == 0 || height == 0) {
        glfwGetFramebufferSize(s_state.context_data.value().window.window, &width, &height);
        glfwWaitEvents();
    }

    vkDeviceWaitIdle(s_state.logical_device.value().device);

    cleanupSwapChain();

    createSwapChainObjects();

    return EngineError::Ok;
}
void VulkanHandler::waitForDeviceIdle(){
    vkDeviceWaitIdle(s_state.logical_device.value().device);
}
EngineError VulkanHandler::drawFrame(){
    vkWaitForFences(s_state.logical_device.value().device, 1, &s_state.inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

    uint32_t imageIndex;
    const auto acquireResult = static_cast<EngineError>(vkAcquireNextImageKHR(s_state.logical_device.value().device, s_state.swap_chain.value(), UINT64_MAX, s_state.imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex));

    if (acquireResult == EngineError::VK_ErrorOutOfDateKHR) {      
        return recreateSwapChain();
    } else if (acquireResult != EngineError::Ok && acquireResult != EngineError::VK_SuboptimalKHR) {
        return acquireResult;
    }
    // Check if a previous frame is using this image (i.e. there is its fence to wait on)
    if (s_state.imagesInFlight[imageIndex] != VK_NULL_HANDLE) {
        vkWaitForFences(s_state.logical_device.value().device, 1, &s_state.imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
    }
    // Mark the image as now being in use by this frame
    s_state.imagesInFlight[imageIndex] = s_state.inFlightFences[currentFrame];

    updateUniformBuffer(imageIndex);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = {s_state.imageAvailableSemaphores[currentFrame]};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &s_state.commandBuffers[imageIndex];

    VkSemaphore signalSemaphores[] = {s_state.renderFinishedSemaphores[currentFrame]};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    vkResetFences(s_state.logical_device.value().device, 1, &s_state.inFlightFences[currentFrame]);
    if (vkQueueSubmit(s_state.logical_device.value().graphicsQueue, 1, &submitInfo, s_state.inFlightFences[currentFrame]) != VK_SUCCESS) {
        return EngineError::VK_FailedToSubmitDrawCommandBuffer;
    }

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = {s_state.swap_chain.value()};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;
    presentInfo.pResults = nullptr; // Optional, needed to check for error with each individual swapchain when there are more than one

    auto presentResult = static_cast<EngineError>(vkQueuePresentKHR(s_state.logical_device.value().presentQueue, &presentInfo));
    if (presentResult == EngineError::VK_ErrorOutOfDateKHR || presentResult == EngineError::VK_SuboptimalKHR || s_state.framebufferResized) {
        s_state.framebufferResized = false;
        presentResult = recreateSwapChain();
    }

    currentFrame = (currentFrame + 1) % MaxFramesInFlight;

    return presentResult;
}
EngineError VulkanHandler::createSyncObjects(){
    s_state.imageAvailableSemaphores.resize(MaxFramesInFlight);
    s_state.renderFinishedSemaphores.resize(MaxFramesInFlight);
    s_state.inFlightFences.resize(MaxFramesInFlight);
    s_state.imagesInFlight.resize(s_state.swapChainImages.size(), VK_NULL_HANDLE);

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for(size_t i = 0; i < MaxFramesInFlight; ++i){
        if (vkCreateSemaphore(s_state.logical_device.value().device, &semaphoreInfo, nullptr, &s_state.imageAvailableSemaphores[i]) != VK_SUCCESS ||
            vkCreateSemaphore(s_state.logical_device.value().device, &semaphoreInfo, nullptr, &s_state.renderFinishedSemaphores[i]) != VK_SUCCESS ||
            vkCreateFence(s_state.logical_device.value().device, &fenceInfo, nullptr, &s_state.inFlightFences[i]) != VK_SUCCESS){
            
            return EngineError::VK_FailedCreateSyncObjects;
        }
    }

    return EngineError::Ok;
}
EngineError VulkanHandler::createCommandBuffers(){
    s_state.commandBuffers.resize(s_state.swapChainFramebuffers.size());
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = s_state.commandPool.value();
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = static_cast<uint32_t>(s_state.commandBuffers.size());

    if (vkAllocateCommandBuffers(s_state.logical_device.value().device, &allocInfo, s_state.commandBuffers.data()) != VK_SUCCESS) {
        return EngineError::VK_FailedCreateCommandBuffers;
    }

    for (size_t i = 0; i < s_state.commandBuffers.size(); i++) {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = 0; // Optional
        beginInfo.pInheritanceInfo = nullptr; // Optional

        if (vkBeginCommandBuffer(s_state.commandBuffers[i], &beginInfo) != VK_SUCCESS) {
            return EngineError::VK_FailedToBeginCommandBuffer;
        }

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = s_state.renderPass.value();
        renderPassInfo.framebuffer = s_state.swapChainFramebuffers[i];

        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = s_state.swapChainExtent;

        VkClearValue clearColor = {0.0f, 0.0f, 0.0f, 1.0f};
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = &clearColor;

        vkCmdBeginRenderPass(s_state.commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        vkCmdBindPipeline(s_state.commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, s_state.graphicsPipeline.value());

        VkViewport viewport{}; //Represents the window rect portion where the image will be seen
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(s_state.swapChainExtent.width);
        viewport.height = static_cast<float>(s_state.swapChainExtent.height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(s_state.commandBuffers[i], 0, 1, &viewport);

        VkRect2D scissor{}; //represents the image rect portion that will be displayed
        scissor.offset = {0, 0};
        scissor.extent = s_state.swapChainExtent;
        vkCmdSetScissor(s_state.commandBuffers[i], 0, 1, &scissor);

        VkBuffer vertexBuffers[] = {s_state.vertexBuffer.value()};
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(s_state.commandBuffers[i], 0, 1, vertexBuffers, offsets);

        vkCmdBindIndexBuffer(s_state.commandBuffers[i], s_state.indexBuffer.value(), 0, VK_INDEX_TYPE_UINT16);

        vkCmdBindDescriptorSets(s_state.commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, s_state.pipelineLayout.value(), 0, 1, &s_state.descriptorSets[i], 0, nullptr);

        //vkCmdDraw(s_state.commandBuffers[i], static_cast<uint32_t>(vertices.size()), 1, 0, 0);
        vkCmdDrawIndexed(s_state.commandBuffers[i], static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);

        vkCmdEndRenderPass(s_state.commandBuffers[i]);

        if (vkEndCommandBuffer(s_state.commandBuffers[i]) != VK_SUCCESS) {
            return EngineError::VK_FailedToEndCommandBuffer;
        }
    }

    return EngineError::Ok;
}
EngineError VulkanHandler::createCommandPool(){
    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = s_state.physical_devices.cbegin()->indices.graphicsFamily.value();
    poolInfo.flags = 0; // Optional

    VkCommandPool commandPool;
    if (vkCreateCommandPool(s_state.logical_device.value().device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
        return EngineError::VK_FailedCreateCommandPool;
    }
    s_state.commandPool = commandPool;

    return EngineError::Ok;
}
EngineError VulkanHandler::createFrameBuffers(){
    s_state.swapChainFramebuffers.resize(s_state.swapChainImageViews.size());

    for (size_t i = 0; i < s_state.swapChainImageViews.size(); i++) {
        VkImageView attachments[] = {
            s_state.swapChainImageViews[i]
        };

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = s_state.renderPass.value();
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = s_state.swapChainExtent.width;
        framebufferInfo.height = s_state.swapChainExtent.height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(s_state.logical_device.value().device, &framebufferInfo, nullptr, &s_state.swapChainFramebuffers[i]) != VK_SUCCESS) {
            return EngineError::VK_FailedCreateFrameBuffers;
        }
    }

    return EngineError::Ok;
}
EngineError VulkanHandler::createGraphicPipeline(){
    EngineError result = EngineError::Ok;

    std::vector<char> vert_data, frag_data;
    auto vertShaderError = FileUtils::readAllFile("D:/Personal/MLEngine/MLEngine/shaders/compiled/vert.spv", vert_data);
    auto fragShaderError = FileUtils::readAllFile("D:/Personal/MLEngine/MLEngine/shaders/compiled/frag.spv", frag_data);

    VkShaderModule vertModule, fragModule;
    auto vertShaderModuleError = createShaderModule(vert_data, vertModule);
    auto fragShaderModuleError = createShaderModule(frag_data, fragModule);

    VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertModule;
    vertShaderStageInfo.pName = "main";
    vertShaderStageInfo.pSpecializationInfo = nullptr; //Set constants

    VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragModule;
    fragShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

    auto bindingDescription = VertexData::getBindingDescription();
    auto attributeDescriptions = VertexData::getAttributeDescriptions();
    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription; // Optional
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());;
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data(); // Optional

    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    /*VkViewport viewport{}; //Represents the window rect portion where the image will be seen
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(s_state.swapChainExtent.width);
    viewport.height = static_cast<float>(s_state.swapChainExtent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{}; //represents the image rect portion that will be displayed
    scissor.offset = {0, 0};
    scissor.extent = s_state.swapChainExtent;*/ //commented now since we use dynamic states

    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports = nullptr;//&viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = nullptr;//&scissor;

    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;

    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;

    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;

    rasterizer.depthBiasEnable = VK_FALSE;
    rasterizer.depthBiasConstantFactor = 0.0f; // Optional
    rasterizer.depthBiasClamp = 0.0f; // Optional
    rasterizer.depthBiasSlopeFactor = 0.0f; // Optional

    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampling.minSampleShading = 1.0f; // Optional
    multisampling.pSampleMask = nullptr; // Optional
    multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
    multisampling.alphaToOneEnable = VK_FALSE; // Optional

    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;
    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
    colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
    colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
    colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
    colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
    colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional

    colorBlendAttachment.blendEnable = VK_TRUE;
    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
    colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f; // Optional
    colorBlending.blendConstants[1] = 0.0f; // Optional
    colorBlending.blendConstants[2] = 0.0f; // Optional
    colorBlending.blendConstants[3] = 0.0f; // Optional

    VkDynamicState dynamicStates[] = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };

    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = 2;
    dynamicState.pDynamicStates = dynamicStates;

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1; // Optional
    pipelineLayoutInfo.pSetLayouts = &s_state.descriptorSetLayout.value(); // Optional
    pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
    pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

    VkPipelineLayout layout;
    if (vkCreatePipelineLayout(s_state.logical_device.value().device, &pipelineLayoutInfo, nullptr, &layout) != VK_SUCCESS) {
        result = EngineError::VK_FailedCreatePipelineLayout;
    }else{
        s_state.pipelineLayout = layout;

        VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount = 2;
        pipelineInfo.pStages = shaderStages;

        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &inputAssembly;
        pipelineInfo.pViewportState = &viewportState;
        pipelineInfo.pRasterizationState = &rasterizer;
        pipelineInfo.pMultisampleState = &multisampling;
        pipelineInfo.pDepthStencilState = nullptr; // Optional
        pipelineInfo.pColorBlendState = &colorBlending;
        pipelineInfo.pDynamicState = &dynamicState; // Optional

        pipelineInfo.layout = s_state.pipelineLayout.value();

        pipelineInfo.renderPass = s_state.renderPass.value();
        pipelineInfo.subpass = 0;

        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
        pipelineInfo.basePipelineIndex = -1; // Optional

        VkPipeline pipeline;
        if (vkCreateGraphicsPipelines(s_state.logical_device.value().device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline) != VK_SUCCESS) {
            result = EngineError::VK_FailedCreatePipeline;
        }else{
            s_state.graphicsPipeline = pipeline;
        }
    }

    vkDestroyShaderModule(s_state.logical_device.value().device, vertModule, nullptr);
    vkDestroyShaderModule(s_state.logical_device.value().device, fragModule, nullptr);
    return result;
}
EngineError VulkanHandler::createRenderPass(){
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = s_state.swapChainImageFormat;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;

    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;

    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    VkRenderPass pass;
    if (vkCreateRenderPass(s_state.logical_device.value().device, &renderPassInfo, nullptr, &pass) != VK_SUCCESS) {
        return EngineError::VK_FailedCreateRenderPass;
    }
    s_state.renderPass = pass;

    return EngineError::Ok;
}
EngineError VulkanHandler::createImageViews(){
    s_state.swapChainImageViews.resize(s_state.swapChainImages.size());
    for (size_t i = 0; i < s_state.swapChainImages.size(); i++) {
        VkImageViewCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = s_state.swapChainImages[i];
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = s_state.swapChainImageFormat;

        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(s_state.logical_device.value().device, &createInfo, nullptr, &s_state.swapChainImageViews[i]) != VK_SUCCESS) {
            return EngineError::VK_ImageViewNotCreated;
        }
    }

    return EngineError::Ok;
}
EngineError VulkanHandler::createSurface(const ContextInitData& context_data){
    VkSurfaceKHR surface;
    if (glfwCreateWindowSurface(s_state.instance.value(), context_data.window.window, nullptr, &surface) != VK_SUCCESS) {
        return EngineError::VK_NoSurfaceCreated;
    }
    s_state.surface = surface;
    return EngineError::Ok;
}
EngineError VulkanHandler::createSwapChain(){
    const auto& physicalDevice = *(s_state.physical_devices.cbegin());
    SwapChainSupportDetails swapChainSupport;
    auto chainSupportError = querySwapChainSupport(physicalDevice.device, swapChainSupport);

    VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
    VkExtent2D extent = chooseSwapExtent(s_state.context_data.value().window.window, swapChainSupport.capabilities);

    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1; //we may sometimes have to wait on the driver to complete internal operations before we can acquire another image to render to. Therefore it is recommended to request at least one more image than the minimum
    if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = s_state.surface.value();

    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    uint32_t queueFamilyIndices[] = {physicalDevice.indices.graphicsFamily.value(), physicalDevice.indices.presentFamily.value()};
    if (physicalDevice.indices.graphicsFamily != physicalDevice.indices.presentFamily) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    } else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0; // Optional
        createInfo.pQueueFamilyIndices = nullptr; // Optional
    }

    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    VkSwapchainKHR swapChain;
    if (vkCreateSwapchainKHR(s_state.logical_device.value().device, &createInfo, nullptr, &swapChain) != VK_SUCCESS) {
        return EngineError::VK_SwapChainNotCreated;
    }
    s_state.swap_chain = swapChain;

    s_state.swapChainImageFormat = surfaceFormat.format;
    s_state.swapChainExtent = extent;

    vkGetSwapchainImagesKHR(s_state.logical_device.value().device, swapChain, &imageCount, nullptr);
    s_state.swapChainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(s_state.logical_device.value().device, swapChain, &imageCount, s_state.swapChainImages.data());

    return EngineError::Ok;
}
EngineError VulkanHandler::setupLogicalDevices(const ContextInitData& context_data){
    const auto& scoredDevice = s_state.getMainPhysicalDevice();

    VkDeviceQueueCreateInfo queueCreateInfo{};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = scoredDevice.indices.graphicsFamily.value();
    queueCreateInfo.queueCount = 1;
    float queuePriority = 1.0f;
    queueCreateInfo.pQueuePriorities = &queuePriority;

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = {scoredDevice.indices.graphicsFamily.value(), scoredDevice.indices.presentFamily.value()};

    for (uint32_t queueFamily : uniqueQueueFamilies) {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pQueueCreateInfos = &queueCreateInfo;
    createInfo.queueCreateInfoCount = 1;

    createInfo.pEnabledFeatures = &scoredDevice.features;
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos = queueCreateInfos.data();

    createInfo.ppEnabledExtensionNames = context_data.vulkan_logical_device_extensions.data();
    createInfo.enabledExtensionCount = static_cast<uint32_t>(context_data.vulkan_logical_device_extensions.size());

    if (context_data.vulkan_use_layers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(context_data.vulkan_layers.size());
        createInfo.ppEnabledLayerNames = context_data.vulkan_layers.data();
    } else {
        createInfo.enabledLayerCount = 0;
    }

    VkDevice device;
    if (vkCreateDevice(scoredDevice.device, &createInfo, nullptr, &device) != VK_SUCCESS) {
        return EngineError::VK_NoLogicalDeviceCouldBeCreated;
    }

    LogicalDevice ld;
    ld.device = std::move(device);
    vkGetDeviceQueue(ld.device, scoredDevice.indices.graphicsFamily.value(), 0, &ld.graphicsQueue);
    vkGetDeviceQueue(device, scoredDevice.indices.presentFamily.value(), 0, &ld.presentQueue);
    
    s_state.logical_device = std::move(ld);

    return EngineError::Ok;
}
static EngineError querySwapChainSupport(const VkPhysicalDevice& device, SwapChainSupportDetails& out_details){
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, s_state.surface.value(), &out_details.capabilities);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, s_state.surface.value(), &formatCount, nullptr);

    if (formatCount != 0) {
        out_details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, s_state.surface.value(), &formatCount, out_details.formats.data());
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, s_state.surface.value(), &presentModeCount, nullptr);

    if (presentModeCount != 0) {
        out_details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, s_state.surface.value(), &presentModeCount, out_details.presentModes.data());
    }

    return EngineError::Ok;
}
static VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
    for (const auto& availableFormat : availableFormats) {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return availableFormat;
        }
    }

    return availableFormats[0];
}
static VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
    for (const auto& availablePresentMode : availablePresentModes) {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return availablePresentMode;
        }
    }
    return VK_PRESENT_MODE_FIFO_KHR;
}
static VkExtent2D chooseSwapExtent(GLFWwindow* window, const VkSurfaceCapabilitiesKHR& capabilities) {
    if (capabilities.currentExtent.width != UINT32_MAX) {
        return capabilities.currentExtent;
    } else {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        VkExtent2D actualExtent = {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};

        actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
        actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

        return actualExtent;
    }
}
EngineError VulkanHandler::setupPhysicalDevices(const ContextInitData& context_data){
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(s_state.instance.value(), &deviceCount, nullptr);
    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(s_state.instance.value(), &deviceCount, devices.data());

    if(!devices.size()){
        return EngineError::VK_NoPhysicalDeviceFound;
    }

    for(size_t i = 0; i < devices.size(); ++i){
        const VkPhysicalDevice& device = devices[i];
        if(device != VK_NULL_HANDLE){
            QueueFamilyIndices queue_family;
            VkPhysicalDeviceProperties properties; 
            VkPhysicalDeviceFeatures features;
            const double score = isPhysicalDeviceSuitable(context_data, device, s_state.surface.value(), queue_family, properties, features);
            if(score != std::numeric_limits<double>::min()){
                s_state.physical_devices.emplace(device, score, queue_family, properties, features);
            }
        }
    }

    return s_state.physical_devices.size() ? EngineError::Ok : EngineError::VK_NoSuitablePhysicalDeviceFound;
}
static const EngineError findQueueFamilies(const VkPhysicalDevice& device, const VkSurfaceKHR& surface, QueueFamilyIndices& out_indices){
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data()); 

    int i = 0;
    for (const auto& queueFamily : queueFamilies) {
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            out_indices.graphicsFamily = i;
        }

        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

        if (presentSupport) {
            out_indices.presentFamily = i;
        }

        if(out_indices.isCompleted()){
            break;
        }

        i++;
    }

    return out_indices.isCompleted() ? EngineError::Ok : EngineError::VK_NoSuitableQueueFamilyFound;
}
static double isPhysicalDeviceSuitable(const ContextInitData& context_data, const VkPhysicalDevice& device, const VkSurfaceKHR& surface, QueueFamilyIndices& out_queueFamily, VkPhysicalDeviceProperties& out_p, VkPhysicalDeviceFeatures& out_f){
    static const double invalid_return_value = std::numeric_limits<double>::min();
    double score = 0.0;

    vkGetPhysicalDeviceProperties(device, &out_p);

    vkGetPhysicalDeviceFeatures(device, &out_f);  

    auto findQueueFamilyError = findQueueFamilies(device, surface, out_queueFamily);

    if(findQueueFamilyError != EngineError::Ok){
        return invalid_return_value;
    }
    if(!out_f.geometryShader){
        return invalid_return_value;
    }
    if(!isDeviceExtensionSupported(context_data, device)){
        return invalid_return_value;
    }

    SwapChainSupportDetails swapChainSupport;
    auto querySwapChainError = querySwapChainSupport(device, swapChainSupport);
    if(swapChainSupport.formats.empty() || swapChainSupport.presentModes.empty() || querySwapChainError != EngineError::Ok){
        return invalid_return_value;
    }


    // Discrete GPUs have a significant performance advantage
    if (out_p.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
        score += 3000.0;
    }

    // Maximum possible size of textures affects graphics quality
    score += out_p.limits.maxImageDimension2D;

    return score;
}
static const bool isDeviceExtensionSupported(const ContextInitData& context_data, const VkPhysicalDevice& device){
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

    std::set<std::string> requiredExtensions(context_data.vulkan_logical_device_extensions.begin(), context_data.vulkan_logical_device_extensions.end());

    for (const auto& extension : availableExtensions) {
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty(); 
}
EngineError VulkanHandler::setupDebugMessengers(const ContextInitData& context_data){
    if(context_data.vulkan_use_layers){
        VkDebugUtilsMessengerCreateInfoEXT messengerCreateInfo{};
        populateDebugMessengerCreateInfo(messengerCreateInfo);

        if(!s_state.extensions_available.count("vkCreateDebugUtilsMessengerEXT")){
            return EngineError::VK_DebugMessengerExtensionsNotFound;
        }
        auto address = (PFN_vkCreateDebugUtilsMessengerEXT)s_state.extensions_available["vkCreateDebugUtilsMessengerEXT"];

        VkDebugUtilsMessengerEXT messenger;
        auto setVkDebugCallbackError = static_cast<EngineError>(address(s_state.instance.value(), &messengerCreateInfo, nullptr, &messenger));
        if(setVkDebugCallbackError != EngineError::Ok){
            return setVkDebugCallbackError;
        }

        s_state.debugMessengers.push_back(std::move(messenger));
    }
    return EngineError::Ok;
}
EngineError VulkanHandler::createInstance(const ContextInitData& context_data, const std::vector<const char*>& extensions){
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Hello Triangle";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_2;
    appInfo.pNext = nullptr;

    VkDebugUtilsMessengerCreateInfoEXT instanceCreateDebugMessenger{};
    populateDebugMessengerCreateInfo(instanceCreateDebugMessenger);
    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    s_logger.console.log("Enabled exts...", Console::getHighestPriorityClassification());
    for(size_t i = 0; i < extensions.size(); ++i){
        s_logger.console.log(extensions[i], Console::getHighestPriorityClassification());
    }
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();
    createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &instanceCreateDebugMessenger;

    if(context_data.vulkan_use_layers){
        createInfo.enabledLayerCount = static_cast<uint32_t>(context_data.vulkan_layers.size());
        createInfo.ppEnabledLayerNames = context_data.vulkan_layers.data();
        
        addExtension("vkCreateDebugUtilsMessengerEXT");
        addExtension("vkDestroyDebugUtilsMessengerEXT");
    }else{
        createInfo.enabledLayerCount = 0;
        createInfo.pNext = nullptr;
    }

    VkInstance instance;
    VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);
    if(result != VkResult::VK_SUCCESS){
        return static_cast<EngineError>(result);
    }

    s_state.instance = instance;

    return EngineError::Ok;
}
EngineError VulkanHandler::checkPreInit(const ContextInitData& context_data, std::vector<const char*>& out_extensions){
    const EngineError vulkan_result = checkVulkanExtensionsValidity((context_data.vulkan_use_layers ? context_data.vulkan_layers : std::vector<const char*>()), s_availableExtensions);
    if(vulkan_result !=EngineError::Ok){
        return vulkan_result;
    }

    out_extensions.resize(s_availableExtensions.size());
    for(size_t i = 0; i < s_availableExtensions.size(); ++i){
        const VkExtensionProperties& prop = s_availableExtensions[i];
        out_extensions[i] = prop.extensionName;
    } 

    return EngineError::Ok;
}
static EngineError getAndClearGlfwError(){
    const char** s = nullptr;
    return static_cast<EngineError>(glfwGetError(s));
}
static EngineError saveAvailableExtensions(const std::set<std::string>& extensions){
    for(const auto& name : extensions){       
        if(s_state.extensions_available.count(name)){
            continue;
        }

        auto address = vkGetInstanceProcAddr(s_state.instance.value(), name.c_str());
        const char ** s = nullptr;
        if(address == nullptr){
            s_logger.console.log("Failed to retrieve vulkan extension: " + name + std::string(" With error: ") + std::to_string(glfwGetError(s)), Console::getHighestPriorityClassification());
            continue;
        }

        s_state.extensions_available[name] = address;
    }

    return EngineError::Ok; //TODO: check for errors
}
static EngineError checkVulkanExtensionsValidity(const std::vector<const char*>& requested_layers, std::vector<VkExtensionProperties>& out_extensions){
    bool vulkan_supported = glfwVulkanSupported();
    auto error = getAndClearGlfwError();
    if(!vulkan_supported){
        return EngineError::ApiUnavailable;
    }

    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    out_extensions.resize(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, out_extensions.data());
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    if(out_extensions.size() < glfwExtensionCount){
        return EngineError::VK_ErrorExtensionNotPresent;
    }
    for(uint32_t i = 0; i < glfwExtensionCount; ++i){
        bool found = false;
        const char* required_extension = glfwExtensions[i];
        for(uint32_t j = 0; j < extensionCount; ++j){
            if(!std::strcmp(required_extension, out_extensions[j].extensionName)){
                found = true;
                break;
            }
        }
        if(!found){
            return EngineError::VK_ErrorExtensionNotPresent;
        }
    }

    return checkVulkanLayersValidity(requested_layers);
}
static EngineError checkVulkanLayersValidity(const std::vector<const char*>& requested_layers){
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    if(availableLayers.size() < requested_layers.size()){
        return EngineError::VK_ErrorLayerNotPresent;
    }
    for(uint32_t i = 0; i < requested_layers.size(); ++i){
        bool found = false;
        const char* required_extension = requested_layers[i];
        for(uint32_t j = 0; j < availableLayers.size(); ++j){
            if(!std::strcmp(required_extension, availableLayers[j].layerName)){
                found = true;
                break;
            }
        }
        if(!found){
            return EngineError::VK_ErrorLayerNotPresent;
        }
    }

    return EngineError::Ok;
}
static VKAPI_ATTR VkBool32 VKAPI_CALL vulkanDebugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData){
    
    //TODO: print a representation of other arguments too
    s_logger.console.log("Vulkan validation layer: " + std::string(pCallbackData->pMessage), Console::getHighestPriorityClassification());

    return VK_FALSE;
}
static void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo){
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = vulkanDebugCallback;
    createInfo.pUserData = nullptr;
}
static EngineError createShaderModule(const std::vector<char>& code, VkShaderModule& out_module) {
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

    if (vkCreateShaderModule(s_state.logical_device.value().device, &createInfo, nullptr, &out_module) != VK_SUCCESS) {
        return EngineError::VK_FailedCreatingShaderModule;
    }

    return EngineError::Ok;
}
static uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(s_state.physical_devices.cbegin()->device, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    throw std::runtime_error("failed to find suitable memory type!");
}
static EngineError copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {
    VkCommandBuffer commandBuffer = beginSingleTimeCommands();

    VkBufferCopy copyRegion{};
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

    endSingleTimeCommands(commandBuffer);

    return EngineError::Ok;
}
void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout) {
    VkCommandBuffer commandBuffer = beginSingleTimeCommands();

    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

    barrier.image = image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    barrier.srcAccessMask = 0; 
    barrier.dstAccessMask = 0; 

    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destinationStage;

    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    } else {
        throw std::invalid_argument("unsupported layout transition!");
    }

    vkCmdPipelineBarrier(
        commandBuffer,
        sourceStage, destinationStage,
        0,
        0, nullptr,
        0, nullptr,
        1, &barrier
    );

    endSingleTimeCommands(commandBuffer);
}
static void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height) {
    VkCommandBuffer commandBuffer = beginSingleTimeCommands();

    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;

    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;

    region.imageOffset = {0, 0, 0};
    region.imageExtent = {
        width,
        height,
        1
    };

    vkCmdCopyBufferToImage(
        commandBuffer,
        buffer,
        image,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        1,
        &region
    );

    endSingleTimeCommands(commandBuffer);
}