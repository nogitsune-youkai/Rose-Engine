#include "VulkanBackend.h"


const std::vector<const char*> validationLayers = {
	"VK_LAYER_KHRONOS_validation"
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
	pickPhysicalDevice();
}

void VulkanBackend::cleanUp()
{
	// vulkan instance must be destroyed right before program exits, all other vulkan resources must be destroyed before the instance destroyed
	if (enableValidationLayers) {
		destroyDebugUtilsMessengerEXT(vulkanInstance, debugMessenger, nullptr);
	}
	vkDestroyInstance(vulkanInstance, nullptr);
}

void VulkanBackend::createInstance()
{
	if (enableValidationLayers && !checkValidationLayerSupport()) {
		throw std::runtime_error("validation layers requested, but not available!");
	}

	// initialize vulkan library by creating it's instance
	VkApplicationInfo appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO; // sType stands for structure type
	appInfo.pNext = nullptr; // pNext is a pointer which is pointing to extension information, that will be useful later
	appInfo.pApplicationName = "Counter Fuck: blood and gore";
	appInfo.applicationVersion = VK_MAKE_API_VERSION(0, 0, 1, 0);
	appInfo.pEngineName = "Rose Engine";
	appInfo.engineVersion = VK_MAKE_API_VERSION(0, 0, 1, 0);
	appInfo.apiVersion = VK_API_VERSION_1_3;

	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;
	

	

	auto extensions = getRequiredExtensions();
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
		throw std::runtime_error("failed to create instance!");
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
		throw std::runtime_error("failed to set up debug messenger!");
	}
}

void VulkanBackend::pickPhysicalDevice()
{
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(vulkanInstance, &deviceCount, nullptr);

	if (deviceCount == 0) {
		throw std::runtime_error("failed to find GPUs with Vulkan support!");
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
		throw std::runtime_error("failed to find a suitable GPU!");
	}
}

bool VulkanBackend::isDeviceSuitable(VkPhysicalDevice GPU)
{
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
	VkPhysicalDeviceFeatures gpuFeatures;
	
	// does GPU support commands that we need?
	QueueFamilyIndices indices = findQueueFamilies(GPU);
	return indices.isComplete();
}

QueueFamilyIndices VulkanBackend::findQueueFamilies(VkPhysicalDevice GPU)
{
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
		if (queueFamily.queueFlags && VK_QUEUE_GRAPHICS_BIT && VK_QUEUE_COMPUTE_BIT && VK_QUEUE_TRANSFER_BIT) {
			indices.graphicsFamily = i;
		}
		if (indices.isComplete()) {
			break;
		}
		i++;
	}
	return indices;
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

VKAPI_ATTR VkBool32 VKAPI_CALL VulkanBackend::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
{
	// this is for debugging, probably gonna use it in logger
	std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
	//std::cerr << "GPU information: "  <<  << std::endl;

	return VK_FALSE;
}
