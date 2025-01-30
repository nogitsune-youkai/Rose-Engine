#include "ShaderCompiler.h"

#ifdef NDEBUG
constexpr bool enableDebugFacilities = false;
#else
constexpr bool enableDebugFacilities = true;
#endif




std::string ShaderCompiler::processShaderFile(const std::string& shaderSourceFile, shaderc_shader_kind shaderKind, const std::string& shaderName)
{
	
	compileOptions.AddMacroDefinition("MY_DEFINE", "1");
	shaderc::PreprocessedSourceCompilationResult compilationResult = compiler.PreprocessGlsl(shaderSourceFile, shaderKind, shaderName.c_str(), compileOptions);

	if (enableDebugFacilities) {
		if (compilationResult.GetCompilationStatus() != shaderc_compilation_status_success) {
			std::cerr << compilationResult.GetErrorMessage();
			return "";
		}
	}
	std::string processingresult { compilationResult.cbegin(), compilationResult.cend() };
	return processingresult;
}

std::string ShaderCompiler::compileShaderToAssembly(const std::string& shaderSourceFile, shaderc_shader_kind shaderKind, const std::string& shaderName)
{
	
	if (enableDebugFacilities) {
		compileOptions.SetOptimizationLevel(shaderc_optimization_level_zero);
	}
	else {
		compileOptions.SetOptimizationLevel(shaderc_optimization_level_performance);
	}

	shaderc::AssemblyCompilationResult assemblyCompilationResult = compiler.CompileGlslToSpvAssembly(shaderSourceFile, shaderKind, shaderName.c_str(), compileOptions);

	if (assemblyCompilationResult.GetCompilationStatus() != shaderc_compilation_status_success) {
		std::cerr << assemblyCompilationResult.GetErrorMessage();
		return "";
	}
	else {
		std::cerr << "shader has been sucessfully compiled to Spv assembly" << std::endl;
	}

	return { assemblyCompilationResult.cbegin(), assemblyCompilationResult.cend() };
}



std::vector<uint32_t> ShaderCompiler::compileShader(const std::string& shaderSourceFile, shaderc_shader_kind shaderKind, const std::string& shaderName)
{
	processShaderFile(shaderSourceFile, shaderKind, shaderName.c_str());
	compileShaderToAssembly(shaderSourceFile, shaderKind, shaderName.c_str());
	shaderc::SpvCompilationResult shaderModule = compiler.CompileGlslToSpv(shaderSourceFile, shaderKind, shaderName.c_str());
	if (enableDebugFacilities) {
		compileOptions.SetOptimizationLevel(shaderc_optimization_level_zero);
	}
	else {
		compileOptions.SetOptimizationLevel(shaderc_optimization_level_performance);
	}

	if (shaderModule.GetCompilationStatus() != shaderc_compilation_status_success) {
		std::cerr << shaderModule.GetErrorMessage();
		return std::vector<uint32_t>();
	}
	else {
		std::cerr << "Shader compilation finished sucessfully" << std::endl;
	}
	return { shaderModule.cbegin(), shaderModule.cend() };
}

std::vector<char> ShaderCompiler::readFile(const std::string& filename)
{
	std::ifstream file(filename, std::ios::ate | std::ios::binary);

	if (!file.is_open()) {
		throw std::runtime_error("failed to open file!");
	}
	size_t fileSize = (size_t)file.tellg();
	std::vector<char> buffer(fileSize);

	file.seekg(0);
	file.read(buffer.data(), fileSize);

	file.close();

	return buffer;
}

//const char ShaderCompiler::readGlslShaderText()
//{
//	 this is work in progress, will be probably implemented at later stages
//	std::filesystem::path shaderDirectory = std::filesystem::current_path() / "Shaders";
//	bool directoryExists = std::filesystem::exists(shaderDirectory);
//	if (!directoryExists) {
//		directoryExists = std::filesystem::create_directories(shaderDirectory);
//	}
//	
//	std::filesystem::path shadersDirectory = std::filesystem::current_path().append("Engine").append("Core").append("Shaders"); // L"D:\\Programming\\Rose Engine\\Rose Engine"
//	std::filesystem::path fileNames = shadersDirectory.filename().string();
//	std::cerr << fileNames.string();
//}

std::string ShaderCompiler::defineShaderType()
{
	return std::string();
}
