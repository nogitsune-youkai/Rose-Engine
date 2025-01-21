#pragma once
#include <iostream>


#include "ThirdParty/API/imgui-1.91.7/imgui.h"
#include "ThirdParty/API/GLFW/glfw3.h"
#include "Engine/Core/GraphicsAPI/Vulkan/VulkanBackend.h"

class MainWindow
{
public:
	int initializeWindow();

private:
	VulkanBackend vulkanBackend;
	
};

