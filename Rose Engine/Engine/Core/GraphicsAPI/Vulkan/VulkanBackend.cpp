#include "VulkanBackend.h"


const std::vector<const char*> validationLayers = {
	"VK_LAYER_KHRONOS_validation"
};

const std::vector<const char*> deviceExtensions = {
	// required device extensions
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

// only enable validation layers in debug mode
#ifdef NDEBUG
constexpr bool enableValidationLayers = false;
#else
constexpr bool enableValidationLayers = true;
#endif


void VulkanBackend::initVulkan()
{
	createInstance();
	setupDebugMessenger();
	createSurface();
	pickPhysicalDevice();
	createLogicalDevice();
	createSwapChain();
}

void VulkanBackend::vulkanRenderMainLoop()
{
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

	}
}

void VulkanBackend::cleanUp()
{
	// vulkan instance must be destroyed right before program exits, all other vulkan resources must be destroyed before the instance destroyed
	vkDestroySwapchainKHR(logicalDevice, swapChain, nullptr);
	vkDestroyDevice(logicalDevice, nullptr);
	if (enableValidationLayers) {
		destroyDebugUtilsMessengerEXT(vulkanInstance, debugMessenger, nullptr);
	}
	vkDestroySurfaceKHR(vulkanInstance, vulkanWindowSurface, nullptr);
	vkDestroyInstance(vulkanInstance, nullptr);
	glfwDestroyWindow(window);
	glfwTerminate();
}

int VulkanBackend::createVulkanWindow()
{
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	

	
	
	window = glfwCreateWindow(1280, 720, "Vulkan window", nullptr, nullptr);
	
	

	 if (!glfwVulkanSupported())
	 {
		 std::exception GLFWWindowError("GLFW: Vulkan Not Supported\n");
		 std::cout << GLFWWindowError.what() << std::endl;
		 return 1;
	 }

	 

	 

	  
	 return 0;
}

void VulkanBackend::createInstance()
{
	if (enableValidationLayers && !checkValidationLayerSupport()) {
		std::runtime_error validationLayersError("validation layers requested, but not available!");
		std::cout << validationLayersError.what() << std::endl;
	}
	
		
	
	// initialize vulkan library by creating it's instance
	VkApplicationInfo appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO; // sType stands for structure type
	appInfo.pNext = nullptr; // pNext is a pointer which is pointing to extension information, that will be useful later
	appInfo.pApplicationName = "Counter Fuck: blood and gore";
	appInfo.applicationVersion = VK_MAKE_API_VERSION(0, 1, 3, 0);
	appInfo.pEngineName = "Rose Engine";
	appInfo.engineVersion = VK_MAKE_API_VERSION(0, 1, 3, 0);
	appInfo.apiVersion = VK_API_VERSION_1_4;

	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;
	

	

	auto extensions = getRequiredExtensions(); // hmm, for some reason i enable KHR_surface here, but i should do it after instance creation
												// because it can actually influence physical device selection
	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data(); // ppEnabledExtensionsName are used to enable extensions for a given type, VkDevice for example, etc
	
	// so basically by setting up debug messenger here using populate populateDebugMessengerCreateInfo 
	// it will automatically be used for every vkCreateInstance and vkDestroyInstance calls, so we can now track what happens 
	// with every instance in our game
	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {};
	if (enableValidationLayers) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();

		populateDebugMessengerCreateInfo(debugCreateInfo);
		createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;
	}
	else {
		createInfo.enabledLayerCount = 0;
		createInfo.pNext = nullptr;
	}

	if (vkCreateInstance(&createInfo, nullptr, &vulkanInstance) != VK_SUCCESS) {
		std::runtime_error vulkanInstanceError("failed to create instance!");
		std::cout << vulkanInstanceError.what() << std::endl;
	}
}

void VulkanBackend::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& debugUtilsMessengerCreateInfo)
{
	// so i guess this is gonna function like a logger with a wide wariety of information, like vulkan warnings, errors and all other crap, 
	// we can manually specify what exactly we want to be logged using messageSeverity and messageType fields
	debugUtilsMessengerCreateInfo = {};
	debugUtilsMessengerCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	debugUtilsMessengerCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	debugUtilsMessengerCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	debugUtilsMessengerCreateInfo.pfnUserCallback = debugCallback; // our own debug callback
	
}

void VulkanBackend::setupDebugMessenger()
{
	if (!enableValidationLayers) {
		return;
	}
	VkDebugUtilsMessengerCreateInfoEXT createInfo{};
	populateDebugMessengerCreateInfo(createInfo);


	if (createDebugUtilsMessengerEXT(vulkanInstance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
		std::runtime_error debugMessengerError("failed to set up debug messenger!");
		std::cout << debugMessengerError.what() << std::endl;
	}
}

void VulkanBackend::createSurface()
{

	VkResult err = glfwCreateWindowSurface(vulkanInstance, window, nullptr, &vulkanWindowSurface);
	if (err != VK_SUCCESS) {
		std::runtime_error createSurfaceError("failed to create window surface!");
		std::cout <<  createSurfaceError.what() << std::endl;
	}
}

void VulkanBackend::pickPhysicalDevice()
{
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(vulkanInstance, &deviceCount, nullptr);

	if (deviceCount == 0) {
		std::runtime_error deviceCountError("failed to find GPUs with Vulkan support!");
		std::cout << deviceCountError.what() << std::endl;
	}
	std::vector<VkPhysicalDevice> physicalGPUs(deviceCount);
	vkEnumeratePhysicalDevices(vulkanInstance, &deviceCount, physicalGPUs.data());

	for (const auto& GPU : physicalGPUs) {
		if (isDeviceSuitable(GPU)) {
			physicalDevice = GPU;
			break;
		}
	}
	if (physicalDevice == VK_NULL_HANDLE) {
		std::runtime_error physicalDeviceError("failed to find a suitable GPU!");
		std::cout << physicalDeviceError.what() << std::endl;
	}
}

void VulkanBackend::createLogicalDevice()
{
	QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfoStructures;
	std::set<uint32_t> uniqueQueueFamilies = { 
		indices.graphicsFamily.value(), indices.presentFamily.value() 
	};
	float queuePriority = 1.0f;
	for (uint32_t queueFamily : uniqueQueueFamilies) {
		VkDeviceQueueCreateInfo queueCreateInfo{};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfoStructures.push_back(queueCreateInfo);
	}
	

	VkPhysicalDeviceFeatures logicalDeviceFeatures{};

	VkDeviceCreateInfo logicalDeviceCreateInfo{};
	logicalDeviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	logicalDeviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfoStructures.size());
	logicalDeviceCreateInfo.pQueueCreateInfos = queueCreateInfoStructures.data();

	logicalDeviceCreateInfo.pEnabledFeatures = &logicalDeviceFeatures;
	logicalDeviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size()); // enable swapchain extension for gpu
	logicalDeviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data(); // enable required extensions

	if (vkCreateDevice(physicalDevice, &logicalDeviceCreateInfo, nullptr, &logicalDevice) != VK_SUCCESS) {
		std::runtime_error logicalDeviceError("Failed to create logical device");
		std::cout << logicalDeviceError.what() << std::endl;
	}
	vkGetDeviceQueue(logicalDevice, indices.graphicsFamily.value(), 0, &graphicsQueue);
	vkGetDeviceQueue(logicalDevice, indices.presentFamily.value(), 0, &presentationQueue);
}

void VulkanBackend::createSwapChain()
{
	SwapChainSupportDetails swapChainSupport = querySwapChainSupportDetails(physicalDevice);
	VkSurfaceFormatKHR surfaceFormat = chooseSwapChainSurfaceFormat(swapChainSupport.surfaceFormats);
	VkPresentModeKHR presentMode = choosePresentationMode(swapChainSupport.presentationModes);
	VkExtent2D imageExtent = chooseSwapExtent(swapChainSupport.surfaceCapabilities);

	uint32_t imageCount = swapChainSupport.surfaceCapabilities.minImageCount + 1; // minimum number of images in swapChain for it to function
	//if for some reason maximum number of images in swapchain is unlimited - limit it by maxImageCount
	if (swapChainSupport.surfaceCapabilities.maxImageCount > 0 && imageCount > swapChainSupport.surfaceCapabilities.maxImageCount) {
		imageCount = swapChainSupport.surfaceCapabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR swapChainCreateInfo{};
	swapChainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapChainCreateInfo.surface = vulkanWindowSurface;

	swapChainCreateInfo.minImageCount = imageCount;
	swapChainCreateInfo.imageFormat = surfaceFormat.format;
	swapChainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
	swapChainCreateInfo.imageExtent = imageExtent;
	swapChainCreateInfo.imageArrayLayers = 1;
	swapChainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT; // bit field specifies what kind of operations we’ll use the images in the swap chain for

	QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
	uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };
	if (indices.graphicsFamily != indices.presentFamily) {
		swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		swapChainCreateInfo.queueFamilyIndexCount = 2;
		swapChainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else {
		swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		swapChainCreateInfo.queueFamilyIndexCount = 0; // Optional
		swapChainCreateInfo.pQueueFamilyIndices = nullptr; // Optional
	}
	swapChainCreateInfo.preTransform = swapChainSupport.surfaceCapabilities.currentTransform;
	swapChainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR; // ignore alpha channel in image
	swapChainCreateInfo.presentMode = presentMode;
	swapChainCreateInfo.clipped = VK_TRUE;
	swapChainCreateInfo.oldSwapchain = VK_NULL_HANDLE; // With Vulkan it’s possible that your swap chain becomes invalid or unoptimized while your application is running, for example because the window was resized. 
													   // In that case the swap chain actually needs to be recreated from scratch and a reference to the old one must be specified in this field.
	if (vkCreateSwapchainKHR(logicalDevice, &swapChainCreateInfo, nullptr, &swapChain) != VK_SUCCESS) {
		std::runtime_error swapChainError("failed to create swap chain!");
		std::cout << swapChainError.what() << std::endl;
	}
}
bool VulkanBackend::isDeviceSuitable(VkPhysicalDevice GPU)
{
	physicalDevice = GPU;
	// get a suitable physical GPU, in this function we can add more checks for GPUs
	// and output various GPU information for debug purposes
	VkPhysicalDeviceProperties gpuProperties;
	vkGetPhysicalDeviceProperties(GPU, &gpuProperties);
	//gpuProperties.driverVersion = static_cast<char*>(gpuDriverInfo.driverName);
	if (enableValidationLayers) {
		std::cerr << "Selected device: " << gpuProperties.deviceName << std::endl;
		std::cout << "NVIDIA Driver version : " << gpuProperties.driverVersion << std::endl; // i should test it for nvidia GPU
		gpuInfo.InitializeAGSLib(); // print gpu information for debug
	}

	//VkPhysicalDeviceFeatures gpuFeatures;
	
	// does GPU support commands that we need?
	QueueFamilyIndices indices = findQueueFamilies(GPU);
	bool extensionsSupported = checkDeviceExtensionSupport(GPU);

	bool swapChainAdequate = false;
	if (extensionsSupported) {
		SwapChainSupportDetails swapChainSupportDetails = querySwapChainSupportDetails(GPU);
		// basically gpu must support at least one surface format and one presentation mode i guess
		swapChainAdequate = !swapChainSupportDetails.surfaceFormats.empty() && swapChainSupportDetails.presentationModes.empty();
	}
	return indices.isComplete() && extensionsSupported && swapChainAdequate;
}

bool VulkanBackend::checkDeviceExtensionSupport(VkPhysicalDevice GPU)
{
	physicalDevice = GPU;
	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(GPU, nullptr, &extensionCount, nullptr);

	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(GPU, nullptr, &extensionCount, availableExtensions.data());
	
	std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

	for (const auto& extension : availableExtensions) {
		requiredExtensions.erase(extension.extensionName);
	}

	return requiredExtensions.empty();
}

QueueFamilyIndices VulkanBackend::findQueueFamilies(VkPhysicalDevice GPU)
{
	physicalDevice = GPU;
	//Queues are essentially simply accept different commands supported by GPUs and as far as i can see
	// all the gpus support 4 most common flags which are - VK_QUEUE_GRAPHICS_BIT, VK_QUEUE_COMPUTE_BIT
	// VK_QUEUE_TRANSFER_BIT and VK_QUEUE_SPARSE_BINDING_BIT
	
	QueueFamilyIndices indices;
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(GPU, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(GPU, &queueFamilyCount, queueFamilies.data());

	int i = 0;
	for (const auto& queueFamily : queueFamilies) {
		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			indices.graphicsFamily = i;
		}
		VkBool32 presentationSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(GPU, i, vulkanWindowSurface, &presentationSupport);

		if (presentationSupport) {
			indices.presentFamily = i;
		}

		if (indices.isComplete()) {
			break;
		}
		i++;
	}
	return indices;

	
}

SwapChainSupportDetails VulkanBackend::querySwapChainSupportDetails(VkPhysicalDevice GPU) {
	physicalDevice = GPU;
	SwapChainSupportDetails swapChainDetails;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(GPU, vulkanWindowSurface, &swapChainDetails.surfaceCapabilities);

	// get supported surface format
	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(GPU, vulkanWindowSurface, &formatCount, nullptr);

	if (formatCount != 0) {
		swapChainDetails.surfaceFormats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(GPU, vulkanWindowSurface, &formatCount, swapChainDetails.surfaceFormats.data());
	}

	// get supported presentation modes
	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(GPU, vulkanWindowSurface, &presentModeCount, nullptr);


	if (presentModeCount != 0) {
		swapChainDetails.presentationModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(GPU, vulkanWindowSurface, &presentModeCount, swapChainDetails.presentationModes.data());
	}
	return swapChainDetails;
}

VkResult VulkanBackend::createDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
{
	// The vkGetInstanceProcAddr function will return nullptr if the function couldn’t be loaded.
	auto function = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(vulkanInstance, "vkCreateDebugUtilsMessengerEXT");

	if (function != nullptr) {
		return function(instance, pCreateInfo, pAllocator, pDebugMessenger);

	}
	else {
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

void VulkanBackend::destroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT pDebugMessenger, const VkAllocationCallbacks* pAllocator)
{
	// used to destroy the debug messenger
	auto function = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (function != nullptr) {
		function(instance, debugMessenger, pAllocator);
	}
}

bool VulkanBackend::checkValidationLayerSupport()
{
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
	
	for (const char* layerName : validationLayers) {
		bool layerFound = false;

		for (const auto& layerProperties : availableLayers) {
			if (strcmp(layerName, layerProperties.layerName) == 0) {
				layerFound = true;
				break;
			}
		}
		if (!layerFound) {
			return false;
		}
	}
	return true;
}

std::vector<const char*> VulkanBackend::getRequiredExtensions()
{
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	if (enableValidationLayers) {
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}
	return extensions;
}

VkSurfaceFormatKHR VulkanBackend::chooseSwapChainSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
	for (const auto& availableFormat : availableFormats) {
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
			return availableFormat;
		}
	}
	return availableFormats[0];
}

VkPresentModeKHR VulkanBackend::choosePresentationMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
{
	// check if VK_PRESENT_MODE_MAILBOX_KHR supported, which is tripple buffering mode, unlocked FPS
	for (const auto& availablePresentMode : availablePresentModes) {
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
			return availablePresentMode;
		}
	}
	return VK_PRESENT_MODE_FIFO_KHR; // if not, use basically vSync
}

VkExtent2D VulkanBackend::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
{
	// swap extent is a resolution of images in swapchain
	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
		return capabilities.currentExtent;
	}
	else {
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);

		VkExtent2D actualExtent = {
			static_cast<uint32_t>(width),
			static_cast<uint32_t>(height)
		};

		actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
		actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

		return actualExtent;
	}
}

VKAPI_ATTR VkBool32 VKAPI_CALL VulkanBackend::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
{
	// this is for debugging, probably gonna use it in logger
	std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
	

	return VK_FALSE;
}
