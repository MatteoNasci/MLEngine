#include <Rendering/Core/vulkanhandler.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <Engine/Debug/console.h>
#include <Engine/Time/timermanager.h>

#include <unordered_map>
#include <cstring>
#include <set>
#include <limits>
#include <optional>
#include <algorithm>



using namespace mle;

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
    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;

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
static std::set<std::string> s_extensions;
static std::vector<VkExtensionProperties> s_availableExtensions;
static Logger s_logger;
static VulkanData s_state;

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
static VkExtent2D chooseSwapExtent(const uint32_t width, const uint32_t height, const VkSurfaceCapabilitiesKHR& capabilities);

//temp
static void waitForLogging(){
    while(s_logger.console.isLoggingToFile()){
        VulkanHandler::advanceLoggerTime(1);
    }
}
static void waitForLoggingAndLog(const std::string& log){
    s_logger.console.log(log, Console::getHighestPriorityClassification());
    waitForLogging();
}


void VulkanHandler::advanceLoggerTime(const double timeToAdd){
    s_logger.timerManager.advanceTime(timeToAdd);
}
EngineError VulkanHandler::addExtension(const std::string& ext){
    EngineError result = EngineError::Ok;
    if(!s_extensions.count(ext)){
        s_extensions.insert(ext);

        if(s_state.instance.has_value()){
            result = saveAvailableExtensions(s_extensions);
            s_extensions.clear();
        }
    }

    return result;
}
bool VulkanHandler::isExtensionLoaded(const std::string& ext){
    return s_state.extensions_available.count(ext);
}
EngineError VulkanHandler::initialize(const ContextInitData& context_data, const uint32_t width, const uint32_t height){
    waitForLoggingAndLog("first");

    std::vector<const char*> extensions;
    auto preInitError = checkPreInit(context_data, extensions);
    if(preInitError != EngineError::Ok){
        return preInitError;
    }
    waitForLoggingAndLog("second");

    auto createInstanceError = createInstance(context_data, extensions);
    if(createInstanceError != EngineError::Ok){
        waitForLoggingAndLog("Failed third with " + std::to_string(static_cast<int>(createInstanceError)));
        return createInstanceError;
    }
    waitForLoggingAndLog("third");

    auto savingAdressedError = saveAvailableExtensions(s_extensions);
    s_extensions.clear();

    waitForLoggingAndLog("fourth");

    auto createSurfaceError = createSurface(context_data);
waitForLoggingAndLog("fifth");
    
    auto settingDebugMessengersError = setupDebugMessengers(context_data);
waitForLoggingAndLog("sixth");

    auto setupPhysicalDevicesError = setupPhysicalDevices(context_data);
    if(setupPhysicalDevicesError != EngineError::Ok){
        waitForLoggingAndLog("Failed seventh with " + std::to_string(static_cast<int>(setupPhysicalDevicesError)));
        return setupPhysicalDevicesError;
    }
waitForLoggingAndLog("seventh");

    auto setupLogicalDevicesError = setupLogicalDevices(context_data);
waitForLoggingAndLog("eighth");

    auto createSwapChainerror = createSwapChain(width, height);
waitForLoggingAndLog("ninth");
    
    waitForLoggingAndLog("Successfull Vulkan init! Created");
    return EngineError::Ok;
}
EngineError VulkanHandler::release(){ 
    if(s_state.logical_device.has_value()){  
        if(s_state.swap_chain.has_value()){
            vkDestroySwapchainKHR(s_state.logical_device.value().device, s_state.swap_chain.value(), nullptr);
        }
        s_state.swap_chain.reset();

        vkDestroyDevice(s_state.logical_device.value().device, nullptr);
    }
    s_state.logical_device.reset();

    if(s_state.instance.has_value()){
        if(s_state.extensions_available.count("vkDestroyDebugUtilsMessengerEXT")){
            auto destroyer = (PFN_vkDestroyDebugUtilsMessengerEXT)s_state.extensions_available["vkDestroyDebugUtilsMessengerEXT"];
            for(size_t i = 0; i < s_state.debugMessengers.size(); ++i){
                const auto& element = s_state.debugMessengers[i];
                destroyer(s_state.instance.value(), element, nullptr);
            }
        }else{
            waitForLoggingAndLog("Vulkan could not find the required extension to use to delete created debug messengers!");
        }
        s_state.debugMessengers.clear();

        if(s_state.surface.has_value()){
            vkDestroySurfaceKHR(s_state.instance.value(), s_state.surface.value(), nullptr);
        }
        s_state.surface.reset();

        vkDestroyInstance(s_state.instance.value(), nullptr);
    }
    s_state.instance.reset();

    waitForLogging();

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
EngineError VulkanHandler::createSwapChain(const uint32_t width, const uint32_t height){
    const auto& physicalDevice = *(s_state.physical_devices.cbegin());
    SwapChainSupportDetails swapChainSupport;
    auto chainSupportError = querySwapChainSupport(physicalDevice.device, swapChainSupport);

    VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
    VkExtent2D extent = chooseSwapExtent(width, height, swapChainSupport.capabilities);

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

    waitForLoggingAndLog(context_data.vulkan_logical_device_extensions[0]);
    waitForLoggingAndLog(std::to_string(context_data.vulkan_logical_device_extensions.size()));
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
static VkExtent2D chooseSwapExtent(const uint32_t width, const uint32_t height, const VkSurfaceCapabilitiesKHR& capabilities) {
    if (capabilities.currentExtent.width != UINT32_MAX) {
        return capabilities.currentExtent;
    } else {
        VkExtent2D actualExtent = {width, height};

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
        waitForLoggingAndLog("Failed find family queue");
        return invalid_return_value;
    }
    if(!out_f.geometryShader){
        waitForLoggingAndLog("Failed geometry shader");
        return invalid_return_value;
    }
    if(!isDeviceExtensionSupported(context_data, device)){
        waitForLoggingAndLog("Failed extension supported");
        return invalid_return_value;
    }

    SwapChainSupportDetails swapChainSupport;
    auto querySwapChainError = querySwapChainSupport(device, swapChainSupport);
    if(swapChainSupport.formats.empty() || swapChainSupport.presentModes.empty() || querySwapChainError != EngineError::Ok){
        waitForLoggingAndLog("Failed swapchain support");
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
    waitForLoggingAndLog("Enabled exts...");
    for(size_t i = 0; i < extensions.size(); ++i){
        waitForLoggingAndLog(extensions[i]);
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

    waitForLoggingAndLog("Successfull Vulkan init!");
    return EngineError::Ok;
}
EngineError VulkanHandler::checkPreInit(const ContextInitData& context_data, std::vector<const char*>& out_extensions){
    const EngineError vulkan_result = checkVulkanExtensionsValidity((context_data.vulkan_use_layers ? context_data.vulkan_layers : std::vector<const char*>()), s_availableExtensions);
    waitForLoggingAndLog("Checking for Vulkan with result code: " + std::to_string(static_cast<int>(vulkan_result)));
    if(vulkan_result !=EngineError::Ok){
        return vulkan_result;
    }

    waitForLoggingAndLog("Initializing Vulkan with extensions:");

    out_extensions.resize(s_availableExtensions.size());
    for(size_t i = 0; i < s_availableExtensions.size(); ++i){
        const VkExtensionProperties& prop = s_availableExtensions[i];
        out_extensions[i] = prop.extensionName;
        waitForLoggingAndLog(prop.extensionName + std::string(", version: ") + std::to_string(prop.specVersion));
    } 

    return EngineError::Ok;
}
static EngineError getAndClearGlfwError(){
    const char** s = nullptr;
    return static_cast<EngineError>(glfwGetError(s));
}
static EngineError saveAvailableExtensions(const std::set<std::string>& extensions){
    waitForLoggingAndLog("saving: " + extensions.size());
    for(const auto& name : extensions){       
        if(s_state.extensions_available.count(name)){
            continue;
        }

        auto address = vkGetInstanceProcAddr(s_state.instance.value(), name.c_str());
        const char ** s = nullptr;
        if(address == nullptr){
            waitForLoggingAndLog("Failed to retrieve vulkan extension: " + name + std::string(" With error: ") + std::to_string(glfwGetError(s)));
            continue;
        }

        waitForLoggingAndLog("Saving func: " + name);
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
    waitForLoggingAndLog("Vulkan validation layer: " + std::string(pCallbackData->pMessage));

    return VK_FALSE;
}
static void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo){
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = /*VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | */VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = vulkanDebugCallback;
    createInfo.pUserData = nullptr;
}