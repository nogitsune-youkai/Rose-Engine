#include "VulkanBackend.h"

void VulkanBackend::initVulkan()
{
	createInstance();
}

void VulkanBackend::cleanUp()
{
	// vulkan instance must be destroyed right before program exits, all other vulkan resources must be destroyed before the instance destroyed
	vkDestroyInstance(vulkanInstance, nullptr);
}

void VulkanBackend::createInstance()
{
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

	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;

	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
	createInfo.enabledExtensionCount = glfwExtensionCount;
	createInfo.ppEnabledExtensionNames = glfwExtensions;
	createInfo.enabledLayerCount = 0;

	VkResult result = vkCreateInstance(&createInfo, nullptr, &vulkanInstance);

	if (vkCreateInstance(&createInfo, nullptr, &vulkanInstance) != VK_SUCCESS) {
		throw std::runtime_error("failed to create instance!");
	}
}
