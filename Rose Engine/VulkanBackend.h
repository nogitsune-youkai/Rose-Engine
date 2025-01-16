#pragma once
#include <stdint.h>
#include <stdexcept>

#include "ThirdParty/API/Vulkan/Include/vulkan/vulkan.h"
#include "ThirdParty/API/GLFW/glfw3.h"
class VulkanBackend
{

public:
	void initVulkan();
	void cleanUp();

private:
	void createInstance();
	VkInstance vulkanInstance;
};

