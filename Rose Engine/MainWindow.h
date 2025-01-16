#pragma once
#include "Engine/Core/GraphicsAPI/Vulkan/VulkanBackend.h"
class MainWindow
{
public:
	int initializeWindow();

private:
	VulkanBackend vulkanBackend;
	
};

