#pragma once
#include <stdint.h>
#include <stdexcept>
#include <vector>
#include <iostream>

#include "vulkan/vulkan.h"
#include "glfw3.h"
class VulkanBackend
{

public:
	void initVulkan();
	void cleanUp();

private:
	void createInstance();
	void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& debugUtilsMessengerCreateInfo);
	void setupDebugMessenger();
	VkResult createDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, 
										  const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
	void destroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT pDebugMessenger, const VkAllocationCallbacks* pAllocator);
	bool checkValidationLayerSupport();
	std::vector<const char*> getRequiredExtensions();
	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
														VkDebugUtilsMessageTypeFlagsEXT messageType,
														const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
														void* pUserData);
	VkInstance vulkanInstance;
	VkDebugUtilsMessengerEXT debugMessenger;
};

