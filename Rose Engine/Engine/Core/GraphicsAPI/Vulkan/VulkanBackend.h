#pragma once

#include <stdint.h>
#include <stdexcept>
#include <vector>
#include <iostream>
#include <optional>


#include "vulkan/vulkan.h"
#include "glfw3.h"
#include "../GPU Information/GPUInformation.h"

struct QueueFamilyIndices {
	// here we gonna store Queue families
	std::optional<uint32_t> graphicsFamily;

	bool isComplete() {
		return graphicsFamily.has_value();
	}
};

class VulkanBackend
{

public:
	void initVulkan();
	void cleanUp();

private:

	void createInstance();
	void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& debugUtilsMessengerCreateInfo);
	void setupDebugMessenger();
	void pickPhysicalDevice();
	bool isDeviceSuitable(VkPhysicalDevice GPU);
	
	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice GPU);
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
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE; // GPU for rendering, it is destroyed implicitly when the vkInstance destroyed
	GPUInformation gpuInfo; // with this object we can get information about gpu such as: driverVersion, vendor, etc in readable format
};

