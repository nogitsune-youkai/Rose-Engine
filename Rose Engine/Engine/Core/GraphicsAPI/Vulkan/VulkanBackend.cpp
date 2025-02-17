#include "VulkanBackend.h"


const std::vector<const char*> validationLayers = {
	"VK_LAYER_KHRONOS_validation"
};

const std::vector<const char*> deviceExtensions = {
	// required device extensions
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

// only enable validation layers in debug mode
#ifdef NDEBUG
constexpr bool enableValidationLayers = false;
#else
constexpr bool enableValidationLayers = true;
#endif


void VulkanBackend::initVulkan()
{
	createInstance();
	setupDebugMessenger();
	createSurface();
	pickPhysicalDevice();
	createLogicalDevice();
	createSwapChain();
	createImageViews();
	createRenderPass();
	createGraphicsPipeline();
	createFrameBuffers();
	createCommandPool();
	createCommandBuffer();
	createSyncObjects();
}

void VulkanBackend::vulkanRenderMainLoop()
{
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		drawFrame();

	}

	vkDeviceWaitIdle(logicalDevice); // wait for the logical device to finish operations before exiting mainLoop
}

void VulkanBackend::cleanUp()
{
	vkDestroySemaphore(logicalDevice, imageAvailableSemaphore, nullptr);
	vkDestroySemaphore(logicalDevice, renderFinishedSemaphore, nullptr);
	vkDestroyFence(logicalDevice, inFlightFence, nullptr);
	// vulkan instance must be destroyed right before program exits, all other vulkan resources must be destroyed before the instance destroyed
	vkDestroyCommandPool(logicalDevice, commandPool, nullptr);
	for (auto framebuffer : swapChainFramebuffers) {
		vkDestroyFramebuffer(logicalDevice, framebuffer, nullptr);
	}
	vkDestroyPipeline(logicalDevice, graphicsPipeline, nullptr);
	vkDestroyPipelineLayout(logicalDevice, pipelineLayout, nullptr);
	for (auto imageView : swapChainImageViews) {
		vkDestroyImageView(logicalDevice, imageView, nullptr);
	}

	vkDestroyRenderPass(logicalDevice, renderPass, nullptr);

	vkDestroySwapchainKHR(logicalDevice, swapChain, nullptr);
	vkDestroyDevice(logicalDevice, nullptr);
	if (enableValidationLayers) {
		destroyDebugUtilsMessengerEXT(vulkanInstance, debugMessenger, nullptr);
	}
	vkDestroySurfaceKHR(vulkanInstance, vulkanWindowSurface, nullptr);
	vkDestroyInstance(vulkanInstance, nullptr);
	glfwDestroyWindow(window);
	glfwTerminate();
}

int VulkanBackend::createVulkanWindow()
{
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	

	
	
	window = glfwCreateWindow(1280, 720, "Vulkan window", nullptr, nullptr);
	
	

	 if (!glfwVulkanSupported())
	 {
		 std::exception GLFWWindowError("GLFW: Vulkan Not Supported\n");
		 std::cout << GLFWWindowError.what() << std::endl;
		 return 1;
	 }

	 

	 

	  
	 return 0;
}

void VulkanBackend::createInstance()
{
	if (enableValidationLayers && !checkValidationLayerSupport()) {
		std::runtime_error validationLayersError("validation layers requested, but not available!");
		std::cout << validationLayersError.what() << std::endl;
	}
	
		
	
	// initialize vulkan library by creating it's instance
	VkApplicationInfo appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO; // sType stands for structure type
	appInfo.pNext = nullptr; // pNext is a pointer which is pointing to extension information, that will be useful later
	appInfo.pApplicationName = "Counter Fuck: blood and gore";
	appInfo.applicationVersion = VK_MAKE_API_VERSION(0, 1, 3, 0);
	appInfo.pEngineName = "Rose Engine";
	appInfo.engineVersion = VK_MAKE_API_VERSION(0, 1, 3, 0);
	appInfo.apiVersion = VK_API_VERSION_1_4;

	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;
	

	

	auto extensions = getRequiredExtensions(); // hmm, for some reason i enable KHR_surface here, but i should do it after instance creation
												// because it can actually influence physical device selection
	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data(); // ppEnabledExtensionsName are used to enable extensions for a given type, VkDevice for example, etc
	
	// so basically by setting up debug messenger here using populate populateDebugMessengerCreateInfo 
	// it will automatically be used for every vkCreateInstance and vkDestroyInstance calls, so we can now track what happens 
	// with every instance in our game
	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {};
	if (enableValidationLayers) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();

		populateDebugMessengerCreateInfo(debugCreateInfo);
		createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;
	}
	else {
		createInfo.enabledLayerCount = 0;
		createInfo.pNext = nullptr;
	}

	if (vkCreateInstance(&createInfo, nullptr, &vulkanInstance) != VK_SUCCESS) {
		std::runtime_error vulkanInstanceError("failed to create instance!");
		std::cout << vulkanInstanceError.what() << std::endl;
	}
}

void VulkanBackend::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& debugUtilsMessengerCreateInfo)
{
	// so i guess this is gonna function like a logger with a wide wariety of information, like vulkan warnings, errors and all other crap, 
	// we can manually specify what exactly we want to be logged using messageSeverity and messageType fields
	debugUtilsMessengerCreateInfo = {};
	debugUtilsMessengerCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	debugUtilsMessengerCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	debugUtilsMessengerCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	debugUtilsMessengerCreateInfo.pfnUserCallback = debugCallback; // our own debug callback
	
}

void VulkanBackend::setupDebugMessenger()
{
	if (!enableValidationLayers) {
		return;
	}
	VkDebugUtilsMessengerCreateInfoEXT createInfo{};
	populateDebugMessengerCreateInfo(createInfo);


	if (createDebugUtilsMessengerEXT(vulkanInstance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
		std::runtime_error debugMessengerError("failed to set up debug messenger!");
		std::cout << debugMessengerError.what() << std::endl;
	}
}

void VulkanBackend::createSurface()
{

	VkResult err = glfwCreateWindowSurface(vulkanInstance, window, nullptr, &vulkanWindowSurface);
	if (err != VK_SUCCESS) {
		std::runtime_error createSurfaceError("failed to create window surface!");
		std::cout <<  createSurfaceError.what() << std::endl;
	}
}

void VulkanBackend::pickPhysicalDevice()
{
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(vulkanInstance, &deviceCount, nullptr);

	if (deviceCount == 0) {
		std::runtime_error deviceCountError("failed to find GPUs with Vulkan support!");
		std::cout << deviceCountError.what() << std::endl;
	}
	std::vector<VkPhysicalDevice> physicalGPUs(deviceCount);
	vkEnumeratePhysicalDevices(vulkanInstance, &deviceCount, physicalGPUs.data());

	for (const auto& GPU : physicalGPUs) {
		if (isDeviceSuitable(GPU)) {
			physicalDevice = GPU;
			break;
		}
	}
	if (physicalDevice == VK_NULL_HANDLE) {
		std::runtime_error physicalDeviceError("failed to find a suitable GPU!");
		std::cout << physicalDeviceError.what() << std::endl;
	}
}

void VulkanBackend::createLogicalDevice()
{
	QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfoStructures;
	std::set<uint32_t> uniqueQueueFamilies = { 
		indices.graphicsFamily.value(), indices.presentFamily.value() 
	};
	float queuePriority = 1.0f;
	for (uint32_t queueFamily : uniqueQueueFamilies) {
		VkDeviceQueueCreateInfo queueCreateInfo{};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfoStructures.push_back(queueCreateInfo);
	}
	

	VkPhysicalDeviceFeatures logicalDeviceFeatures{};

	VkDeviceCreateInfo logicalDeviceCreateInfo{};
	logicalDeviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	logicalDeviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfoStructures.size());
	logicalDeviceCreateInfo.pQueueCreateInfos = queueCreateInfoStructures.data();

	logicalDeviceCreateInfo.pEnabledFeatures = &logicalDeviceFeatures;
	logicalDeviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size()); // enable swapchain extension for gpu
	logicalDeviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data(); // enable required extensions

	if (vkCreateDevice(physicalDevice, &logicalDeviceCreateInfo, nullptr, &logicalDevice) != VK_SUCCESS) {
		std::runtime_error logicalDeviceError("Failed to create logical device");
		std::cout << logicalDeviceError.what() << std::endl;
	}
	vkGetDeviceQueue(logicalDevice, indices.graphicsFamily.value(), 0, &graphicsQueue);
	vkGetDeviceQueue(logicalDevice, indices.presentFamily.value(), 0, &presentationQueue);
}

void VulkanBackend::createSwapChain()
{
	SwapChainSupportDetails swapChainSupport = querySwapChainSupportDetails(physicalDevice);
	VkSurfaceFormatKHR surfaceFormat = chooseSwapChainSurfaceFormat(swapChainSupport.surfaceFormats);
	VkPresentModeKHR presentMode = choosePresentationMode(swapChainSupport.presentationModes);
	VkExtent2D imageExtent = chooseSwapExtent(swapChainSupport.surfaceCapabilities);

	uint32_t imageCount = swapChainSupport.surfaceCapabilities.minImageCount + 1; // minimum number of images in swapChain for it to function
	//if for some reason maximum number of images in swapchain is unlimited - limit it by maxImageCount
	if (swapChainSupport.surfaceCapabilities.maxImageCount > 0 && imageCount > swapChainSupport.surfaceCapabilities.maxImageCount) {
		imageCount = swapChainSupport.surfaceCapabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR swapChainCreateInfo{};
	swapChainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapChainCreateInfo.surface = vulkanWindowSurface;

	swapChainCreateInfo.minImageCount = imageCount;
	swapChainCreateInfo.imageFormat = surfaceFormat.format;
	swapChainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
	swapChainCreateInfo.imageExtent = imageExtent;
	swapChainCreateInfo.imageArrayLayers = 1;
	swapChainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT; // bit field specifies what kind of operations we�ll use the images in the swap chain for

	QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
	uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };
	if (indices.graphicsFamily != indices.presentFamily) {
		swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		swapChainCreateInfo.queueFamilyIndexCount = 2;
		swapChainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else {
		swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		swapChainCreateInfo.queueFamilyIndexCount = 0; // Optional
		swapChainCreateInfo.pQueueFamilyIndices = nullptr; // Optional
	}
	swapChainCreateInfo.preTransform = swapChainSupport.surfaceCapabilities.currentTransform;
	swapChainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR; // ignore alpha channel in image
	swapChainCreateInfo.presentMode = presentMode;
	swapChainCreateInfo.clipped = VK_TRUE;
	swapChainCreateInfo.oldSwapchain = VK_NULL_HANDLE; // With Vulkan it�s possible that your swap chain becomes invalid or unoptimized while your application is running, for example because the window was resized. 
													   // In that case the swap chain actually needs to be recreated from scratch and a reference to the old one must be specified in this field.
	if (vkCreateSwapchainKHR(logicalDevice, &swapChainCreateInfo, nullptr, &swapChain) != VK_SUCCESS) {
		std::runtime_error swapChainError("failed to create swap chain!");
		std::cout << swapChainError.what() << std::endl;
	}
	// resize image buffer
	vkGetSwapchainImagesKHR(logicalDevice, swapChain, &imageCount, nullptr);
	swapChainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(logicalDevice, swapChain, &imageCount, swapChainImages.data());


	swapChainImageFormat = surfaceFormat.format;
	swapChainExtent = imageExtent;
}
void VulkanBackend::createImageViews()
{
	swapChainImageViews.resize(swapChainImages.size());

	for (size_t i = 0; i < swapChainImages.size(); i++) {
		VkImageViewCreateInfo imageViewCreateInfo = {}; // this image view is gonna be used as color target for textures
		imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		imageViewCreateInfo.image = swapChainImages[i];
		imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D; // how to treat images, like 2D-textures or 3D-textures
		imageViewCreateInfo.format = swapChainImageFormat;
		imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY; // swizzling is basically combining vectors and thus creating different colors for a texture
		imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		imageViewCreateInfo.subresourceRange.baseMipLevel = 0; // mipmap level
		imageViewCreateInfo.subresourceRange.levelCount = 1;
		imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
		imageViewCreateInfo.subresourceRange.layerCount = 1;

		if (vkCreateImageView(logicalDevice, &imageViewCreateInfo, nullptr, &swapChainImageViews[i]) != VK_SUCCESS) {
			std::runtime_error imageViewError("failed to create image views");
			std::cout << imageViewError.what() << std::endl;
		}
	}
}
void VulkanBackend::createGraphicsPipeline()
{
	const char shaderText[] = { "#version 450\n"
							   "layout(location = 0) in vec3 fragColor;\n"
							   "layout(location = 0) out vec4 outColor;\n"
							   "void main() {outColor = vec4(fragColor, 1.0);\n"
								"}\n" 
	};
	
	auto vertexShaderCode = shaderCompiler.readFile("D:/Programming/Rose Engine/Rose Engine/Engine/Core/Shaders/vertexShader.spv");
	auto fragmentShaderCode = shaderCompiler.readFile("D:/Programming/Rose Engine/Rose Engine/Engine/Core/Shaders/fragmentShader.spv");

	VkShaderModule vertexShaderModule = createShaderModule(vertexShaderCode);
	VkShaderModule fragmentShaderModule = createShaderModule(fragmentShaderCode);


	VkPipelineShaderStageCreateInfo vertexShaderStageCreateInfo = {};
	vertexShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertexShaderStageCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertexShaderStageCreateInfo.module = vertexShaderModule;
	vertexShaderStageCreateInfo.pName = "main"; // function name to invoke inside shader module code

	// same but this time for fragment shader
	VkPipelineShaderStageCreateInfo fragmentShaderStageCreateInfo{};
	fragmentShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragmentShaderStageCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragmentShaderStageCreateInfo.module = fragmentShaderModule;
	fragmentShaderStageCreateInfo.pName = "main";

	VkPipelineShaderStageCreateInfo shaderStages[] = { vertexShaderStageCreateInfo, fragmentShaderStageCreateInfo }; // array with these 2 structs for future references

	VkPipelineVertexInputStateCreateInfo vertexInputCreateInfo = {}; // bindings for geometry
	vertexInputCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputCreateInfo.vertexBindingDescriptionCount = 0;
	vertexInputCreateInfo.pVertexBindingDescriptions = nullptr;
	vertexInputCreateInfo.vertexAttributeDescriptionCount = 0;
	vertexInputCreateInfo.pVertexAttributeDescriptions = nullptr;

	VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo = {};
	inputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST; // topology, for now we gonna draw triangles
	inputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;

	std::vector<VkDynamicState> dynamicStates = {
	VK_DYNAMIC_STATE_VIEWPORT,
	VK_DYNAMIC_STATE_SCISSOR
	};

	VkPipelineDynamicStateCreateInfo dynamicState = {};
	dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
	dynamicState.pDynamicStates = dynamicStates.data();

	VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
	pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutCreateInfo.setLayoutCount = 0; // Optional
	pipelineLayoutCreateInfo.pSetLayouts = nullptr; // Optional
	pipelineLayoutCreateInfo.pushConstantRangeCount = 0; // Optional
	pipelineLayoutCreateInfo.pPushConstantRanges = nullptr; // Optional

	if (vkCreatePipelineLayout(logicalDevice, &pipelineLayoutCreateInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
		std::runtime_error pipelineLayoutError("failed to create pipeline layout!");
		std::cerr << pipelineLayoutError.what() << std::endl;
	}


	VkPipelineViewportStateCreateInfo viewPortState = {};
	viewPortState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewPortState.viewportCount = 1;
	viewPortState.scissorCount = 1;

	// Rasterizer
	VkPipelineRasterizationStateCreateInfo rasterizer = {};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE; // this should be true for shadow maps later
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL; // how fragments are generated for geometry modes
	rasterizer.lineWidth = 1.0f; // line thickness, don't know what it's for, gonna learn later
	rasterizer.cullMode = VK_CULL_MODE_BACK_BIT; // everything that is behind the camera is discarded, as far as i understand
	rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE; // vertex with negative value is considered front-facing
	// lines below gonna be used for shadow maps later, false for now
	rasterizer.depthBiasEnable = VK_FALSE;
	rasterizer.depthBiasConstantFactor = 0.0f; // Optional
	rasterizer.depthBiasClamp = 0.0f; // Optional
	rasterizer.depthBiasSlopeFactor = 0.0f; // Optional


	// Multisampling
	VkPipelineMultisampleStateCreateInfo multisampling{};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisampling.minSampleShading = 1.0f; // Optional
	multisampling.pSampleMask = nullptr; // Optional
	multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
	multisampling.alphaToOneEnable = VK_FALSE; // Optional
	
	// Color blending
	// for now it is disabled, i'm gonna look into that later

	VkPipelineColorBlendAttachmentState colorBlendAttachment{};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_FALSE;

	VkPipelineColorBlendStateCreateInfo colorBlending{};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	colorBlending.blendConstants[0] = 0.0f; // Optional
	colorBlending.blendConstants[1] = 0.0f; // Optional
	colorBlending.blendConstants[2] = 0.0f; // Optional
	colorBlending.blendConstants[3] = 0.0f; // Optional

	
	// Graphics pipeline
	VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo = {};
	graphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	graphicsPipelineCreateInfo.stageCount = 2;
	graphicsPipelineCreateInfo.pStages = shaderStages;
	graphicsPipelineCreateInfo.pVertexInputState = &vertexInputCreateInfo;
	graphicsPipelineCreateInfo.pInputAssemblyState = &inputAssemblyStateCreateInfo;
	graphicsPipelineCreateInfo.pViewportState = &viewPortState;
	graphicsPipelineCreateInfo.pRasterizationState = &rasterizer;
	graphicsPipelineCreateInfo.pMultisampleState = &multisampling;
	graphicsPipelineCreateInfo.pDepthStencilState = nullptr; // Optional
	graphicsPipelineCreateInfo.pColorBlendState = &colorBlending;
	graphicsPipelineCreateInfo.pDynamicState = &dynamicState;
	graphicsPipelineCreateInfo.layout = pipelineLayout;
	graphicsPipelineCreateInfo.renderPass = renderPass;
	graphicsPipelineCreateInfo.subpass = 0;
	graphicsPipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE; // this can be used for extending the second or third, etc graphics pipeline in future
	graphicsPipelineCreateInfo.basePipelineIndex = -1;

	if (vkCreateGraphicsPipelines(logicalDevice, VK_NULL_HANDLE, 1, &graphicsPipelineCreateInfo, nullptr, &graphicsPipeline) != VK_SUCCESS) {
		std::runtime_error graphicsPipelineError("failed to create graphics pipeline!");
		std::cerr << graphicsPipelineError.what() << std::endl;
	}

	
	//auto compiledToAssembly = shaderCompiler.compileShader(shaderText, shaderc_fragment_shader, "shader.fragment");
	//shaderCompiler.readGlslShaderText();


	vkDestroyShaderModule(logicalDevice, fragmentShaderModule, nullptr);
	vkDestroyShaderModule(logicalDevice, vertexShaderModule, nullptr);
}
void VulkanBackend::createRenderPass()
{
	VkAttachmentDescription colorAttachment = {};
	colorAttachment.format = swapChainImageFormat;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	// render subpasses
	VkAttachmentReference colorAttachmentRef{};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;

	VkSubpassDependency dependency{};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	// renderPass
	VkRenderPassCreateInfo renderPassCreateInfo = {};
	renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassCreateInfo.attachmentCount = 1;
	renderPassCreateInfo.pAttachments = &colorAttachment;
	renderPassCreateInfo.subpassCount = 1;
	renderPassCreateInfo.pSubpasses = &subpass;

	renderPassCreateInfo.dependencyCount = 1;
	renderPassCreateInfo.pDependencies = &dependency;

	if (vkCreateRenderPass(logicalDevice, &renderPassCreateInfo, nullptr, &renderPass) != VK_SUCCESS) {
		std::runtime_error renderPassError("failed to create render pass!");
		std::cerr << renderPassError.what() << std::endl;
	}
}
void VulkanBackend::createFrameBuffers()
{
	swapChainFramebuffers.resize(swapChainImageViews.size()); // resize to hold all of the frameBuffers

	for (size_t i = 0; i < swapChainImageViews.size(); i++) {
		VkImageView attachments[] = {
		swapChainImageViews[i]
		};


		VkFramebufferCreateInfo frameBufferCreateInfo = {};
		frameBufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		frameBufferCreateInfo.renderPass = renderPass;
		frameBufferCreateInfo.attachmentCount = 1;
		frameBufferCreateInfo.pAttachments = attachments;
		frameBufferCreateInfo.width = swapChainExtent.width;
		frameBufferCreateInfo.height = swapChainExtent.height;
		frameBufferCreateInfo.layers = 1;

		if (vkCreateFramebuffer(logicalDevice, &frameBufferCreateInfo, nullptr, &swapChainFramebuffers[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to create framebuffer!");
		}
	}
}
void VulkanBackend::createCommandPool()
{
	QueueFamilyIndices queueFamilyIndices = findQueueFamilies(physicalDevice);

	VkCommandPoolCreateInfo commandPoolCreateInfo = {};
	commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	commandPoolCreateInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

	if (vkCreateCommandPool(logicalDevice, &commandPoolCreateInfo, nullptr, &commandPool) != VK_SUCCESS) {
		std::runtime_error commandPoolError("failed to create command pool!");
		std::cout << commandPoolError.what() << std::endl;
	}
}
void VulkanBackend::createCommandBuffer()
{
	VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};
	commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	commandBufferAllocateInfo.commandPool = commandPool;
	commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	commandBufferAllocateInfo.commandBufferCount = 1;

	if (vkAllocateCommandBuffers(logicalDevice, &commandBufferAllocateInfo, &commandBuffer) != VK_SUCCESS) {
		std::runtime_error commandBufferAllocateError("failed to allocate command buffers!");
		std::cout << commandBufferAllocateError.what() << std::endl;
	}
}
void VulkanBackend::recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex)
{
	VkCommandBufferBeginInfo commandBufferBeginInfo{};
	commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	commandBufferBeginInfo.flags = 0; // Optional
	commandBufferBeginInfo.pInheritanceInfo = nullptr; // Optional

	if (vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo) != VK_SUCCESS) {
		std::runtime_error commandBufferRecordingError("failed to begin recording command buffer!");
		std::cout << commandBufferRecordingError.what() << std::endl;
	}

	// here we begin drawing
	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = renderPass;
	renderPassInfo.framebuffer = swapChainFramebuffers[imageIndex];

	renderPassInfo.renderArea.offset = { 0, 0 }; // size of render area
	renderPassInfo.renderArea.extent = swapChainExtent;

	VkClearValue clearColor = { {{0.0f, 0.0f, 0.0f, 1.0f}} };
	renderPassInfo.clearValueCount = 1;
	renderPassInfo.pClearValues = &clearColor;

	vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(swapChainExtent.width);
	viewport.height = static_cast<float>(swapChainExtent.height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

	VkRect2D scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent = swapChainExtent;
	vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

	// actual drawing operation
	vkCmdDraw(commandBuffer, 3, 1, 0, 0);
	vkCmdEndRenderPass(commandBuffer);

	if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
		std::runtime_error vulkanEndCommandBufferError("failed to record command buffer!");
		std::cout << vulkanEndCommandBufferError.what() << std::endl;
	}
}
void VulkanBackend::drawFrame()
{
	vkWaitForFences(logicalDevice, 1, &inFlightFence, VK_TRUE, UINT_FAST64_MAX);
	vkResetFences(logicalDevice, 1, &inFlightFence);
	uint32_t imageIndex;
	vkAcquireNextImageKHR(logicalDevice, swapChain, UINT_FAST64_MAX, imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);
	vkResetCommandBuffer(commandBuffer, 0);
	recordCommandBuffer(commandBuffer, imageIndex);

	

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = { imageAvailableSemaphore };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;
	VkSemaphore signalSemaphores[] = { renderFinishedSemaphore };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFence) != VK_SUCCESS) {
		std::runtime_error graphicsQueueSubmissionError("failed to submit draw command buffer!");
		std::cout << graphicsQueueSubmissionError.what() << std::endl;
	}

	VkPresentInfoKHR presentationInfo{};
	presentationInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	presentationInfo.waitSemaphoreCount = 1;
	presentationInfo.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapChains[] = { swapChain };
	presentationInfo.swapchainCount = 1;
	presentationInfo.pSwapchains = swapChains;
	presentationInfo.pImageIndices = &imageIndex;

	vkQueuePresentKHR(presentationQueue, &presentationInfo);
}
void VulkanBackend::createSyncObjects()
{
	VkSemaphoreCreateInfo semaphoreCreateInfo = {};
	semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceCreateInfo = {};
	fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	if (vkCreateSemaphore(logicalDevice, &semaphoreCreateInfo, nullptr, &imageAvailableSemaphore) != VK_SUCCESS ||
		vkCreateSemaphore(logicalDevice, &semaphoreCreateInfo, nullptr, &renderFinishedSemaphore) != VK_SUCCESS ||
		vkCreateFence(logicalDevice, &fenceCreateInfo, nullptr, &inFlightFence) != VK_SUCCESS) {
		std::runtime_error semaphoreCreationError("failed to create semaphores!");
		std::cout << semaphoreCreationError.what() << std::endl;
	}
}
bool VulkanBackend::isDeviceSuitable(VkPhysicalDevice GPU)
{
	
	// get a suitable physical GPU, in this function we can add more checks for GPUs
	// and output various GPU information for debug purposes
	VkPhysicalDeviceProperties gpuProperties;
	vkGetPhysicalDeviceProperties(GPU, &gpuProperties);

	if (enableValidationLayers) {
		std::cerr << "Selected device: " << gpuProperties.deviceName << std::endl;
		std::cout << "NVIDIA Driver version : " << gpuProperties.driverVersion << std::endl; // i should test it for nvidia GPU
		gpuInfo.InitializeAGSLib(); // print gpu information for debug
	}

	VkPhysicalDeviceFeatures gpuFeatures;
	vkGetPhysicalDeviceFeatures(GPU, &gpuFeatures);
	// does GPU support commands that we need?
	QueueFamilyIndices indices = findQueueFamilies(GPU);
	bool extensionsSupported = checkDeviceExtensionSupport(GPU);

	bool swapChainAdequate = false;
	if (extensionsSupported) {
		SwapChainSupportDetails swapChainSupportDetails = querySwapChainSupportDetails(GPU);
		// basically gpu must support at least one surface format and one presentation mode i guess
		swapChainAdequate = !swapChainSupportDetails.surfaceFormats.empty() && !swapChainSupportDetails.presentationModes.empty();
	}
	return indices.isComplete() && extensionsSupported && swapChainAdequate; 
}

bool VulkanBackend::checkDeviceExtensionSupport(VkPhysicalDevice GPU)
{
	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(GPU, nullptr, &extensionCount, nullptr);

	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(GPU, nullptr, &extensionCount, availableExtensions.data());
	
	std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

	for (const auto& extension : availableExtensions) {
		requiredExtensions.erase(extension.extensionName);
	}

	return requiredExtensions.empty();
}

QueueFamilyIndices VulkanBackend::findQueueFamilies(VkPhysicalDevice GPU)
{
	//Queues are essentially simply accept different commands supported by GPUs and as far as i can see
	// all the gpus support 4 most common flags which are - VK_QUEUE_GRAPHICS_BIT, VK_QUEUE_COMPUTE_BIT
	// VK_QUEUE_TRANSFER_BIT and VK_QUEUE_SPARSE_BINDING_BIT
	
	QueueFamilyIndices indices;
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(GPU, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(GPU, &queueFamilyCount, queueFamilies.data());

	int i = 0;
	for (const auto& queueFamily : queueFamilies) {
		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			indices.graphicsFamily = i;
		}
		VkBool32 presentationSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(GPU, i, vulkanWindowSurface, &presentationSupport);

		if (presentationSupport) {
			indices.presentFamily = i;
		}

		if (indices.isComplete()) {
			break;
		}
		i++;
	}
	return indices;

	
}

SwapChainSupportDetails VulkanBackend::querySwapChainSupportDetails(VkPhysicalDevice GPU) {
	SwapChainSupportDetails swapChainDetails;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(GPU, vulkanWindowSurface, &swapChainDetails.surfaceCapabilities);

	// get supported surface format
	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(GPU, vulkanWindowSurface, &formatCount, nullptr);

	if (formatCount != 0) {
		swapChainDetails.surfaceFormats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(GPU, vulkanWindowSurface, &formatCount, swapChainDetails.surfaceFormats.data());
	}

	// get supported presentation modes
	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(GPU, vulkanWindowSurface, &presentModeCount, nullptr);


	if (presentModeCount != 0) {
		swapChainDetails.presentationModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(GPU, vulkanWindowSurface, &presentModeCount, swapChainDetails.presentationModes.data());
	}
	return swapChainDetails;
}

VkResult VulkanBackend::createDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
{
	// The vkGetInstanceProcAddr function will return nullptr if the function couldn�t be loaded.
	auto function = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(vulkanInstance, "vkCreateDebugUtilsMessengerEXT");

	if (function != nullptr) {
		return function(instance, pCreateInfo, pAllocator, pDebugMessenger);

	}
	else {
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

void VulkanBackend::destroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT pDebugMessenger, const VkAllocationCallbacks* pAllocator)
{
	// used to destroy the debug messenger
	auto function = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (function != nullptr) {
		function(instance, debugMessenger, pAllocator);
	}
}

bool VulkanBackend::checkValidationLayerSupport()
{
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
	
	for (const char* layerName : validationLayers) {
		bool layerFound = false;

		for (const auto& layerProperties : availableLayers) {
			if (strcmp(layerName, layerProperties.layerName) == 0) {
				layerFound = true;
				break;
			}
		}
		if (!layerFound) {
			return false;
		}
	}
	return true;
}

std::vector<const char*> VulkanBackend::getRequiredExtensions()
{
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	if (enableValidationLayers) {
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}
	return extensions;
}

VkSurfaceFormatKHR VulkanBackend::chooseSwapChainSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
	for (const auto& availableFormat : availableFormats) {
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
			return availableFormat;
		}
	}
	return availableFormats[0];
}

VkPresentModeKHR VulkanBackend::choosePresentationMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
{
	// check if VK_PRESENT_MODE_MAILBOX_KHR supported, which is tripple buffering mode, unlocked FPS
	for (const auto& availablePresentMode : availablePresentModes) {
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
			return availablePresentMode;
		}
	}
	return VK_PRESENT_MODE_FIFO_KHR; // if not, use basically vSync
}

VkExtent2D VulkanBackend::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
{
	// swap extent is a resolution of images in swapchain
	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
		return capabilities.currentExtent;
	}
	else {
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);

		VkExtent2D actualExtent = {
			static_cast<uint32_t>(width),
			static_cast<uint32_t>(height)
		};

		actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
		actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

		return actualExtent;
	}
}

VKAPI_ATTR VkBool32 VKAPI_CALL VulkanBackend::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
{
	// this is for debugging, probably gonna use it in logger
	std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
	

	return VK_FALSE;
}

VkShaderModule VulkanBackend::createShaderModule(const std::vector<char>& code)
{
	VkShaderModuleCreateInfo shaderCreateInfo = {};
	shaderCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	shaderCreateInfo.codeSize = code.size();
	shaderCreateInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

	VkShaderModule shaderModule;
	if (vkCreateShaderModule(logicalDevice, &shaderCreateInfo, nullptr, &shaderModule) != VK_SUCCESS) {
		std::runtime_error shaderModuleError("failed to create shader module!");
		std::cerr << shaderModuleError.what() << std::endl;
	}
	return shaderModule;
}
