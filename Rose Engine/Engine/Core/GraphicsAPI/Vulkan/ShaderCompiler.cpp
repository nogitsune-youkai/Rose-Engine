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

std::string ShaderCompiler::readGlslShaderText(std::filesystem::path)
{
	return std::string();
}

std::string ShaderCompiler::defineShaderType()
{
	return std::string();
}
