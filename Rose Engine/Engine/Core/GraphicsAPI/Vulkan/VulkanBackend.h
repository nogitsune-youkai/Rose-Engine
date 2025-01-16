#pragma once
#include <stdint.h>
#include <stdexcept>

#include "vulkan/vulkan.h"
#include "glfw3.h"
class VulkanBackend
{

public:
	void initVulkan();
	void cleanUp();

private:
	void createInstance();
	VkInstance vulkanInstance;
};

