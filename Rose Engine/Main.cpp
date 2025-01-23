#include "Engine/Core/GraphicsAPI/Vulkan/VulkanBackend.h"

int main() {
    VulkanBackend vulkanRenderer;
    vulkanRenderer.createVulkanWindow();
    vulkanRenderer.initVulkan();
    vulkanRenderer.vulkanRenderMainLoop();
    vulkanRenderer.cleanUp();
}