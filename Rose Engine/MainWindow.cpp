#include "ThirdParty/API/imgui-1.91.7/imgui.h"
#include "ThirdParty/API/Vulkan/Include/vulkan/vulkan.h"
#include "ThirdParty/API/GLFW/glfw3.h"
#include <iostream>
#include "MainWindow.h"

int MainWindow::initializeWindow()
{
    
    
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    GLFWwindow* window = glfwCreateWindow(1280, 720, "Vulkan window", nullptr, nullptr);

    if (!glfwVulkanSupported())
    {
        printf("GLFW: Vulkan Not Supported\n");
        return 1;
    }

    vulkanBackend.initVulkan();

    ImGui::CreateContext();
    
    ImGui::StyleColorsDark();

    ImGuiIO& io = ImGui::GetIO(); (void)io;
    
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        


        bool show_another_window = true;
        //ImGui::NewFrame();
        
        
        //if (show_another_window)
        //{
            //ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
            //ImGui::Text("Hello from another window!");
            //if (ImGui::Button("Close Me"))
                //show_another_window = false;
            //ImGui::End();
       // }
        //ImGui::Render();
        //glfwSwapBuffers(window);
    }
    ImGui::DestroyContext();

    vulkanBackend.cleanUp(); // vulkan instance must be destroyed right before program exits
    glfwDestroyWindow(window);
    
    glfwTerminate();
    return 0;
}


