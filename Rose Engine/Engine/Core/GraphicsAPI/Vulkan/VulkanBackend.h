#pragma once

#define NOMINMAX // fucking windows.h redefinitions

#include <stdint.h>
#include <stdexcept>
#include <vector>
#include <iostream>
#include <print>
#include <optional>
#include <set>
#include <cstdint>
#include <algorithm> 
#include <limits>

#include "Engine/Core/Debug/GPU Information/GPUInformation.h"
#include "vulkan/vulkan.h"
#include "glfw3.h"
#include "ShaderCompiler.h"


struct QueueFamilyIndices {
	// here we gonna store Queue families
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily; // presentation queue

	bool isComplete() {
		return graphicsFamily.has_value() && presentFamily.has_value();
	}
};


struct SwapChainSupportDetails {
	// get swap chain support details for GPU
	VkSurfaceCapabilitiesKHR surfaceCapabilities;
	std::vector<VkSurfaceFormatKHR> surfaceFormats; // surface for is basically color depth
	std::vector<VkPresentModeKHR> presentationModes; // presentation mode is conditions for "swapping" images to screen
};

class VulkanBackend
{

public:
	int createVulkanWindow();
	void initVulkan();
	void vulkanRenderMainLoop();
	void cleanUp();

private:

	
	
	void createInstance();
	void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& debugUtilsMessengerCreateInfo);
	void setupDebugMessenger();
	void createSurface();
	void pickPhysicalDevice();
	void createLogicalDevice();
	void createSwapChain();
	void createImageViews();
	void createGraphicsPipeline();
	
	bool isDeviceSuitable(VkPhysicalDevice GPU);
	bool checkDeviceExtensionSupport(VkPhysicalDevice GPU);
	
	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice GPU);
	SwapChainSupportDetails querySwapChainSupportDetails(VkPhysicalDevice GPU);
	VkResult createDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, 
										  const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
	void destroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT pDebugMessenger, const VkAllocationCallbacks* pAllocator);
	bool checkValidationLayerSupport();
	std::vector<const char*> getRequiredExtensions();
	VkSurfaceFormatKHR chooseSwapChainSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR choosePresentationMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
														VkDebugUtilsMessageTypeFlagsEXT messageType,
														const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
														void* pUserData);
	VkShaderModule createShaderModule(const std::vector<char>& code);
	GLFWwindow* window;
	VkInstance vulkanInstance;
	VkDebugUtilsMessengerEXT debugMessenger;
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE; // GPU for rendering, it is destroyed implicitly when the vkInstance destroyed
	GPUInformation gpuInfo; // with this object we can get information about gpu such as: driverVersion, vendor, etc in readable format
	VkDevice logicalDevice;
	VkQueue graphicsQueue;
	VkSurfaceKHR vulkanWindowSurface;
	VkQueue presentationQueue;
	VkSwapchainKHR swapChain;
	std::vector<VkImage> swapChainImages;
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;
	std::vector<VkImageView> swapChainImageViews;
	ShaderCompiler shaderCompiler;
	

};

